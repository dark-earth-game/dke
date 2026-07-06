#include <Cocoa/Cocoa.h>
#include <AudioToolbox/AudioQueue.h>
#include <mach-o/dyld.h>
#include <mach/mach_time.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#include "../system.h"
#include "../common.h"
#include "../audio/audio.h"
// #include "platform.h"

/* =========================================================================
 * CoreAudio AudioQueue streaming backend
 * =========================================================================
 *
 * 4 queue buffers of AQ_BUF_SIZE bytes each.  A free-list protected by a
 * pthread_mutex + condvar hands buffers to ls_platform_audio_push.
 * The AudioQueue callback (aq_callback) returns finished buffers to the
 * free-list.  If no buffer is free within a short spin, the push drops
 * the block and returns 0 (caller retries on the next frame).
 * ========================================================================= */

#define AQ_NUM_BUFS   8
#define AQ_BUF_SIZE   16384   /* bytes per buffer (~186 ms @ 22050 Hz stereo i16) —
                                  large enough to hold one CRH movie frame's PCM
                                  (~5880 bytes) plus headroom */

static AudioQueueRef        aq_queue   = NULL;
static AudioQueueBufferRef  aq_bufs[AQ_NUM_BUFS];

/* Free-list */
static AudioQueueBufferRef  aq_free[AQ_NUM_BUFS];
static i32                  aq_free_count = 0;
static pthread_mutex_t      aq_mutex  = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t       aq_cond   = PTHREAD_COND_INITIALIZER;

static b8 aq_open = false;

/* AudioQueue callback — called from CoreAudio's internal thread */
static void aq_callback(void *user_data,
                        AudioQueueRef queue,
                        AudioQueueBufferRef buf)
{
    (void)user_data;
    (void)queue;
    pthread_mutex_lock(&aq_mutex);
    if (aq_free_count < AQ_NUM_BUFS) {
        aq_free[aq_free_count++] = buf;
    }
    pthread_cond_signal(&aq_cond);
    pthread_mutex_unlock(&aq_mutex);
}

b8 ls_platform_audio_open(i32 sample_rate, i32 channels, i32 bit_depth)
{
    if (aq_open) return true;   /* already open */

    AudioStreamBasicDescription fmt;
    memset(&fmt, 0, sizeof(fmt));
    fmt.mSampleRate       = (Float64)sample_rate;
    fmt.mFormatID         = kAudioFormatLinearPCM;
    fmt.mFormatFlags      = kAudioFormatFlagIsSignedInteger
                          | kAudioFormatFlagIsPacked;
    /* Little-endian on all Apple silicon / x86 hosts — no endian flag needed */
    fmt.mBitsPerChannel   = (UInt32)bit_depth;
    fmt.mChannelsPerFrame = (UInt32)channels;
    fmt.mBytesPerFrame    = (UInt32)(channels * (bit_depth / 8));
    fmt.mFramesPerPacket  = 1;
    fmt.mBytesPerPacket   = fmt.mBytesPerFrame;

    OSStatus err = AudioQueueNewOutput(&fmt, aq_callback, NULL,
                                       NULL, NULL, 0, &aq_queue);
    if (err != noErr) {
        printf("ls_platform_audio_open: AudioQueueNewOutput failed (%d)\n", (int)err);
        aq_queue = NULL;
        return false;
    }

    /* Allocate buffers and pre-fill with silence */
    pthread_mutex_lock(&aq_mutex);
    aq_free_count = 0;
    for (i32 i = 0; i < AQ_NUM_BUFS; i++) {
        err = AudioQueueAllocateBuffer(aq_queue, AQ_BUF_SIZE, &aq_bufs[i]);
        if (err != noErr) {
            printf("ls_platform_audio_open: AllocateBuffer[%d] failed (%d)\n", i, (int)err);
            pthread_mutex_unlock(&aq_mutex);
            AudioQueueDispose(aq_queue, true);
            aq_queue = NULL;
            return false;
        }
        /* Pre-fill with silence to avoid initial underrun */
        memset(aq_bufs[i]->mAudioData, 0, AQ_BUF_SIZE);
        aq_bufs[i]->mAudioDataByteSize = AQ_BUF_SIZE;
        AudioQueueEnqueueBuffer(aq_queue, aq_bufs[i], 0, NULL);
    }
    pthread_mutex_unlock(&aq_mutex);

    err = AudioQueueStart(aq_queue, NULL);
    if (err != noErr) {
        printf("ls_platform_audio_open: AudioQueueStart failed (%d)\n", (int)err);
        AudioQueueDispose(aq_queue, true);
        aq_queue = NULL;
        return false;
    }

    aq_open = true;
    printf("ls_platform_audio_open: CoreAudio queue started (%d Hz, %d ch, %d bit)\n",
           sample_rate, channels, bit_depth);
    return true;
}

i32 ls_platform_audio_push(const i16 *pcm, i32 bytes)
{
    if (!aq_open || !aq_queue || !pcm || bytes <= 0) return 0;

    const u8 *src = (const u8 *)pcm;
    i32 remaining = bytes;
    i32 total_queued = 0;

    while (remaining > 0) {
        pthread_mutex_lock(&aq_mutex);

        /* Wait up to ~250 ms for a free buffer — long enough for CoreAudio
         * to drain a queued buffer at any typical sample rate. */
        if (aq_free_count == 0) {
            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_nsec += 250 * 1000000L;
            while (ts.tv_nsec >= 1000000000L) { ts.tv_sec++; ts.tv_nsec -= 1000000000L; }
            pthread_cond_timedwait(&aq_cond, &aq_mutex, &ts);
        }

        if (aq_free_count == 0) {
            /* Queue still full — drop the remainder rather than block forever. */
            pthread_mutex_unlock(&aq_mutex);
            return total_queued;
        }

        AudioQueueBufferRef buf = aq_free[--aq_free_count];
        pthread_mutex_unlock(&aq_mutex);

        i32 chunk = remaining < AQ_BUF_SIZE ? remaining : AQ_BUF_SIZE;
        memcpy(buf->mAudioData, src, (size_t)chunk);
        buf->mAudioDataByteSize = (UInt32)chunk;

        OSStatus err = AudioQueueEnqueueBuffer(aq_queue, buf, 0, NULL);
        if (err != noErr) {
            pthread_mutex_lock(&aq_mutex);
            aq_free[aq_free_count++] = buf;
            pthread_mutex_unlock(&aq_mutex);
            return total_queued;
        }

        src           += chunk;
        remaining     -= chunk;
        total_queued  += chunk;
    }

    return total_queued;
}

void ls_platform_audio_close(void)
{
    if (!aq_open || !aq_queue) return;
    AudioQueueStop(aq_queue, true);   /* true = synchronous drain */
    AudioQueueDispose(aq_queue, true);
    aq_queue = NULL;
    aq_open  = false;
    pthread_mutex_lock(&aq_mutex);
    aq_free_count = 0;
    pthread_mutex_unlock(&aq_mutex);
    printf("ls_platform_audio_close: CoreAudio queue stopped.\n");
}


static NSWindow *window = NULL;
static u8 *surface = NULL;
static NSAutoreleasePool *pool = NULL;


static void handle_events(system_t *system, i32 type, i32 key) {
    // if (key > 0 && key < 256) {
    //     return;
    // }
    // https://web.archive.org/web/20100501161453/http://www.classicteck.com/rbarticles/mackeyboard.php
    switch(key) {
        // w
        case 0x03:
            system->actions[119] = type;
            break;
        // s
        case 0x01:
            system->actions[115] = type;
            break;
        // a
        case 0x00:
            system->actions[97] = type;
            break;
        // d
        case 0x02:
            system->actions[100] = type;
            break;
        // q
        case 0x0C:
            system->actions[113] = type;
            break;
        // e
        case 0x0E:
            system->actions[101] = type;
            break;
        // r
        case 0x0F:
            system->actions[114] = type;
            break;
        // t
        case 0x11:
            system->actions[116] = type;
            break;
        // l
        case 0x25:
            system->actions[108] = type;
            break;
        /* Navigation keys — wired for menu_update (pass 2) */
        // Return / Enter (keyCode 36) → actions[13] (ECX=0x0D in original asm)
        case 0x24:
            system->actions[13] = type;
            break;
        // Escape (keyCode 53) → actions[27] (ECX=0x1B in original asm)
        case 0x35:
            system->actions[27] = type;
            break;
        // Up arrow (keyCode 126) → actions[38] (ECX=0x25 / VK_UP)
        case 0x7E:
            system->actions[38] = type;
            break;
        // Down arrow (keyCode 125) → actions[40] (ECX=0x27 / VK_DOWN)
        case 0x7D:
            system->actions[40] = type;
            break;
        // Left arrow (keyCode 123) → actions[37] (VK_LEFT)
        case 0x7B:
            system->actions[37] = type;
            break;
        // Right arrow (keyCode 124) → actions[39] (VK_RIGHT)
        case 0x7C:
            system->actions[39] = type;
            break;
        // F12 (keyCode 111) → actions[111] for screenshot
        case 0x6F:
            system->actions[111] = type;
            break;
        default:
            break;
    }
}

static void handle_mouse_events(system_t *system, i32 type, i32 x, i32 y) {
    switch(type) {
        case 0:
            break;
        case 1:
            system->motion_x = x;
            system->motion_y = y;
            break;
        default:
            break;
    }
}

@interface WindowDelegate : NSObject <NSWindowDelegate>
@end

@implementation WindowDelegate {
    system_t *system;
}

- (instancetype)initWithSystem:(system_t *)sys {
    self = [super init];
    if (self != nil) {
        system = sys;
    }
    return self;
}

- (BOOL)windowShouldClose:(NSWindow *)sender {
    system->quit = 1;
    return NO;
}

@end



@interface ContentView : NSView
@end

@implementation ContentView {
    system_t *system;
}

- (instancetype)initWithSystem:(system_t *)sys {
    self = [super init];
    if (self != nil) {
        system = sys;
    }
    return self;
}

- (BOOL)acceptsFirstResponder {
    return YES; // keyboard events
}

- (void)drawRect:(NSRect)dirtyRect {
    NSBitmapImageRep *rep = [[[NSBitmapImageRep alloc]
            initWithBitmapDataPlanes:&(surface)
                          pixelsWide:system->width
                          pixelsHigh:system->height
                       bitsPerSample:8
                     samplesPerPixel:3
                            hasAlpha:NO
                            isPlanar:NO
                      colorSpaceName:NSCalibratedRGBColorSpace
                         bytesPerRow:system->width * system->bpp
                        bitsPerPixel:32] autorelease];
    NSImage *nsimage = [[[NSImage alloc] init] autorelease];
    [nsimage addRepresentation:rep];
    [nsimage drawInRect:dirtyRect];
}

- (void)keyDown:(NSEvent *)event {
    handle_events(system, 1, [event keyCode]);
}

- (void)keyUp:(NSEvent *)event {
    handle_events(system, 0, [event keyCode]);
}

- (void)mouseDown:(NSEvent *)event {
    /* Update motion_x/y at click time so hitbox test uses current position.
     * Cocoa y-origin is bottom-left; flip to match top-left framebuffer origin. */
    NSPoint point = [self convertPoint:[event locationInWindow] fromView:nil];
    system->motion_x = (i32)point.x;
    system->motion_y = system->height - (i32)point.y;
    /* Edge = 2 (pressed this frame); ls_update_events downgrades to 1 next tick. */
    system->mouse_left = 2;
}

- (void)mouseUp:(NSEvent *)event {
    (void)event;
    system->mouse_left = 0;
}

- (void)rightMouseDown:(NSEvent *)event {
    (void)event;
    system->mouse_right = 2;
}

- (void)rightMouseUp:(NSEvent *)event {
    (void)event;
    system->mouse_right = 0;
}

- (void)otherMouseDown:(NSEvent *)event {
    (void)event;
    system->mouse_middle = 2;
}

- (void)otherMouseUp:(NSEvent *)event {
    (void)event;
    system->mouse_middle = 0;
}

- (void)scrollWheel:(NSEvent *)event {
    // float offset = (float)[event scrollingDeltaY];
    // if ([event hasPreciseScrollingDeltas]) {
    //     offset *= 0.1f;
    // }
}

- (void)updateTrackingAreas {
    [self initTrackingArea];
}

- (void) initTrackingArea {
    NSTrackingAreaOptions options = (NSTrackingActiveAlways | NSTrackingInVisibleRect |
                                NSTrackingMouseEnteredAndExited | NSTrackingMouseMoved);

    NSTrackingArea *area = [[NSTrackingArea alloc] initWithRect:[self bounds]
                                                        options:options
                                                            owner:self
                                                        userInfo:nil];

    [self addTrackingArea:area];
}

- (void)mouseMoved:(NSEvent *)event {
    [super mouseMoved: event];
    NSPoint point = [self convertPoint:[event locationInWindow] fromView:nil];
    /* Cocoa y-origin is bottom-left; framebuffer y-origin is top-left.
     * Flip y so menu hitbox tests (top-down) work correctly. */
    i32 flipped_y = system->height - (i32)point.y;
    handle_mouse_events(system, 1, (i32)point.x, flipped_y);
}

@end
    

static void create_window(system_t *system) {
    NSRect rect;
    NSUInteger mask;
    WindowDelegate *delegate;
    ContentView *view;
    
    delegate = [[WindowDelegate alloc] initWithSystem:system];
    view = [[[ContentView alloc] initWithSystem:system] autorelease];

    rect = NSMakeRect(0, 0, system->width, system->height);
    mask = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable;
    window = [[NSWindow alloc] initWithContentRect:rect
                                         styleMask:mask
                                           backing:NSBackingStoreBuffered
                                             defer:NO];
    [window setDelegate:delegate];
    [window setContentView:view];
    [window makeFirstResponder:view];
    [window makeKeyAndOrderFront:nil];

    [window setTitle:[NSString stringWithUTF8String:system->title]];
    [window setColorSpace:[NSColorSpace genericRGBColorSpace]];
    [window center];
}


i32 ls_init_instance(system_t *system, i32 width, i32 height, i32 bpp) {
    /* Line-buffer stdout so print() output reaches the shell when the app
     * runs backgrounded (Cocoa apps otherwise leave stdout fully buffered). */
    setvbuf(stdout, NULL, _IOLBF, 0);
    setvbuf(stderr, NULL, _IOLBF, 0);

    system->quit = false;
    system->width = width;
    system->height = height;
    system->bpp = bpp;

    surface = (u8 *)malloc(width * height * bpp);

    pool = [[NSAutoreleasePool alloc] init];
    [NSApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    
    NSMenu *menu, *app_menu;
    NSMenuItem *app_menu_item, *quit_menu_item;
    NSString *app_name, *quit_title;

    menu = [[[NSMenu alloc] init] autorelease];
    [NSApp setMainMenu:menu];

    app_menu_item = [[[NSMenuItem alloc] init] autorelease];
    [menu addItem:app_menu_item];

    app_menu = [[[NSMenu alloc] init] autorelease];
    [app_menu_item setSubmenu:app_menu];

    app_name = [[NSProcessInfo processInfo] processName];
    quit_title = [@"Quit " stringByAppendingString:app_name];
    quit_menu_item = [[[NSMenuItem alloc] initWithTitle:quit_title action:@selector(terminate:) keyEquivalent:@"q"] autorelease];

    [app_menu addItem:quit_menu_item];

    [NSApp finishLaunching];

    create_window(system);

    return TRUE;
}

void ls_update_events(system_t *system) {
    /* Downgrade pressed-edge (2) → held (1) so consumers can distinguish
     * a click frame from subsequent held frames. */
    if (system->mouse_left   == 2) system->mouse_left   = 1;
    if (system->mouse_right  == 2) system->mouse_right  = 1;
    if (system->mouse_middle == 2) system->mouse_middle = 1;

    NSEvent* ev;
    do {
        ev = [NSApp nextEventMatchingMask: NSEventMaskAny
                                untilDate: nil
                                    inMode: NSDefaultRunLoopMode
                                    dequeue: YES];
        if (ev) {
            [NSApp sendEvent: ev];
        }
    } while (ev);
    [pool drain];
    pool = [[NSAutoreleasePool alloc] init];
}

inline u32 ls_tick() {
    return mach_absolute_time() / 1000000;
}

inline void ls_delay(u32 delay) {
    [NSThread sleepForTimeInterval:delay / 1000.0];
}

void ls_set_title(system_t *system, c8 *title) {
    [window setTitle:[NSString stringWithUTF8String:title]];
}

void ls_blit(system_t *system, u32 *front_buffer) {
    memcpy(surface, (u8*)front_buffer, system->width * system->height * system->bpp);
}

void ls_flip(system_t *system) {
    [[window contentView] setNeedsDisplay:YES];
}

void ls_release(system_t *system) {
    [window orderOut:nil];

    [[window delegate] release];
    [window close];

    [pool drain];
    pool = [[NSAutoreleasePool alloc] init];

    free(surface);
    free(window);
}

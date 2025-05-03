#include <Cocoa/Cocoa.h>
#include <mach-o/dyld.h>
#include <mach/mach_time.h>
#include <unistd.h>

#include "../core/system.h"
#include "../core/common.h"
// #include "platform.h"


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
}

- (void)mouseUp:(NSEvent *)event {
}

- (void)rightMouseDown:(NSEvent *)event {
}

- (void)rightMouseUp:(NSEvent *)event {
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
    // printf("x: %f, y: %f\n", point.x, point.y);
    handle_mouse_events(system, 1, point.x, point.y);
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


void system_init(system_t *system, c8 *title, i32 width, i32 height, i32 bpp) {
    print("Initializing system...\n");
    system->quit = false;
    system->title = title;
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
}

void system_events(system_t *system) {
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

inline u32 system_tick() {
    return mach_absolute_time() / 1000000;
}

inline void system_delay(u32 delay) {
    [NSThread sleepForTimeInterval:delay / 1000.0];
}

void system_set_title(system_t *system, c8 *title) {
    [window setTitle:[NSString stringWithUTF8String:title]];
}

void system_blit(system_t *system, u32 *front_buffer) {
    memcpy(surface, (u8*)front_buffer, system->width * system->height * system->bpp);
}

void system_flip(system_t *system) {
    [[window contentView] setNeedsDisplay:YES];
}

void system_release(system_t *system) {
    [window orderOut:nil];

    [[window delegate] release];
    [window close];

    [pool drain];
    pool = [[NSAutoreleasePool alloc] init];

    free(surface);
    free(window);
}

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include "../system.h"
#include "../common.h"


static HWND window;
static HDC hdc;
static HDC memory_dc;

static u8 *surface = NULL;


static void handle_events(system_t *system, i32 type, i32 key) {
    switch(key) {
        // w
        case 'W':
            system->actions[119] = type;
            break;
        // s
        case 'S':
            system->actions[115] = type;
            break;
        // a
        case 'A':
            system->actions[97] = type;
            break;
        // d
        case 'D':
            system->actions[100] = type;
            break;
        // q
        case 'Q':
            system->actions[113] = type;
            break;
        // e
        case 'E':
            system->actions[101] = type;
            break;
        // r
        case 'R':
            system->actions[114] = type;
            break;
        // t
        case 'T':
            system->actions[116] = type;
            break;
        // l
        case 'L':
            system->actions[108] = type;
            break;
        default:
            break;
    }
}

static void create_window(system_t *system) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = DefWindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "";
    RegisterClass(&wc);

    window = CreateWindow(
        wc.lpszClassName,
        "",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        system->width,
        system->height,
        NULL,
        NULL,
        wc.hInstance,
        NULL
    );

    hdc = GetDC(window);
    memory_dc = CreateCompatibleDC(hdc);

    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = system->width;
    bmi.bmiHeader.biHeight = -system->height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = system->bpp * 8;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = 0;
    bmi.bmiHeader.biXPelsPerMeter = 0;
    bmi.bmiHeader.biYPelsPerMeter = 0;
    bmi.bmiHeader.biClrUsed = 0;
    bmi.bmiHeader.biClrImportant = 0;

    HBITMAP dib = CreateDIBSection(memory_dc, &bmi, DIB_RGB_COLORS, (void**)&surface, NULL, 0);
    SelectObject(memory_dc, dib);

    ShowWindow(window, SW_SHOW);
}


i32 ls_init_instance(system_t *system, i32 width, i32 height, i32 bpp) {
    system->quit = false;
    system->width = width;
    system->height = height;
    system->bpp = bpp;

    surface = (u8 *)malloc(width * height * bpp);

    create_window(system);

    return TRUE;
}

void ls_update_events(system_t *system) {
    MSG msg;
    while (PeekMessage(&msg, window, 0, 0, PM_REMOVE)) {
        switch (msg.message) {
            case WM_QUIT:
            case WM_CLOSE:
                system->quit = true;
                break;
            case WM_KEYDOWN:
                handle_events(system, true, msg.wParam);
                break;
            case WM_KEYUP:
                handle_events(system, false, msg.wParam);
                break;
            default:
                break;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

inline u32 ls_tick() {
    return GetTickCount();
}

inline void ls_delay(u32 delay) {
    Sleep(delay);
}

void ls_set_title(system_t *system, c8 *title) {
    SetWindowText(window, title);
}

void ls_blit(system_t *system, u32 *front_buffer) {
    memcpy(surface, (u8*)front_buffer, system->width * system->height * system->bpp);
}

void ls_flip(system_t *system) {
    HDC window_dc = GetDC(window);
    BitBlt(window_dc, 0, 0, system->width, system->height, memory_dc, 0, 0, SRCCOPY);
    ReleaseDC(window, window_dc);
}

void ls_release(system_t *system) {
    free(surface);
}

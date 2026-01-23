/*===============================================================================
  @File:   win_main.cpp
  @Brief:  Windows platform-specific entry point and main window procedure.
  @Author: Tejas
  @Date:   28-08-2025
  @Notice: Released under the MIT License. See LICENSE file for details.
  ===============================================================================*/

#define NOMINMAX
#include <windows.h>
#include <shellapi.h>

#include "base.h"
#include "editor.h"
#include "editor_view.h"
#include "input.h"

/******* Structure Definations *******/
// TODO(Tejas): Make this platform independent
typedef struct {
    HFONT    font;
    COLORREF font_color;
    HBRUSH   background;
    HPEN     cursor_color;

    EditorView ev;
} EditorOptions;

typedef struct {
    HDC     dc;
    HBITMAP bm;
    int     w;
    int     h;
} BackBuffer;
/*************************************/

/******* Global Variables *******/
global BackBuffer    G_back_buffer;
global EditorOptions G_editor_opt;
global Editor       *G_editor;
/********************************/

internal void RenderGapBuffer(HDC hdc, GapBuffer *gb, int font_w, int font_h) {

    // NOTE(Tejas): font_w will be used for line wrapping in the future.
    (void)font_w;

    SetTextColor(hdc, G_editor_opt.font_color);
    SetBkMode(hdc, TRANSPARENT);

    int x = 0;
    int y = 0;

    for (int i = G_editor_opt.ev.start_line; i <= G_editor_opt.ev.end_line && i < gb->lines.count; i++) {

        Line line = gb->lines.items[i];

        // FIXME(Tejas): breaks if number of chars in a line is bigger than 4096(bytes).
        char temp[KB(4)];
        int len = 0;

        for (int index = line.start; index < line.end; index++) {

            if (index >= gb->gap_start && index <= gb->gap_end)
                continue;

            temp[len++] = gb->data.chars[index];
        }

        TextOutA(hdc, x, y, temp, len);
        y += font_h;
    }
}

internal void RenderCursor(HDC hdc, GapBuffer *gb, int font_w, int font_h) {

    int row = ed_GetCursorRow(gb) - G_editor_opt.ev.start_line;

    int y =  row * font_h;
    int x = ed_GetCursorCol(gb) * font_w;
    Rectangle(hdc, x, y, x + 3, y + font_h);
}

internal void ResizeBackBuffer(HWND hwnd, int w, int h) {

    if (G_back_buffer.dc && (w == G_back_buffer.w && h == G_back_buffer.h))
        return;

    if (G_back_buffer.bm) {
        DeleteObject(G_back_buffer.bm);
        G_back_buffer.bm = NULL;
    }

    HDC dc = GetDC(hwnd);

    if (!G_back_buffer.dc) {
        G_back_buffer.dc = CreateCompatibleDC(dc);
    }

    G_back_buffer.bm = CreateCompatibleBitmap(dc, w, h);
    SelectObject(G_back_buffer.dc, G_back_buffer.bm);

    ReleaseDC(hwnd, dc);

    G_back_buffer.w = w;
    G_back_buffer.h = h;
}

internal LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    LRESULT result = 0;

    switch (msg) {

    case WM_SIZE: {
        RECT client_rect = { };
        GetClientRect(hwnd, &client_rect);

        int w = client_rect.right  - client_rect.left;
        int h = client_rect.bottom - client_rect.top;

        ResizeBackBuffer(hwnd, w, h);

    } break;

    case WM_CREATE: {

        G_editor_opt.background = CreateSolidBrush(RGB(18, 18, 18));
        G_editor_opt.font_color = RGB(208, 208, 208);
        G_editor_opt.font = CreateFont(-24, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                       ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                       DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Consolas");

        G_editor_opt.cursor_color = CreatePen(PS_SOLID, 1, RGB(255, 255, 0));

        G_editor_opt.ev.start_line = 0;

    } break;

    case WM_CHAR: {

        bool ctrl_down = (GetKeyState(VK_CONTROL) & 0x8000) != 0;

        if (!ctrl_down) {
            ed_InsertCharAtCursor(&(G_editor->gb), (char)wParam);
            if (ed_GetCursorRow(&(G_editor->gb)) == G_editor_opt.ev.end_line)
                ev_MoveViewOneLineDown(&(G_editor_opt.ev), &(G_editor->gb));
            InvalidateRect(hwnd, NULL, TRUE);
        }

    } break;

    case WM_KEYDOWN: {

        KeyInput in = { };
        in.ctrl  = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
        in.shift = (GetKeyState(VK_SHIFT)   & 0x8000) != 0;
        in.alt   = (GetKeyState(VK_MENU)    & 0x8000) != 0;
        in.key   = (u32)wParam;

        in_HandleKey(in, G_editor, &G_editor_opt.ev);

        InvalidateRect(hwnd, NULL, TRUE);

    } break;

    case WM_PAINT: {

        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT rect;
        GetClientRect(hwnd, &rect);

        int width  = rect.right - rect.left;
        int height = rect.bottom - rect.top;

        (void)width;

        FillRect(G_back_buffer.dc, &rect, G_editor_opt.background);

        // Text Rendering
        HFONT old_font = (HFONT)SelectObject(G_back_buffer.dc, G_editor_opt.font);

        TEXTMETRIC tm;
        GetTextMetrics(G_back_buffer.dc, &tm);

        int font_w = tm.tmAveCharWidth;
        int font_h = tm.tmHeight;

        ev_UpdateEditorView(&(G_editor_opt.ev), &(G_editor->gb), font_h, height);
        RenderGapBuffer(G_back_buffer.dc, &(G_editor->gb), font_w, font_h);

        // Cursor Rendering
        HPEN old_pen = (HPEN)SelectObject(G_back_buffer.dc, G_editor_opt.cursor_color);

        RenderCursor(G_back_buffer.dc, &(G_editor->gb), font_w, font_h);

        SelectObject(G_back_buffer.dc, old_font);
        SelectObject(G_back_buffer.dc, old_pen);

        BitBlt(hdc, 0, 0, width, height, G_back_buffer.dc, 0, 0, SRCCOPY);

        EndPaint(hwnd, &ps);

    } break;

    case WM_DROPFILES: {

        HDROP hdrop = (HDROP) wParam;

        // NOTE(Tejas): For now we only allow one file to be dropped into the editor
        int file_count = DragQueryFile(hdrop, 0xFFFFFFFF, NULL, 0);
        if (!(file_count > 1)) {
            char file_name[KB(1)];
            DragQueryFileA(hdrop, 0, file_name, KB(1));
            ed_Init(&G_editor, file_name);
            LOG("Loading %s", file_name);
            InvalidateRect(hwnd, NULL, TRUE);
        }

    } break;

    case WM_DESTROY: {
        ed_Close(G_editor);
        DeleteObject(G_editor_opt.background);
        DeleteObject(G_editor_opt.font);
        PostQuitMessage(0);
    } break;

    default: {
        result = DefWindowProc(hwnd, msg, wParam, lParam);
    } break;

    }

    return result;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
#ifndef NDEBUG
    bool console_attached = false;
    if (!AttachConsole(ATTACH_PARENT_PROCESS)) {
        AllocConsole();
        console_attached = true;
    }
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
#endif

    (void)hPrevInstance;
    (void)pCmdLine;
    (void)nCmdShow;

    int wnd_width  = 1000;
    int wnd_height = 800;

    const char* wnd_name = "Text-Editor";

    WNDCLASSA wc = { };
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = wnd_name;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW); // NOTE(Karan): Default arrow cursor for now
                                                    // but handle WM_SETCURSOR to change it later

    RegisterClassA(&wc);

    HWND hwnd = CreateWindowExA(0, wc.lpszClassName, wnd_name,
                                WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                                (int)wnd_width, (int)wnd_height,
                                NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, SW_SHOWNORMAL);

    DragAcceptFiles(hwnd, TRUE);

    const char* file_name = "test.txt";
    if (__argc == 2) {
        file_name = __argv[1];
    }
    ed_Init(&G_editor, file_name);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

#ifndef NDEBUG
    if (console_attached) FreeConsole();
#endif
    return 0;
}

/*===============================================================================
  @File:   win_main.cpp
  @Brief:  Windows platform-specific entry point and main window procedure.
  @Author: Tejas
  @Date:   28-08-2025
  @Notice: Released under the MIT License. See LICENSE file for details.
  ===============================================================================*/

#define NOMINMAX
#include <windows.h>

#include "base.h"
#include "editor.h"

/******* Structure Definations *******/
// TODO(Tejas): Make this platform independent
struct EditorOptions {
    HFONT    font;
    COLORREF font_color;
    HBRUSH   background;
    HPEN     cursor_color;
};
/*************************************/

/******* Global Variables *******/
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

    for (int i = 0; i < gb->lines.count; i++) {

        Line line = gb->lines.items[i];

        char temp[4096];
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

    int y = ed_GetCursorRow(gb) * font_h;
    int x = ed_GetCursorCol(gb) * font_w;
    Rectangle(hdc, x, y, x + 3, y + font_h);
}

internal LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    LRESULT result = 0;

    switch (msg) {

    case WM_CREATE: {
        
        G_editor_opt.background = CreateSolidBrush(RGB(18, 18, 18));
        G_editor_opt.font_color = RGB(208, 208, 208);
        G_editor_opt.font = CreateFont(-24, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                       ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                       DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Consolas");

        G_editor_opt.cursor_color = CreatePen(PS_SOLID, 1, RGB(255, 255, 0));

    } break;

    case WM_CHAR: {

        bool ctrl_down = (GetKeyState(VK_CONTROL) & 0x8000) != 0;

        if (!ctrl_down) {
            ed_InsertCharAtCursor(&(G_editor->gb), (char)wParam);
            InvalidateRect(hwnd, NULL, TRUE);
        }

    } break;

    case WM_KEYDOWN: {
        
        bool ctrl_down = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
        
        switch (wParam) {

        case 'S': {
            if (ctrl_down) {
                saveGapBufferToFile(
                    G_editor->file_name,
                    G_editor->gb.data.chars,
                    G_editor->gb.data.capacity,
                    G_editor->gb.gap_start,
                    G_editor->gb.gap_end
                );
            }
        } break;

        case 'A': {
            if (ctrl_down) {
                ed_LogGapBuffer(&(G_editor->gb));
            }
        } break;

        case 'F': {
            if (ctrl_down) {
                ed_LogValidChars(&(G_editor->gb));
            }
        } break;

        case VK_RIGHT: {
            ed_MoveCursorRight(&(G_editor->gb));
            InvalidateRect(hwnd, NULL, TRUE);
        } break;

        case VK_LEFT: {
            ed_MoveCursorLeft(&(G_editor->gb));
            InvalidateRect(hwnd, NULL, TRUE);
        } break;

        case VK_UP: {
            ed_MoveCursorUp(&(G_editor->gb));
            InvalidateRect(hwnd, NULL, TRUE);
        } break;

        case VK_DOWN: {
            ed_MoveCursorDown(&(G_editor->gb));
            InvalidateRect(hwnd, NULL, TRUE);
        } break;

        }

    } break;

    case WM_PAINT: {

        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT rect;
        GetClientRect(hwnd, &rect);
        FillRect(hdc, &rect, G_editor_opt.background);

        // Text Rendering
        HFONT old_font = (HFONT)SelectObject(hdc, G_editor_opt.font);

        TEXTMETRIC tm;
        GetTextMetrics(hdc, &tm);

        int font_w = tm.tmAveCharWidth;
        int font_h = tm.tmHeight;

        RenderGapBuffer(hdc, &(G_editor->gb), font_w, font_h);

        SelectObject(hdc, old_font);

        // Cursor Rendering
        HPEN old_pen = (HPEN)SelectObject(hdc, G_editor_opt.cursor_color);

        RenderCursor(hdc, &(G_editor->gb), font_w, font_h);

        SelectObject(hdc, old_pen);

        EndPaint(hwnd, &ps);

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

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {

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

    int wnd_width  = 1000;
    int wnd_height = 800;

    const char* wnd_name = "Text-Editor";

    WNDCLASSA wc = { };
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = wnd_name;

    RegisterClassA(&wc);

    HWND hwnd = CreateWindowExA(0, wc.lpszClassName, wnd_name,
                                WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                                (int)wnd_width, (int)wnd_height,
                                NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, SW_SHOWNORMAL);

    const char* file_name = "test.txt";
    if (__argc == 2) {
        file_name = __argv[1];
        LOG("%s", file_name);
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

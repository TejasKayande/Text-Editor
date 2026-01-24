
#include "editor_view.h"

void ev_UpdateEditorView(EditorView *ev, GapBuffer *gb, int font_w, int font_h, int wnd_width, int wnd_height) {

    if (ev->start_line >= gb->lines.count)
        ev->start_line = gb->lines.count - 1;

    if (ev->start_line <= 0)
        ev->start_line = 0;

    int lines_in_viewport = wnd_height / font_h;
    ev->end_line = ev->start_line + lines_in_viewport;


    // TODO(Karan): Handle horizontal scrolling later
    ev->start_col = 0;


    int cols_in_viewport  = wnd_width / font_w;
    ev->end_col   = ev->start_col + cols_in_viewport + 1; // NOTE(Karan): +1 to accomodate partial character at the end

    
    // if (ev->end_line >= gb->lines.count) ev->end_line = gb->lines.count - 1;

    int cur_row = ed_GetCursorRow(gb);
    while (cur_row < ev->start_line)   ev_MoveViewOneLineUp(ev, gb);
    while (cur_row > ev->end_line - 1) ev_MoveViewOneLineDown(ev, gb);
}

void ev_MoveViewOneLineDown(EditorView *ev, GapBuffer *gb) {

    if (ev->start_line >= gb->lines.count) return;

    ev->start_line++;
    ev->end_line++;
}

void ev_MoveViewOneLineUp(EditorView *ev, GapBuffer *gb) {

    (void)(gb);

    if (ev->start_line <= 0) return;

    ev->start_line--;
    ev->end_line--;
}

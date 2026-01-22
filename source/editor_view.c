
#include "editor_view.h"

void ev_UpdateEditorView(EditorView *ev, GapBuffer *gb, int font_h, int wnd_height) {

    if (ev->start_line >= gb->lines.count)
        ev->start_line = gb->lines.count - 1;

    if (ev->start_line <= 0)
        ev->start_line = 0;

    int lines_in_viewport = wnd_height / font_h;
    ev->end_line = ev->start_line + lines_in_viewport;

    // if (ev->end_line >= gb->lines.count) ev->end_line = gb->lines.count - 1;

    // TODO(Tejas): Make it so that the cursor will always be inside the View Port!
    int cur_row = ed_GetCursorRow(gb);
    if (cur_row < ev->start_line) ed_SetCursorRow(gb, ev->start_line);
    if (cur_row > ev->end_line)   ed_SetCursorRow(gb, ev->end_line - 1);
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

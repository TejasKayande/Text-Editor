/*===============================================================================
  @File:   input.h
  @Brief: 
  @Author: Tejas
  @Date:   20-01-2026
  @Notice: Released under the MIT License. See LICENSE file for details.
  ===============================================================================*/

#include "input.h"

typedef struct {
    bool ctrl;
    bool shift;
    bool alt;
    u32 key;
    EditorCommand cmd;
} Keybinds;

internal Keybinds G_keybinds[] = {

    { true, false, false, 'S', CMD_SAVE },

    { true, false, false, 'P', CMD_MOVE_CUR_UP    },
    { true, false, false, 'N', CMD_MOVE_CUR_DOWN  },
    { true, false, false, 'B', CMD_MOVE_CUR_LEFT  },
    { true, false, false, 'F', CMD_MOVE_CUR_RIGHT },

    { true, false, false, 'Y', CMD_MOVE_VIEW_UP   },
    { true, false, false, 'E', CMD_MOVE_VIEW_DOWN },

    { true, false, false, 'D', CMD_MOVE_DOWN_PARA },
    { true, false, false, 'U', CMD_MOVE_UP_PARA   },

    { true, true, false, 'A', CMD_LOG_GAP_BUFFER },
};

internal EditorCommand GetEditorCommand(KeyInput in) {

    int count = ARRAY_COUNT(G_keybinds);
    for (int i = 0; i < count; i++) {
        Keybinds *sc = &G_keybinds[i];
        if (sc->ctrl  == in.ctrl  &&
            sc->shift == in.shift &&
            sc->alt   == in.alt   &&
            sc->key   == in.key) {
            return sc->cmd;
        }
    }

    return CMD_NONE;
}

internal void ExecuteCommand(EditorCommand cmd, Editor *ed, EditorView *ev) {

    GapBuffer *gb = &ed->gb;

    switch (cmd) {

    case CMD_SAVE: {
        SaveGapBufferToFile(
            ed->file_name,
            gb->data.chars,
            gb->data.capacity,
            gb->gap_start,
            gb->gap_end
        );
    } break;

    case CMD_MOVE_CUR_LEFT: {
        ed_MoveCursorLeft(gb);
    } break;

    case CMD_MOVE_CUR_RIGHT: {
        ed_MoveCursorRight(gb);
    } break;

    case CMD_MOVE_CUR_UP: {
        if (ed_GetCursorRow(gb) == ev->start_line)
            ev_MoveViewOneLineUp(ev, gb);
        ed_MoveCursorUp(gb);
    } break;

    case CMD_MOVE_CUR_DOWN: {
        if (ed_GetCursorRow(gb) == ev->end_line)
            ev_MoveViewOneLineDown(ev, gb);
        ed_MoveCursorDown(gb);
    } break;

    case CMD_MOVE_VIEW_UP: {
        ev_MoveViewOneLineUp(ev, gb);
    } break;

    case CMD_MOVE_VIEW_DOWN: {
        ev_MoveViewOneLineDown(ev, gb);
    } break;

    case CMD_LOG_GAP_BUFFER: {
        ed_LogGapBuffer(gb);
    } break;

    case CMD_MOVE_UP_PARA: {
        ed_MoveCursorParaUp(gb);
    } break;

    case CMD_MOVE_DOWN_PARA: {
        ed_MoveCursorParaDown(gb);
    } break;

    default: break;

    }
}

void in_HandleKey(KeyInput in, Editor *ed, EditorView *ev) {

    EditorCommand cmd = GetEditorCommand(in);

    if (cmd != CMD_NONE) {
        ExecuteCommand(cmd, ed, ev);
    }
}

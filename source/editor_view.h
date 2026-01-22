/*===============================================================================
  @File:   editor_view.h
  @Brief:  
  @Author: Tejas
  @Date:   23-09-2025
  @Notice: Released under the MIT License. See LICENSE file for details.
  ===============================================================================*/

#ifndef EDITOR_VIEW_H
#define EDITOR_VIEW_H

#include "base.h"
#include "editor.h"

// NOTE(Tejas): All the visual stuff goes in here

// TODO(Tejas): I maybe would want to create an enum for font
//              but maybe not... hmmm

typedef struct EditorView {
    int start_line;
    int end_line;
    int start_col;
    int end_col;
} EditorView;

// TODO(Tejas): Does the column wrap also go in here??
void ev_UpdateEditorView(EditorView *ev, GapBuffer *gb, int font_w, int font_h, int wnd_width, int wnd_height);

void ev_MoveViewOneLineDown(EditorView *ev, GapBuffer *gb);
void ev_MoveViewOneLineUp(EditorView *ev, GapBuffer *gb);

#endif // EDITOR_VIEW_H

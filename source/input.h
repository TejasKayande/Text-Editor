/*===============================================================================
  @File:   input.h
  @Brief: 
  @Author: Tejas
  @Date:   20-01-2026
  @Notice: Released under the MIT License. See LICENSE file for details.
  ===============================================================================*/

#ifndef INPUT_H
#define INPUT_H

#include "base.h"
#include "editor.h"
#include "editor_view.h"

typedef struct {

    bool ctrl;
    bool shift;
    bool alt;

    // TODO(Tejas): This will for now use ASCII codes, but maybe change these to custom?
    u32 key;

} KeyInput;

typedef enum {

    CMD_NONE = 0,
    CMD_SAVE,

    CMD_MOVE_CUR_UP,
    CMD_MOVE_CUR_DOWN,
    CMD_MOVE_CUR_LEFT,
    CMD_MOVE_CUR_RIGHT,

    CMD_MOVE_VIEW_UP,
    CMD_MOVE_VIEW_DOWN,

    // NOTE(Tejas): Debug Commands
    CMD_LOG_GAP_BUFFER,

} EditorCommand;

void in_HandleKey(KeyInput in, Editor *ed, EditorView *ev);

#endif INPUT_H

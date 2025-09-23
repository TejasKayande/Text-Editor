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

// NOTE(Tejas): All the visual stuff goes in here

// TODO(Tejas): I maybe would want to create an enum for font
//              but maybe not... hmmm

struct EditorView {
    int start_line;
    int end_line;
};

#endif // EDITOR_VIEW_H

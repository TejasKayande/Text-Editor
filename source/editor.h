/*===============================================================================
  @File:   editor.h
  @Brief:
  @Author: Tejas
  @Date:   28-08-2025
  @Notice: Released under the MIT License. See LICENSE file for details.
  ===============================================================================*/

#ifndef EDITOR_H
#define EDITOR_H

#include "platform.h"

#define GET_GAP_SIZE(gb)    (((gb)->gap_end) - ((gb)->gap_start) + 1)
#define GET_TEXT_LENGTH(gb) ((gb)->data.capacity - GET_GAP_SIZE(gb))

#define INITIAL_GAP_SIZE  KB(1)
#define INITIAL_MAX_LINES KB(1)

typedef struct Line {
    int start;
    int end;
} Line;

typedef struct Lines {
    Line *items;
    int count;
    int capacity;
} Lines;

typedef struct Data {
    char *chars;
    int capacity;
} Data;

// NOTE(Tejas): gap_start and gap_end are inclusive.
typedef struct GapBuffer {
    Data  data;
    Lines lines;
    int   gap_start;
    int   gap_end;
    int   cur_pos;
} GapBuffer;

// NOTE(Tejas): Editor Theme and stuff can go in here
typedef struct Editor {
    GapBuffer gb;
    const char* file_name;
} Editor;

void ed_Init(Editor **ed, const char *file_name);
void ed_Close(Editor *ed);

void ed_RecalculateLines(GapBuffer *gb);

void ed_InsertCharAtCursor(GapBuffer *gb, char ch);

int ed_GetCursorRow(GapBuffer *gb);
int ed_GetCursorCol(GapBuffer *gb);

void ed_SetCursorRow(GapBuffer *gb, int row);

void ed_MoveCursorRight(GapBuffer *gb);
void ed_MoveCursorLeft(GapBuffer *gb);
void ed_MoveCursorUp(GapBuffer *gb);
void ed_MoveCursorDown(GapBuffer *gb);

void ed_DeleteCharToRight(GapBuffer *gb);

void ed_MoveCursorParaUp(GapBuffer *gb);
void ed_MoveCursorParaDown(GapBuffer *gb);

// NOTE(Tejas): For Debugging purposes
void ed_LogGapBuffer(GapBuffer *gb);
void ed_LogValidChars(GapBuffer *gb);

#endif // EDITOR_H

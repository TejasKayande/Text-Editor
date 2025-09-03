/*===============================================================================
  @File:   editor.cpp
  @Brief:  
  @Author: Tejas
  @Date:   30-08-2025
  @Notice: Released under the MIT License. See LICENSE file for details.
  ===============================================================================*/

#include "editor.h"

internal void AdjustLinesBufferCapacity(Lines *lines) {

    lines->capacity = lines->capacity * 2;
    lines->items = (Line*)ReallocateMem(lines->items, lines->capacity * sizeof(Line));
}

internal void MoveGapToCursor(GapBuffer* gb) {

    // FIXME(Tejas): This is going to shit its pants if shift is > than gap_size
    //               This whole functions needs to be thrown away...

    if (gb->cur_pos >= gb->gap_start && gb->cur_pos <= gb->gap_end) {
        gb->cur_pos = gb->gap_start;
    }

    if (gb->gap_start == gb->cur_pos) {

        return;

    } else if (gb->cur_pos < gb->gap_start) {

        int shift = gb->gap_start - gb->cur_pos;

        for (int i = 0; i < shift; i++) {
            int src  = gb->cur_pos + i;
            int dest = (gb->gap_end - shift + 1) + i;
            gb->data.chars[dest] = gb->data.chars[src];
        }

        gb->gap_start = gb->cur_pos;
        gb->gap_end  -= shift;

        return;

    } else {

        int shift = (gb->cur_pos - gb->gap_end) - 1; // dont want to include cursor

        for (int i = 0; i < shift; i++) {
            int src  = gb->gap_end + 1 + i;
            int dest = gb->gap_start + i;
            gb->data.chars[dest] = gb->data.chars[src];
        }

        gb->gap_start += shift;
        gb->gap_end   += shift;

        gb->cur_pos = gb->gap_start;

        return;
    }
}

internal int GetCursorPosInValidChars(GapBuffer *gb) {

    int pos = 0;
    for (int index = 0; index < gb->data.capacity; index++) {

        if (index == gb->cur_pos) return pos;

        if (index >= gb->gap_start && index <= gb->gap_end) {
            continue;
        }

        pos++;
    }

    return pos;
}

void FillGapWithChar(GapBuffer *gb, char ch) {
    for (int index = gb->gap_start; index <= gb->gap_end; index++)
        gb->data.chars[index] = ch;
}

void ed_Init(Editor **ed, const char* file_name) {

    *ed = (Editor*)AllocateMem(sizeof(Editor));
    GapBuffer *gb = &((*ed)->gb);

    int fs, bs;
    int gap_size = 1024;
    gb->data.chars    = LoadFileIntoGapBuffer(file_name, &fs, &bs, gap_size);
    gb->data.capacity = bs;

    // NOTE(Tejas): The Buffer that is returned by loadFileIntoBuffer, will contain
    //              the contents of the file as well as a gap buffer after the file
    //              content.

    // NOTE(Tejas): because gap_start is an index into the buffer. it will be put
    //              initially at the index of File Size (as the last char of the
    //              file will be at the index of (fs - 1)).

    // NOTE(Tejas): gap_end will be initially set to the last index of the buffer as
    //              the capacity of the buffer is initially the size of the file +
    //              the size of the Gap Buffer.

    gb->gap_start = fs;
    gb->gap_end   = bs - 1;

    gb->cur_pos = gb->gap_start;

    (*ed)->file_name = file_name;

    gb->lines.capacity = 1024;
    gb->lines.items    = (Line*)AllocateMem(sizeof(Line) * gb->lines.capacity);
    gb->lines.count    = 0;

    FillGapWithChar(gb, '-');
    ed_RecalculateLines(gb);
}

void ed_Close(Editor *ed) {

    FreeMem(ed->gb.lines.items);
    FreeMem(ed->gb.data.chars);
    FreeMem(ed);
}

void ed_RecalculateLines(GapBuffer *gb) {

    // NOTE(Tejas): Could this be just an internal function for this TU? Maybe...

    gb->lines.count = 0;;
    int start = 0, index = 0;

    while (index < gb->data.capacity) {

        if (index >= gb->gap_start && index <= gb->gap_end) {
            index = gb->gap_end + 1;
            continue;
        }

        char ch = gb->data.chars[index];

        if (ch == '\n') {
            Line line = { };
            line.start = start;
            line.end   = index;

            if (gb->lines.count >= gb->lines.capacity) {
                AdjustLinesBufferCapacity(&(gb->lines));
            }

            gb->lines.items[gb->lines.count++] = line;
            start = line.end + 1;
        }

        index++;
    }

    if (start < index) {

        Line line = { };
        line.start = start;
        line.end   = index;

        if (gb->lines.count >= gb->lines.capacity) {
            AdjustLinesBufferCapacity(&(gb->lines));
        }

        gb->lines.items[gb->lines.count++] = line;
    }
}

void ed_InsertCharAtCursor(GapBuffer *gb, char ch) {

    // find out at what index in the buffer is the cursor in
    // and check if its equal to gap_start. If not then move
    // gap buffer to that index.
    MoveGapToCursor(gb);

    // TODO(Tejas): Add dynamic gap expansion
    if (gb->gap_start >= gb->gap_end) return;

    if (ch == '\b') {
        if (gb->gap_start > 0) {
            gb->gap_start--;
            gb->cur_pos = gb->gap_start;
        }
    }

    else {

        if (ch == '\r') ch = '\n';

        gb->data.chars[gb->gap_start++] = ch;
        gb->cur_pos = gb->gap_start;
    }

    FillGapWithChar(gb, '-');
    ed_RecalculateLines(gb);
}

int ed_GetCursorRow(GapBuffer *gb) {

    for (int i = 0; i < gb->lines.count; i++) {
        Line l = gb->lines.items[i];
        if (gb->cur_pos >= l.start && gb->cur_pos <= l.end) return i;
    }

    return gb->lines.count;
}

int ed_GetCursorCol(GapBuffer *gb) {

    int row = ed_GetCursorRow(gb);
    if (row >= gb->lines.count) return 0;

    Line l = gb->lines.items[row];
    int col = 0;

    for (int index = l.start; index < l.end; index++) {

        if (index == gb->cur_pos) return col;

        if (index >= gb->gap_start && index <= gb->gap_end)
            continue;

        col++;
    }

    return col;
}

void ed_MoveCursorRight(GapBuffer *gb) {


    int before = gb->cur_pos;

    // NOTE(Tejas): we move the cursor to gap_end + 2 because if the cursor is
    //              equal to gap_start its visually already at the gap_end + 1 char.
    if (gb->cur_pos == gb->gap_start) {
        gb->cur_pos = gb->gap_end + 2;
    } else {
        gb->cur_pos++;
    }

    if (gb->cur_pos > gb->data.capacity) gb->cur_pos = before;
}

void ed_MoveCursorLeft(GapBuffer *gb) {
    
    gb->cur_pos--;

    if (gb->cur_pos > gb->gap_start && gb->cur_pos <= gb->gap_end) {
        gb->cur_pos = (gb->gap_start > 0) ? (gb->gap_start - 1) : 0;
    }

    if (gb->cur_pos < 0) gb->cur_pos = 0;
}

void ed_MoveCursorUp(GapBuffer *gb) {

    // TODO(Tejas): column number presist.
    
    int row = ed_GetCursorRow(gb);
    int col = ed_GetCursorCol(gb);

    if (row <= 0) return;

    int new_row = row - 1;
    Line l = gb->lines.items[new_row];

    int index = l.start;
    for (int i = 0; i < col; i++) {
        if (index > gb->gap_start && index <= gb->gap_end) {
            index = gb->gap_end + 1;
            i--;
            continue;
        }

        if (index >= l.end) break;

        index++;
    }

    gb->cur_pos = index;
}

void ed_MoveCursorDown(GapBuffer *gb) {

    // TODO(Tejas): column number presist.
    
    int row = ed_GetCursorRow(gb);
    int col = ed_GetCursorCol(gb);

    if (row >= (gb->lines.count - 1)) return;

    int new_row = row + 1;
    Line l = gb->lines.items[new_row];

    int index = l.start;
    for (int i = 0; i < col; i++) {
        if (index > gb->gap_start && index <= gb->gap_end) {
            index = gb->gap_end + 1;
            i--;
            continue;
        }

        if (index >= l.end) break;

        index++;
    }

    gb->cur_pos = index;
}

void ed_LogGapBuffer(GapBuffer *gb) {

    LOG("----------GapBuffer----------\n");
    LOG("Buffer Capacity: %d\n", gb->data.capacity);
    LOG("Gap Size: %d\n", GET_GAP_SIZE(gb));
    LOG("Valid Chars: %d\n", GET_TEXT_LENGTH(gb));
    LOG("\n");
    LOG("Gap Start: %d\n", gb->gap_start);
    LOG("Gap End: %d\n", gb->gap_end);
    LOG("Cursor Pos: %d\n", gb->cur_pos);
    LOG("\n");
    LOG("Total Lines: %d\n", gb->lines.count);
    LOG("Cursor Row: %d\n", ed_GetCursorRow(gb));
    LOG("Cursor Col: %d\n", ed_GetCursorCol(gb));
    LOG("Cursor in Valid Chars: %d\n", GetCursorPosInValidChars(gb));
    char ch = gb->data.chars[gb->cur_pos];
    if (ch == '\n') LOG("Char At Cursor: \\n\n");
    else LOG("Char At Cursor: %c\n", ch);
    LOG("-----------------------------\n");
}

void ed_LogValidChars(GapBuffer *gb) {

    LOG("----------------Buffer Content----------------\n");
    for (int index = 0; index < gb->data.capacity; index++) {

        if (index >= gb->gap_start && index <= gb->gap_end) {
            index = gb->gap_end;
            continue;
        }

        char ch = gb->data.chars[index];

        if (ch == '\n') printf("\\n\n");
        else printf("%c", ch);
    }
    LOG("----------------------------------------------\n");
}

/*===============================================================================
  @File:   win_platform.cpp
  @Brief:  
  @Author: Tejas
  @Date:   30-08-2025
  @Notice: Released under the MIT License. See LICENSE file for details.
  ===============================================================================*/

#include "platform.h"
#include "editor.h"

void* AllocateMem(int size) {
    
    void* ptr = malloc(size);

    // TODO(Tejas): add a logging system.
    if (!ptr) return NULL; 
    return ptr;
}

void* ReallocateMem(void* ptr, int new_size) {
    
    void* new_ptr = realloc(ptr, new_size);

    // TODO(Tejas): add a logging system.
    if (!new_ptr) return NULL;
    return new_ptr;
}

void FreeMem(void* ptr) {

    if (ptr) free(ptr);
}

char* LoadFileIntoGapBuffer(const char* file_path, int *file_size, int *buffer_size, int gap_size) {
    
    HANDLE file_handle = CreateFileA(file_path, GENERIC_READ, FILE_SHARE_READ,
                                     NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                                     NULL);

    DWORD f_size     = GetFileSize(file_handle, NULL);
    DWORD total_size = f_size + gap_size;
    char* raw        = (char*)AllocateMem(total_size);
    memset(raw, 0, total_size);

    DWORD chars_read = 0;
    ReadFile(file_handle, raw, f_size, &chars_read, NULL);

    CloseHandle(file_handle);

    // normalizing new lines form /r/n to /n
    char* normalized = (char*)AllocateMem(total_size);
    int chars_written = 0;
    for (DWORD i = 0; i < chars_read; i++) {

        if (raw[i] == '\r') {
            if (i + 1 < chars_read && raw[i + 1] == '\n') {
                normalized[chars_written++] = '\n';
                i++;
            } else {
                normalized[chars_written++] = '\n';
            }
        } else {
            normalized[chars_written++] = raw[i];
        }
    }

    if (file_size) *file_size = chars_written;
    if (buffer_size) *buffer_size = *file_size + gap_size;

    FreeMem(raw);
    return normalized;
}

void SaveGapBufferToFile(const char* file_path, char *buffer, int buffer_count, int gap_start, int gap_end) {
    
    HANDLE file_handle = CreateFileA(file_path, GENERIC_WRITE, 0,
                                     NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,
                                     NULL);

    DWORD bw;

    for (int index = 0; index < buffer_count; index++) {

        if (index >= gap_start && index <= gap_end) {
            index = gap_end;
            continue;
        }

        char ch = buffer[index];
        if (ch == '\n') WriteFile(file_handle, "\r\n", 2, &bw, NULL);   
        else            WriteFile(file_handle, &ch, 1, &bw, NULL);   
    }
}

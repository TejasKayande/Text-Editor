
@echo off
setlocal

cd /D "%~dp0"

if not exist "bin" (mkdir bin)

pushd bin

set FLAGS=/FC /EHsc /MP /Zi /W4
rem set set FLAGS=/Ox /MP /GL

set SYSTEM_LIBS=User32.lib Gdi32.lib Shell32.lib

set SOURCE_FILES=..\source\platform.c ..\source\editor.c ..\source\editor_view.c ..\source\input.c

set OUTPUT_EXE=editor.exe

call cl %FLAGS% %SOURCE_FILES% %SYSTEM_LIBS% ^
        /link /OUT:%OUTPUT_EXE% 

popd
endlocal

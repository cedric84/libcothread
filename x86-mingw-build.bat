@echo off

REM ---Definitions---
set PFX=x86-mingw
set OUT_PFX=out\%PFX%
set CFLAGS=-Wall -Werror -I./include
set SRC=./src/cothread.c ./src/%PFX%/cothread.S
set CC=%MINGW_HOME32%\bin\gcc

REM ---Create the output directory---
cmake -E make_directory ./out

REM ---Build---
%CC% %CFLAGS% %SRC% -o %OUT_PFX%-unittest	./unittest/main.c
%CC% %CFLAGS% %SRC% -o %OUT_PFX%-tuto0	 	./tuto0/main.c

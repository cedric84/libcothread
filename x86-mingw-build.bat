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
%CC% %CFLAGS% %SRC% -o %OUT_PFX%-unittest	^
	-I./unittest/include					^
	./unittest/src/unittest0.c				^
	./unittest/src/unittest1.c				^
	./unittest/src/main.c
%CC% %CFLAGS% %SRC% -o %OUT_PFX%-tuto0	 	./tuto0/main.c
%CC% %CFLAGS% %SRC% -o %OUT_PFX%-tuto1 -I./tuto1	^
	./tuto1/main.c		^
	./tuto1/stream.c	^
	./tuto1/parser0.c	^
	./tuto1/parser1.c

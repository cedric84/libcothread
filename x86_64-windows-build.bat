@echo off

REM ---Definitions---
set PFX=x86_64-windows
set OUT_PFX=out\%PFX%

REM ---Create output directories---
cmake -E make_directory ./obj
cmake -E make_directory ./out

REM ---Assemble---
REM https://learn.microsoft.com/en-us/cpp/assembler/masm/ml-and-ml64-command-line-reference?view=msvc-170
set ASM_FLAGS=	^
	/c	^
	/Fo obj\cothread-asm.obj	^
	/nologo	^
	/W3	^
	/WX	^
	src\%PFX%\cothread-asm.asm

ml64.exe %ASM_FLAGS%

REM ---Compile---
REM https://learn.microsoft.com/en-us/cpp/build/reference/compiler-command-line-syntax?view=msvc-170
set CFLAGS=	^
	/c	^
	/I include	^
	/MD	^
	/nologo	^
	/permissive-	^
	/W3	^
	/WX

CL %CFLAGS% /Fo:obj\unittest-main.obj /I unittest\include unittest\src\main.c
CL %CFLAGS% /Fo:obj\unittest-unittest0.obj /I unittest\include unittest\src\unittest0.c
CL %CFLAGS% /Fo:obj\unittest-unittest1.obj /I unittest\include unittest\src\unittest1.c
CL %CFLAGS% /Fo:obj\tuto0-main.obj tuto0\main.c
CL %CFLAGS% /Fo:obj\tuto1-main.obj /I tuto1 tuto1\main.c
CL %CFLAGS% /Fo:obj\tuto1-stream.obj /I tuto1 tuto1\stream.c
CL %CFLAGS% /Fo:obj\tuto1-parser0.obj /I tuto1 tuto1\parser0.c
CL %CFLAGS% /Fo:obj\tuto1-parser1.obj /I tuto1 tuto1\parser1.c
CL %CFLAGS% /Fo:obj\ src\cothread.c

REM ---Link---
REM https://learn.microsoft.com/en-us/cpp/build/reference/linker-options?view=msvc-170
set LDFLAGS=	^
	/MACHINE:X64	^
	/NOLOGO	^
	/SUBSYSTEM:CONSOLE

LINK %LDFLAGS% /OUT:%OUT_PFX%-unittest.exe	^
	obj\unittest-main.obj	^
	obj\unittest-unittest0.obj	^
	obj\unittest-unittest1.obj	^
	obj\cothread.obj	^
	obj\cothread-asm.obj
LINK %LDFLAGS% /OUT:%OUT_PFX%-tuto0.exe obj\tuto0-main.obj obj\cothread.obj obj\cothread-asm.obj
LINK %LDFLAGS% /OUT:%OUT_PFX%-tuto1.exe	^
	obj\tuto1-main.obj	^
	obj\tuto1-stream.obj	^
	obj\tuto1-parser0.obj	^
	obj\tuto1-parser1.obj	^
	obj\cothread.obj	^
	obj\cothread-asm.obj

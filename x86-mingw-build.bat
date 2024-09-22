@echo off

REM ---Definitions---
set PFX=x86-mingw
set CC=%MINGW_HOME32%\bin\gcc

REM ---Build---
%MINGW_HOME32%\bin\mingw32-make

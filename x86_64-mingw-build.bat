@echo off

REM ---Definitions---
set PFX=x86_64-mingw
set CC=%MINGW_HOME64%\bin\gcc

REM ---Build---
%MINGW_HOME64%\bin\mingw32-make

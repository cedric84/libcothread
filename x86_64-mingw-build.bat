@echo off

REM ---Definitions---
set PFX=x86_64-mingw
set CC=%MINGW_HOME64%\bin\gcc
set CXX=%MINGW_HOME64%\bin\g++

REM ---Build---
%MINGW_HOME64%\bin\mingw32-make
copy "%MINGW_HOME64%\bin\libgcc_s_seh-1.dll"	"out\libgcc_s_seh-1.dll"
copy "%MINGW_HOME64%\bin\libstdc++-6.dll"		"out\libstdc++-6.dll"

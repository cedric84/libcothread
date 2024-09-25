@echo off

REM ---Definitions---
set PFX=x86-mingw
set CC=%MINGW_HOME32%\bin\gcc
set CXX=%MINGW_HOME32%\bin\g++

REM ---Build---
%MINGW_HOME32%\bin\mingw32-make

REM ---Post-build---
copy "%MINGW_HOME32%\bin\libgcc_s_dw2-1.dll"	"out\libgcc_s_dw2-1.dll"
copy "%MINGW_HOME32%\bin\libstdc++-6.dll"		"out\libstdc++-6.dll"

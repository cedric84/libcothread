#! /bin/bash

#---Definitions---#
PFX=x86-gnu_linux
OUT_PFX=./out/${PFX}
CFLAGS="-Wall -Werror -I./include"
SRC="./src/cothread.c ./src/${PFX}/cothread.S"
CC=i686-linux-gnu-gcc

#---Create the output directory---#
mkdir -p ./out

#---Build---#
${CC}  ${CFLAGS} ${SRC} -o ${OUT_PFX}-unittest	./unittest/main.c	&&
${CC}  ${CFLAGS} ${SRC} -o ${OUT_PFX}-tuto0		./tuto0/main.c	&&
true

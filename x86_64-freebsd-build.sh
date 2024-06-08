#! /bin/sh

#---Definitions---#
PFX=x86_64-freebsd
OUT_PFX=./out/${PFX}
CFLAGS="-Wall -Werror -I./include"
SRC="./src/cothread.c ./src/${PFX}/cothread.S"

#---Create the output directory---#
mkdir -p ./out

#---Build---#
cc  ${CFLAGS} ${SRC} -o ${OUT_PFX}-unittest	./unittest/main.c	&&
cc  ${CFLAGS} ${SRC} -o ${OUT_PFX}-tuto0	./tuto0/main.c	&&
true

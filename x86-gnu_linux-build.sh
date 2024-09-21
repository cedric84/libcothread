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
${CC}  ${CFLAGS} ${SRC} -o ${OUT_PFX}-unittest	\
	-I./unittest/include						\
	./unittest/src/unittest0.c					\
	./unittest/src/unittest1.c					\
	./unittest/src/main.c	&&
${CC}  ${CFLAGS} ${SRC} -o ${OUT_PFX}-tuto0		./tuto0/main.c	&&
${CC}  ${CFLAGS} ${SRC} -o ${OUT_PFX}-tuto1 -I./tuto1	\
	./tuto1/main.c		\
	./tuto1/stream.c	\
	./tuto1/parser0.c	\
	./tuto1/parser1.c	&&
true

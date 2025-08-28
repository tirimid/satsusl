#!/bin/bash

INCLUDE="-Igsatsu -Ilibsatsu"
DEFINES=""
WARNINGS="-Wall -Wextra -Wshadow"
LIBRARIES="-L. -lsatsu-bin -lm $(pkg-config --cflags --libs sdl2 SDL2_ttf)"
CFLAGS="-std=c99 -pedantic -O3 -D_GNU_SOURCE"

# TODO: rewrite this to add dependencies.

CC=gcc
CFLAGS_FULL="$INCLUDE $DEFINES $WARNINGS $CFLAGS $LIBRARIES"

echo "[$0] build: compilation" >&2
$CC -o gsatsu-bin gsatsu-resources.o gsatsu/main.c $CFLAGS_FULL
if [ $? -ne 0 ]
then
	echo "[$0] build: failed to compile!" >&2
	exit 1
fi

echo "[$0] build: finished successfully" >&2

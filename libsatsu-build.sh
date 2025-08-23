#!/bin/bash

INCLUDE="-Ilibsatsu"
DEFINES=""
WARNINGS="-Wall -Wextra -Wshadow"
LIBRARIES=""
CFLAGS="-std=c99 -pedantic -g3 -fsanitize=address -D_GNU_SOURCE"

CC=gcc
AR=ar
CFLAGS_FULL="$INCLUDE $DEFINES $WARNINGS $CFLAGS $LIBRARIES"

echo "[$0] build: compilation" >&2
$CC -o libsatsu-bin.o -c libsatsu/lib.c $CFLAGS_FULL
if [ $? -ne 0 ]
then
	echo "[$0] build: failed to compile!" >&2
	exit 1
fi

echo "[$0] build: archiving" >&2
$AR -crs libsatsu-bin.a libsatsu-bin.o
if [ $? -ne 0 ]
then
	echo "[$0] build: failed to archive!" >&2
	exit 1
fi

echo "[$0] build: finished successfully" >&2

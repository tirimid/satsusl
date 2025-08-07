#!/bin/bash

INCLUDE="-Icsatsu"
DEFINES=""
WARNINGS="-Wall -Wextra -Wshadow"
LIBRARIES=""
CFLAGS="-std=c99 -pedantic -O3"

CC=gcc
CFLAGS_FULL="$INCLUDE $DEFINES $WARNINGS $CFLAGS $LIBRARIES"

echo "[$0] build: compilation" >&2
$CC -o csatsu-bin csatsu/main.c $CFLAGS_FULL
if [ $? -ne 0 ]
then
	echo "[$0] build: failed to compile!" >&2
	exit 1
fi

echo "[$0] build: finished successfully" >&2

#!/bin/bash

INCLUDE="-Igsatsu"
DEFINES=""
WARNINGS="-Wall -Wextra -Wshadow"
LIBRARIES=""
CFLAGS="-std=c++17 -pedantic -O3"

CPP=g++
CPPFLAGS_FULL="$INCLUDE $DEFINES $WARNINGS $CFLAGS $LIBRARIES"

echo "[$0] build: compilation" >&2
$CPP -o gsatsu-bin gsatsu/main.cc $CPPFLAGS_FULL
if [ $? -ne 0 ]
then
	echo "[$0] build: failed to compile!" >&2
	exit 1
fi

echo "[$0] build: finished successfully" >&2

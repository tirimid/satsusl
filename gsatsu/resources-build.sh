#!/bin/bash

INCLUDE="-Igsatsu"
DEFINES=""
WARNING="-Wall -Wextra -Wshadow"
LIBRARIES=""
CFLAGS="-std=c99 -pedantic"

CC=gcc
CFLAGS_FULL="$INCLUDE $DEFINES $WARNINGS $CFLAGS $LIBRARIES"

echo "[$0] resources: make fonts" >&2
make -B -s -C gsatsu/font > /dev/null
if [ $? -ne 0 ]
then
	echo "[$0] resources: failed to make fonts!" >&2
	exit 1
fi

echo "[$0] resources: compilation" >&2
$CC -o gsatsu-resources.o -c gsatsu/resources.c $CFLAGS_FULL
if [ $? -ne 0 ]
then
	echo "[$0] resources: failed to compile!" >&2
	exit 1
fi

echo "[$0] resources: finished successfully" >&2

#!/bin/bash

INSTALLLIB="/usr/lib"
INSTALLHEADER="/usr/include/satsu"

echo "[$0] install: pre-install checks" >&2

if [ $EUID -ne 0 ]
then
	echo "[$0] install: run the script as root!" >&2
	exit 1
fi

stat libsatsu-bin.a > /dev/null 2> /dev/null
if [ $? -ne 0 ]
then
	echo "[$0] install: build the library before installing!" >&2
	exit 1
fi

echo "[$0] install: copying library" >&2
cp libsatsu-bin.a $INSTALLLIB/libsatsu.a 2> /dev/null
if [ $? -ne 0 ]
then
	echo "[$0] install: failed to copy library!" >&2
	exit 1
fi

echo "[$0] install: making fresh header directory" >&2
rm -rf $INSTALLHEADER
mkdir -p $INSTALLHEADER
if [ $? -ne 0 ]
then
	echo "[$0] install: failed to make fresh header directory!" >&2
	exit 1
fi

echo "[$0] install: copying header" >&2
cp libsatsu/satsu.h $INSTALLHEADER
if [ $? -ne 0 ]
then
	echo "[$0] install: failed to copy header!" >&2
	exit 1
fi

echo "[$0] install: finished successfully" >&2

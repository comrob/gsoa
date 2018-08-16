#!/bin/sh

if [ `uname` = FreeBSD ]
then
   gmake clean
else
   make clean
fi

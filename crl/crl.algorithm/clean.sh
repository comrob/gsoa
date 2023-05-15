#!/bin/sh

if [ `uname` = FreeBSD ]
then
   gmake INSTALL_SUFFIX=.fbsd clean
else
   make clean
fi

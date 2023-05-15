#!/bin/sh

if [ `uname` = FreeBSD ]
then
   gmake INSTALL_SUFFIX=.fbsd dist
else
   make dist
fi

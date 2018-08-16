#!/bin/sh

if [ `uname` = FreeBSD ]
then
   cmd=gmake 
else
   cmd=make 
fi

#export CXXFLAGS=-std=c++17

rm -rf build

mkdir -p build


cd build && cmake -D CMAKE_INSTALL_PREFIX=. ..
$cmd -C crl install
$cmd gsoa-etsp 
$cmd -C gsoa-etsp install
$cmd gsoa-cetsp 
$cmd -C gsoa-cetsp install
cd -


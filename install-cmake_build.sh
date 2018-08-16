#!/bin/sh

if [ `uname` = FreeBSD ]
then
   cmd=gmake 
else
   cmd=make 
fi
#export CXXFLAGS=-std=c++11
export CXXFLAGS=-std=c++17

rm -rf build

mkdir -p build
cd build && cmake -D CMAKE_INSTALL_PREFIX=../ ../
$cmd install
cd -


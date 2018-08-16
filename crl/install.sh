#!/bin/sh

if [ `uname` = FreeBSD ]
then
   cmd=gmake 
else
   cmd=make 
fi
#export CXXFLAGS=-std=c++11
export CXXFLAGS=-std=c++17

libs="crl crl.gui crl.algorithm crl.loader crl.tsplib"
for lib in $libs
do
   $cmd -C $lib install $@
done

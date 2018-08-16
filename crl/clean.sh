#!/bin/sh

if [ `uname` = FreeBSD ]
then
   cmd=gmake 
else
   cmd=make 
fi

libs="crl crl.gui crl.algorithm crl.loader crl.tsplib"
for lib in $libs
do
   $cmd -C $lib clean
done

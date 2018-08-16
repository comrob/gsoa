#
# Date:      2009/08/26 14:27
# Author:    Jan Faigl
#

SRC=src
LIBRARY=libcrl.a

#installation prefix
INSTALL_PREFIX=../..

include ../Mk/libs.mk

#CXXFLAGS+=-std=c++11

CXXFLAGS+=-O2
#CXXFLAGS+=-g

CPPFLAGS+=$(BOOST_CPPFLAGS)
CPPFLAGS+=$(LOG4CXX_CPPFLAGS)
CPPFLAGS+=-I.

LIBRARY_OBJS=$(patsubst %.cc,%.o,$(wildcard $(SRC)/*.cc))
include ../Mk/comrob.mk

install: lib
	mkdir -p $(INCLUDE_PREFIX)/crl
	cp $(SRC)/*.h $(INCLUDE_PREFIX)/crl

deinstall:
	$(RM) -r $(LIBRARY_PREFIX) $(INCLUDE_PREFIX)
	$(RM) $(LIBRARY)


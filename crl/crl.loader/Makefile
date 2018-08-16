#
# Date:      2009/08/26 14:27
# Author:    Jan Faigl
#

SRC=src
LIBRARY=libcrl-loader.a

#installation prefix
INSTALL_PREFIX=../..

include ../Mk/libs.mk

#CXXFLAGS+=-std=c++11
CXXFLAGS+=-O2
#CXXFLAGS+=-O2 -ggdb3

CPPFLAGS+=$(BOOST_CPPFLAGS)
CPPFLAGS+=$(LOG4CXX_CPPFLAGS)

CPPFLAGS+=-I$(INSTALL_PREFIX)/include
CPPFLAGS+=-I.

LIBRARY_OBJS=$(patsubst %.cc,%.o,$(wildcard $(SRC)/*.cc))
include ../Mk/comrob.mk

install: lib
	mkdir -p $(INCLUDE_PREFIX)/crl/loader
	cp $(SRC)/*.h $(INCLUDE_PREFIX)/crl/loader

deinstall:
	$(RM) -r $(LIBRARY_PREFIX) $(INCLUDE_PREFIX)
	$(RM) $(LIBRARY)


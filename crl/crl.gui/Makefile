#
# Date:      2009/08/26 14:27
# Author:    Jan Faigl
#

SRC=src
LIBRARY=libcrl-gui.a

#installation prefix
INSTALL_PREFIX=../..

include ../Mk/libs.mk

#CXXFLAGS+=-std=c++11
CXXFLAGS+=-O2

CPPFLAGS+=$(BOOST_CPPFLAGS)
CPPFLAGS+=$(LOG4CXX_CPPFLAGS)
CPPFLAGS+=$(CAIRO_CPPFLAGS)
CPPFLAGS+=-I$(INSTALL_PREFIX)/include
CPPFLAGS+=-I.

LIBRARY_OBJS=$(patsubst %.cc,%.o,$(wildcard $(SRC)/*.cc))
include ../Mk/comrob.mk

install: lib
	mkdir -p $(INCLUDE_PREFIX)/crl/gui
	cp $(SRC)/*.h $(INCLUDE_PREFIX)/crl/gui

deinstall:
	$(RM) -r $(LIBRARY_PREFIX) $(INCLUDE_PREFIX)
	$(RM) $(LIBRARY)


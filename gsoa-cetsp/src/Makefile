#
# Date:      2011/07/11 17:55
# Author:    Jan Faigl
#

CXX:=ccache $(CXX)

include Mk/libs.mk

#CXXFLAGS+=-std=c++11
CPPFLAGS+=$(LOCAL_CFLAGS)
LDFLAGS+=$(LOCAL_LDFLAGS)

CPPFLAGS+=$(CRL_CFLAGS) $(BOOST_CFLAGS) $(CAIRO_CFLAGS) $(LOG4CXX_CPPFLAGS)
LDFLAGS+=$(CRL-ALGORITHM) $(CRL-GUI_LDFLAGS) $(CRL_LDFLAGS) $(CAIRO_LDFLAGS) $(BOOST_LDFLAGS) $(LOG4CXX_LDFLAGS)

CXXFLAGS+=-std=c++11
CXXFLAGS+=-O2 -march=native

OBJS=\
     tgsoa-cetsp.o\
     gsoa.o\
     gsoa_ring.o\
     gsoa_learning.o\
     route_path_utils.o\
     coords.o\

TARGET=tgsoa-cetsp

include Mk/comrob.mk

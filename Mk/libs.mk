
# Date:      2009/08/26 14:13
# Author:    Jan Faigl
#

UNAME=$(shell uname)

#uncomment if the ccache is installed
#CXX:=ccache $(CXX)

ifeq ($(UNAME),FreeBSD)
   BOOST_CPPFLAGS=-I/usr/local/include
   BOOST_LDFLAGS=-L/usr/local/lib
else
   BOOST_CPPFLAGS=-I/usr/include
   BOOST_LDFLAGS=-L/usr/lib
   BOOST_IOSTREAMS_LDFLAGS=-lboost_iostreams
endif

#--------------------------------------------------------
# BOOST
#--------------------------------------------------------
BOOST_THREADS_LDFLAGS=-lboost_thread
BOOST_FILESYSTEM_LDFLAGS=-lboost_filesystem
BOOST_PROGRAM_OPTIONS_LDFLAGS=-lboost_program_options
BOOST_IOSTREAMS_LDFLAGS=-lboost_iostreams

#--------------------------------------------------------
# LOG4CXX
#--------------------------------------------------------
LOG4CXX_CPPFLAGS:=$(shell pkg-config --cflags liblog4cxx)
LOG4CXX_LDFLAGS:=$(shell pkg-config --libs liblog4cxx)

#--------------------------------------------------------
# CAIRO
#--------------------------------------------------------
CAIRO_CPPFLAGS:=$(shell pkg-config --cflags cairo)
CAIRO_LDFLAGS:=$(shell pkg-config --libs cairo)


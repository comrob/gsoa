# - Try to find the log4cxx logging library
# Will define:
#
# Log4cxx_INCLUDE_DIRS - include directories needed to use the library
# Log4cxx_LIBRARIES - Libraries to link agains fpr the driver
#
# Possible hints:
# Log4cxx_VISUAL_STUDIO_PROJECT - root directory of the log4cxx visual studio project on windows
#
# Copyright (C) 2010 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
#
# This program is free software; you can redistribute it
# and/or modify it under the terms of the GNU General
# Public License as published by the Free Software Foundation;
# either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#

include(FindPackageHandleStandardArgs)

find_path(Log4cxx_INCLUDE_DIRS
   PATHS ${Log4cxx_VISUAL_STUDIO_PROJECT}/src/main/include
   NAMES logger.h
   PATH_SUFFIXES log4cxx)

find_library(Log4cxx_LIBRARIES
   PATHS ${Log4cxx_VISUAL_STUDIO_PROJECT}/projects/Debug
   ${Log4cxx_VISUAL_STUDIO_PROJECT}/projects/Release
   NAMES log4cxx)

# post-process inlude path
if(Log4cxx_INCLUDE_DIRS)
   string(REGEX REPLACE log4cxx$$ "" Log4cxx_INCLUDE_DIRS ${Log4cxx_INCLUDE_DIRS})
   set(Log4cxx_INCLUDE_DIRS ${Log4cxx_INCLUDE_DIRS} CACHE PATH "log4cxx include dirs" FORCE)
endif()

find_package_handle_standard_args(Log4cxx DEFAULT_MSG Log4cxx_INCLUDE_DIRS Log4cxx_LIBRARIES)

# only visible in advanced view
mark_as_advanced(Log4cxx_INCLUDE_DIRS Log4cxx_LIBRARIES)

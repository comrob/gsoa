# - Try to find the cairo library
# Once done this will define
#
#  Cairo_FOUND - system has cairo
#  Cairo_INCLUDE_DIRS - the cairo include directory
#  Cairo_LIBRARIES - Link these to use cairo
#
# Define Cairo_MIN_VERSION for which version desired.
#

include(FindPkgConfig)

pkg_check_modules(PC_CAIRO cairo) 

find_path(Cairo_INCLUDE_DIRS
    NAMES cairo.h
    HINTS ${PC_CAIRO_INCLUDEDIR}
          ${PC_CAIRO_INCLUDE_DIRS}
    PATH_SUFFIXES cairo
)

find_library(Cairo_LIBRARIES
    NAMES cairo pthread
    HINTS ${PC_CAIRO_LIBDIR}
          ${PC_CAIRO_LIBRARY_DIRS}
)

IF (Cairo_INCLUDE_DIRS)
    IF (EXISTS "${Cairo_INCLUDE_DIRS}/cairo-version.h")
        FILE(READ "${Cairo_INCLUDE_DIRS}/cairo-version.h" CAIRO_VERSION_CONTENT)

        STRING(REGEX MATCH "#define +CAIRO_VERSION_MAJOR +([0-9]+)" _dummy "${CAIRO_VERSION_CONTENT}")
        SET(CAIRO_VERSION_MAJOR "${CMAKE_MATCH_1}")

        STRING(REGEX MATCH "#define +CAIRO_VERSION_MINOR +([0-9]+)" _dummy "${CAIRO_VERSION_CONTENT}")
        SET(CAIRO_VERSION_MINOR "${CMAKE_MATCH_1}")

        STRING(REGEX MATCH "#define +CAIRO_VERSION_MICRO +([0-9]+)" _dummy "${CAIRO_VERSION_CONTENT}")
        SET(CAIRO_VERSION_MICRO "${CMAKE_MATCH_1}")

        SET(CAIRO_VERSION "${CAIRO_VERSION_MAJOR}.${CAIRO_VERSION_MINOR}.${CAIRO_VERSION_MICRO}")
    ENDIF ()
ENDIF ()

# FIXME: Should not be needed anymore once we start depending on CMake 2.8.3
SET(VERSION_OK TRUE)
IF (Cairo_FIND_VERSION)
    IF (Cairo_FIND_VERSION_EXACT)
        IF ("${Cairo_FIND_VERSION}" VERSION_EQUAL "${CAIRO_VERSION}")
            # FIXME: Use IF (NOT ...) with CMake 2.8.2+ to get rid of the ELSE block
        ELSE ()
            SET(VERSION_OK FALSE)
        ENDIF ()
    ELSE ()
        IF ("${Cairo_FIND_VERSION}" VERSION_GREATER "${CAIRO_VERSION}")
            SET(VERSION_OK FALSE)
        ENDIF ()
    ENDIF ()
ENDIF ()

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Cairo DEFAULT_MSG Cairo_INCLUDE_DIRS Cairo_LIBRARIES VERSION_OK)

mark_as_advanced(Cairo_LIBRARIES Cairo_INCLUDE_DIRS)

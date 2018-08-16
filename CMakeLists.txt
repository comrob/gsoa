cmake_minimum_required(VERSION 2.8)
project(crl)

# Debug / Release
if(CMAKE_BUILD_TYPE MATCHES Debug)
    message("Compiling in the Debug mode")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
else()
    message("Compiling in the Release mode")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")
    set(CMAKE_BUILD_TYPE Release)
endif()

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_STANDARD 11) #would use gnu++11

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/crl/cmake")

enable_testing()
add_subdirectory(crl)
if(HAS_CRL)
   message("Crl found add gsoa-etsp and gsoa-cetsp")
   find_package(X11)
   find_package(Log4cxx)
   add_subdirectory(gsoa-etsp)
   add_subdirectory(gsoa-cetsp)
endif(HAS_CRL)

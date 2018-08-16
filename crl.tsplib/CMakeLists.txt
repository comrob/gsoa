cmake_minimum_required(VERSION 2.8)
project(crl-tsplib)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
   src/loadertour.cc
   src/loadertsp.cc
   src/tsplib.cc
   src/tsplibloader.cc
   src/tsplibspec.cc
   )

add_library(crl-tsplib STATIC ${SOURCE_FILES})

add_executable(tsploader src/tloader.cc ${SOURCE_FILES})
target_link_libraries(tsploader crl-gui)

install(DIRECTORY src/ DESTINATION include/crl/tsp FILES_MATCHING PATTERN "*.h")
install(TARGETS crl-tsplib DESTINATION lib)
install(TARGETS tsploader DESTINATION bin)

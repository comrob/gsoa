cmake_minimum_required(VERSION 2.8)
project(crl-loader)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
   src/distances_module.cc
   src/graph_module.cc
   src/loader_types.cc
   src/map_partition_module.cc
   src/route_module.cc
   src/text_format_loader.cc
   src/text_stream.cc
)

add_library(crl-loader STATIC ${SOURCE_FILES})
target_link_libraries(crl-loader crl)

install(DIRECTORY src/ DESTINATION include/crl/loader FILES_MATCHING PATTERN "*.h")
install(TARGETS crl-loader DESTINATION lib)

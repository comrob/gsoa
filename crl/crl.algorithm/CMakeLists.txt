cmake_minimum_required(VERSION 2.8)
project(crl-algorithm)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
   src/algorithm.cc
   src/algorithm_base.cc
   src/result_log.cc
   src/text_result_log.cc
)

add_library(crl-algorithm STATIC ${SOURCE_FILES})
#target_include_directories(crl-algorithm PUBLIC crl)
target_link_libraries(crl-algorithm)

install(DIRECTORY src/ DESTINATION include/crl/alg FILES_MATCHING PATTERN "*.h")
install(TARGETS crl-algorithm DESTINATION lib)

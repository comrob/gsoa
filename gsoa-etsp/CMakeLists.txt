cmake_minimum_required(VERSION 2.8.12)
project(gsoa-etsp)

set(SOURCE_FILES
   src/coords.cc
   src/gsoa.cc
   src/gsoa_learning.cc
   src/gsoa_ring.cc
   src/route_path_utils.cc
   )

add_executable(tgsoa-etsp src/tgsoa-etsp.cc ${SOURCE_FILES})
target_include_directories(tgsoa-etsp PUBLIC ${CMAKE_BINARY_DIR}/include) 
target_link_libraries(tgsoa-etsp crl-algorithm crl-gui)

install(TARGETS tgsoa-etsp DESTINATION .)
install(FILES tgsoa-etsp.cfg DESTINATION .)
install(DIRECTORY etc DESTINATION .)


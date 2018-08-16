cmake_minimum_required(VERSION 2.8)
project(gsoa-cetsp)

set(SOURCE_FILES
   src/coords.cc
   src/gsoa.cc
   src/gsoa_learning.cc
   src/gsoa_ring.cc
   src/route_path_utils.cc
   )

add_executable(tgsoa-cetsp src/tgsoa-cetsp.cc ${SOURCE_FILES})
target_include_directories(tgsoa-cetsp PUBLIC ${CMAKE_BINARY_DIR}/include) 
target_link_libraries(tgsoa-cetsp crl-algorithm crl-gui)

install(TARGETS tgsoa-cetsp DESTINATION .)
install(FILES tgsoa-cetsp.cfg DESTINATION .)
install(DIRECTORY etc DESTINATION .)

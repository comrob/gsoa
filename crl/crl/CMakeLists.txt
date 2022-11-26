cmake_minimum_required(VERSION 2.8.12)
project(crl)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
   src/bbox.cc
   src/file_utils.cc
   src/logging.cc
   src/proc_signal.cc
   src/random.cc
   src/thread.cc
   src/timerN.cc
   src/waitobject.cc
   src/ws.cc
)

add_library(crl STATIC ${SOURCE_FILES})
target_include_directories(crl PUBLIC ${Boost_INCLUDE_DIRS} ${Log4cxx_INCLUDE_DIRS})
target_link_libraries(crl ${Log4cxx_LIBRARIES} ${Boost_LIBRARIES})

install(DIRECTORY src/ DESTINATION include/crl FILES_MATCHING PATTERN "*.h")
install(TARGETS crl DESTINATION lib)

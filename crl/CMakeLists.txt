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

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# For debug and profilling:
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")

#message("CMAKE_CXX_COMPILER_ID  ${CMAKE_CXX_COMPILER_ID}")
#if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
#    MESSAGE("Disable error messsages for unused-command-line-argument (Clang only)")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=unused-command-line-argument -Qunused-arguments")
#endif()
#MESSAGE("CMAKE_C_COMPILER_ID  ${CMAKE_C_COMPILER_ID}")
#if("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
#    MESSAGE("Disable error messsages for unused-command-line-argument (Clang only)")
#    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error=unused-command-line-argument -Qunused-arguments")
#endif()

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

enable_testing()
find_package(Boost REQUIRED COMPONENTS filesystem program_options thread)
find_package(Cairo)
find_package(X11)
find_package(Log4cxx)

if(Boost_FOUND AND Log4cxx_FOUND)
   message("Boost libraries OK")
   execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/include/crl.core)
   execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${PROJECT_SOURCE_DIR}/crl/src ${CMAKE_CURRENT_BINARY_DIR}/include/crl.core/crl)
   include_directories(${CMAKE_CURRENT_BINARY_DIR}/include/crl.core)
   add_subdirectory(crl) 

   #propage boost and log4cxx headers files to other modules depending on the crl
   include_directories(${Boost_INCLUDE_DIRS} ${Log4cxx_INCLUDE_DIRS})
else()
   message("Boost not found")
endif()

if(Boost_FOUND AND Log4cxx_FOUND AND Cairo_FOUND AND X11_FOUND)
   message("Cairo and X11 OK")
   include_directories(${Cairo_INCLUDE_DIRS})

   execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/include/crl)
   include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

   execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${PROJECT_SOURCE_DIR}/crl.gui/src ${CMAKE_CURRENT_BINARY_DIR}/include/crl/gui)
   add_subdirectory(crl.gui)

   execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${PROJECT_SOURCE_DIR}/crl.algorithm/src ${CMAKE_CURRENT_BINARY_DIR}/include/crl/alg)
   add_subdirectory(crl.algorithm)

   execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${PROJECT_SOURCE_DIR}/crl.loader/src ${CMAKE_CURRENT_BINARY_DIR}/include/crl/loader)
   add_subdirectory(crl.loader)

   execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${PROJECT_SOURCE_DIR}/crl.tsplib/src ${CMAKE_CURRENT_BINARY_DIR}/include/crl/tsp)
   add_subdirectory(crl.tsplib)
   set(HAS_CRL TRUE CACHE INTERNAL "")
else()
   message("Cairo not found")
endif()

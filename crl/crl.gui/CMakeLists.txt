cmake_minimum_required(VERSION 2.8.12)
project(crl-gui)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
   src/cairogui.cc
   src/cairowin.cc
   src/cairowin_offscreen.cc
   src/cairoxrender.cc
   src/cairoxwin.cc
   src/cairoxwinsimple.cc
   src/canvas.cc
   src/canvas_object_loader.cc
   src/canvas_object_saver.cc
   src/canvascontainer.cc
   src/colormap.cc
   src/colors.cc
   src/guievent.cc
   src/guifactory.cc
   src/guihandler.cc
   src/manager.cc
   src/markers.cc
   src/mouseclick.cc
   src/renderer_object.cc
   src/renderercairo.cc
   src/shape.cc
   src/shapes.cc
   src/view_transform.cc
   src/waitforclick.cc
   src/win_adjust_size.cc
   src/xcolors.cc
   src/cairogui.cc
   src/cairowin.cc
   src/cairowin_offscreen.cc
   src/cairoxrender.cc
   src/cairoxwin.cc
   src/cairoxwinsimple.cc
   src/canvas.cc
   src/canvas_object_loader.cc
   src/canvas_object_saver.cc
   src/canvascontainer.cc
   src/colormap.cc
   src/colors.cc
   src/guievent.cc
   src/guifactory.cc
   src/guihandler.cc
   src/manager.cc
   src/markers.cc
   src/mouseclick.cc
   src/renderer_object.cc
   src/renderercairo.cc
   src/shape.cc
   src/shapes.cc
   src/view_transform.cc
   src/waitforclick.cc
   src/win_adjust_size.cc
   src/xcolors.cc
   )

add_library(crl-gui STATIC ${SOURCE_FILES})
target_include_directories(crl-gui PUBLIC ${Cairo_INCLUDE_DIRS})

target_link_libraries(crl-gui crl ${Cairo_LIBRARIES} ${X11_LIBRARIES}) 

find_package(Threads REQUIRED)
if(THREADS_HAVE_PTHREAD_ARG)
  target_compile_options(crl-gui PUBLIC "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(crl-gui "${CMAKE_THREAD_LIBS_INIT}")
endif()

install(DIRECTORY src/ DESTINATION include/crl/gui FILES_MATCHING PATTERN "*.h")
install(TARGETS crl-gui DESTINATION lib)

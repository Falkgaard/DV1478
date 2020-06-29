cmake_minimum_required(VERSION 3.14)

project( "DV1478"
         VERSION 0.1.0
       # DESCRIPTION  TODO
       # HOMEPAGE_URL TODO
         LANGUAGES C CXX )

set(CMAKE_CXX_STANDARD 20)

# Step 1: Add hello_imgui as a subdirectory
add_subdirectory(lib/hello_imgui) # TODO: add lib instead and add hello_imgui etc in lib/CMakeLists.txt
add_subdirectory(lib/stb)
add_subdirectory(lib/falk)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${HELLOIMGUI_BASEPATH}/hello_imgui_cmake)

# include hello_imgui_add_app
include(hello_imgui_add_app)

# (emscripten only): include hello_imgui_emscripten_global_options
if (EMSCRIPTEN)
    include(${HELLOIMGUI_BASEPATH}/hello_imgui_cmake/emscripten/hello_imgui_emscripten_global_options.cmake)
endif()

# Step 4: add app
hello_imgui_add_app( dv1478_app src/main.cpp )

#add_executable(my_executable main.cpp)
#target_compile_features(my_executable PRIVATE cxx_std_20)


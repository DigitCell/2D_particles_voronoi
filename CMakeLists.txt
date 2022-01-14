cmake_minimum_required(VERSION 3.14)



# SDL2, glew, glm, ImGui
add_subdirectory("external")
set(EXECUTABLE_NAME "Voronoi_exp")

#set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 14)

project(${EXECUTABLE_NAME}
    VERSION     0.05
    DESCRIPTION "SDL2 OpenGL Marching Cubes Boids "
    LANGUAGES CXX
)


# production
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fpermissive -std=c++14 -O3 -Wall -Wextra -pedantic -Wno-unused-parameter")

#GLM LIB
set(GLM_DIR  ${CMAKE_SOURCE_DIR}/external/glm-0.9.9.8/cmake/glm)
find_package(GLM REQUIRED)
include_directories(${GLM_INCLUDE_DIRS})
link_directories(${GLM_LIBRARY_DIRS})
add_definitions(${GLM_DEFINITIONS})
if(NOT GLM_FOUND)
        message(Error "GLM not found")
endif(NOT GLM_FOUND)

# IMGUI LIB
set (IMGUILIB ImGuiLibrary)
set(IMGUI_ROOT ${CMAKE_SOURCE_DIR}/external/imgui-1.85)
set(IMGUI_SOURCES ${IMGUI_ROOT}/imgui.cpp ${IMGUI_ROOT}/imgui_draw.cpp ${IMGUI_ROOT}/imgui_widgets.cpp ${IMGUI_ROOT}/imgui_tables.cpp)
set(IMGUI_IMPL_SOURCES ${IMGUI_ROOT}/backends/imgui_impl_sdl.cpp ${IMGUI_ROOT}/backends/imgui_impl_opengl3.cpp)
add_library(${IMGUILIB} STATIC ${IMGUI_SOURCES} ${IMGUI_IMPL_SOURCES} )
target_include_directories(${IMGUILIB} PUBLIC "${IMGUI_ROOT}")
target_include_directories(${IMGUILIB} PUBLIC "${IMGUI_ROOT}/backends")

# search for pkg-config
include (FindPkgConfig)
if(NOT PKG_CONFIG_FOUND)
    message (FATAL_ERROR "pkg-config not found")
endif()

# check for libpng
pkg_check_modules (LIBPNG REQUIRED)
if (NOT LIBPNG_FOUND)
    message(FATAL_ERROR "You don't seem to have libpng16 development libraries installed")
else()
    include_directories (${LIBPNG_INCLUDE_DIRS})
    link_directories (${LIBPNG_LIBRARY_DIRS})
    link_libraries (${LIBPNG_LIBRARIES})
endif()

include(FindOpenMP)
if(NOT OPENMP_FOUND)
    message("OpenMP not found. Trying '-fopenmp=libiomp5'")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp=libiomp5")
else()
    message("OpenMP found: ${OpenMP_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()



SET(TARGET_H

    src/Constants.hpp
    src/structs.hpp

    src/graphmodule.hpp
    src/mainloop.hpp
    src/solver.hpp

    src/support/shaderutils.hpp
    src/support/arcball_camera.hpp
    src/support/Buffer.h
    src/support/ComputeProcess.h
    src/support/ConfigParser.h
    src/support/Icosphere.h
    src/support/NoiseSettings.h
    src/support/texture.hpp

    src/support/meshData.h
    src/support/meshCreator.h
)

SET(TARGET_SRC

    src/graphmodule.cpp
    src/mainloop.cpp
    src/solver.cpp

    src/support/shaderutils.cpp
    src/support/Buffer.cpp
    src/support/ComputeProcess.cpp
    src/support/ConfigParser.cpp
    src/support/Icosphere.cpp
    src/support/NoiseSettings.cpp
    src/support/texture.cpp

    src/support/meshData.cpp

    src/main.cpp
)


add_executable(${EXECUTABLE_NAME} ${TARGET_SRC} ${TARGET_H})

target_link_libraries(${EXECUTABLE_NAME} ${IMGUILIB} )


set(homeDir "/home/geotyper")
set(stb_lib  ${homeDir}/CPP_lib/stb)
include_directories(${stb_lib})

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES}")
target_include_directories(${EXECUTABLE_NAME} PUBLIC "${GLM_DIR}")

# Windows prereq: get dev libraries from https://www.libsdl.org/download-2.0.php
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
# workaround for CMP0004 issue with the SDL2 libraries on Linux
string(STRIP "${SDL2_LIBRARIES}" SDL2_LIBRARIES)
target_link_libraries(${EXECUTABLE_NAME} ${SDL2_LIBRARIES})

# OpenGL
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED COMPONENTS OpenGL)
if(NOT TARGET OpenGL::OpenGL)
  target_link_libraries(${EXECUTABLE_NAME} OpenGL::GL)
  target_include_directories(${EXECUTABLE_NAME} PUBLIC OpenGL::GL)
else()
  target_link_libraries(${EXECUTABLE_NAME} OpenGL::OpenGL)
  target_include_directories(${EXECUTABLE_NAME} PUBLIC OpenGL::OpenGL)
endif()

# GL Extension Wrangler (GLEW) get dev libraries from http://glew.sourceforge.net/
find_package(GLEW REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} "${GLEW_LIBRARIES}" -ldl -lpthread )
target_include_directories(${EXECUTABLE_NAME} PUBLIC "${GLEW_INCLUDE_DIRS}")

#OpenCV
find_package( OpenCV REQUIRED )
include_directories(${EXECUTABLE_NAME} ${OpenCV_INCLUDE_DIRS} )
target_link_libraries(${EXECUTABLE_NAME} ${OpenCV_LIBS} )


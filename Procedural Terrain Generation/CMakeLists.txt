cmake_minimum_required(VERSION 3.8)
project(Procedural_Terrain_Generation)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")

find_package(Freetype REQUIRED)
find_package(OpenGL REQUIRED COMPONENTS OpenGL)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(OpenCL REQUIRED)


include_directories(${CMAKE_SOURCE_DIR}/external/Include)
include_directories(src/)

aux_source_directory(src SOURCE)

add_executable(Procedural_Terrain_Generation src/Main.cpp ${SOURCE})

target_link_libraries(Procedural_Terrain_Generation 
                      OpenCL::OpenCL 
                      OpenGL::GL 
                      GLEW::glew 
                      glfw) 

target_include_directories(Procedural_Terrain_Generation PRIVATE 
                           ${FREETYPE_INCLUDE_DIRS}
                           ${OPENCL_INCLUDE_DIR} )






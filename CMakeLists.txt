cmake_minimum_required (VERSION 2.6)
 
project (TI1805-Project)

find_package (GLUT REQUIRED)
find_package (OpenGL REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/src)
add_subdirectory(src)

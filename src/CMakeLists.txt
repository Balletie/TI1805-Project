cmake_minimum_required (VERSION 2.6)
 
project (TI1805-Project)

find_package (GLUT REQUIRED)
find_package (OpenGL REQUIRED)

file (GLOB SOURCES *.cpp)
add_executable (project ${SOURCES})
target_link_libraries (project ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})

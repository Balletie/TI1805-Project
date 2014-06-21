add_subdirectory(kdtree)
add_subdirectory(shapes)

set (main
	raytracing.cpp
	mesh.cpp
	main.cpp
)

add_executable (project ${main})
target_link_libraries (project shapes kdtree ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})

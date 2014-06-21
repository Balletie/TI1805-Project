add_subdirectory(shapes)

set (main
	kdnode.cpp
	raytracing.cpp
	mesh.cpp
	main.cpp
)

add_executable (project ${main})
target_link_libraries (project shapes ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})

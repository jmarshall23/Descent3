SET (CPPS
		"tr_clipper.cpp"
		"tr_draw.cpp"
		"tr_globalvars.cpp"
		"tr_instance.cpp"
		"gl_main.cpp"
		"tr_points.cpp"
		"tr_setup.cpp"
		"tr_transforms.cpp"
	    glew.c
)

IF(UNIX)

  SET (CPPS ${CPPS} lnxscreenmode.cpp)

ENDIF()

# These are excluded.
#opengl.cpp
#renderer.cpp
#Direct3D.cpp

ADD_LIBRARY(renderer STATIC ${CPPS} "gl_image.cpp" "gl_render.cpp" "gl_init.cpp")
target_link_libraries(renderer opengl32)
SET (HEADERS 
		HardwareInternal.h
		RendererConfig.h
		)
SET (CPPS
		HardwareClipper.cpp
		HardwareDraw.cpp
		HardwareGlobalVars.cpp
		HardwareInstance.cpp
		HardwareOpenGL.cpp
		HardwarePoints.cpp
		HardwareSetup.cpp
		HardwareTransforms.cpp
	    glew.c
		
		
		
		
		
		
		
		)

IF(UNIX)

  SET (CPPS ${CPPS} lnxscreenmode.cpp)

ENDIF()

# These are excluded.
#opengl.cpp
#renderer.cpp
#Direct3D.cpp

ADD_LIBRARY(renderer STATIC ${HEADERS} ${CPPS})
target_link_libraries(renderer opengl32)
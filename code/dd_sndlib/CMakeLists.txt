SET (HEADERS 
		ia3dapi.h
		Ia3dutil.h
		vmanpset.h)
SET (CPPS
		geometry.cpp
		llsopenal.cpp
		wave.cpp
		ssl_lib.cpp)

ADD_LIBRARY(dd_sndlib STATIC ${HEADERS} ${CPPS})
target_link_libraries(dd_sndlib ${CMAKE_SOURCE_DIR}/libopenal/libs/Win32/OpenAL32.lib)
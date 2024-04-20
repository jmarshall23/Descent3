SET (CPPS
		imgui.cpp
		imgui_draw.cpp
		imgui_impl_opengl3.cpp
		imgui_impl_win32.cpp
		imgui_tables.cpp
		imgui_widgets.cpp			
		)

ADD_LIBRARY(libimgui STATIC ${HEADERS} ${CPPS})
target_link_libraries(libimgui opengl32)
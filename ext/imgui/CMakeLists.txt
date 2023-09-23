add_library(imgui
	imconfig.h
	imgui.h
	imgui.cpp
	imgui_demo.cpp
	imgui_draw.cpp
	imgui_internal.h
	imgui_tables.cpp
	imgui_widgets.cpp
	imgui_stdlib.cpp
	backends/imgui_impl_glfw.cpp
	backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC "${PROJECT_SOURCE_DIR}/ext/imgui")
target_include_directories(imgui PUBLIC "${PROJECT_SOURCE_DIR}/ext/imgui/backends")
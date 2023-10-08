project(imgui)

add_library(${PROJECT_NAME}
    "${PROJECT_SOURCE_DIR}/imgui/imgui.cpp"
    "${PROJECT_SOURCE_DIR}/imgui/imgui_demo.cpp"
    "${PROJECT_SOURCE_DIR}/imgui/imgui_draw.cpp"
    "${PROJECT_SOURCE_DIR}/imgui/imgui_tables.cpp"
    "${PROJECT_SOURCE_DIR}/imgui/imgui_widgets.cpp"
    "${PROJECT_SOURCE_DIR}/imgui/backends/imgui_impl_glfw.cpp"
    "${PROJECT_SOURCE_DIR}/imgui/backends/imgui_impl_opengl3.cpp"
    "${PROJECT_SOURCE_DIR}/imgui/misc/cpp/imgui_stdlib.cpp"
)

target_include_directories(${PROJECT_NAME} 
PUBLIC
    "${PROJECT_SOURCE_DIR}/imgui"
    "${PROJECT_SOURCE_DIR}/imgui/backends"
)

project(stb)

add_library(${PROJECT_NAME} stb.c)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})

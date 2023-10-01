project(${CGRA_PROJECT}_test_physics)

add_executable(${PROJECT_NAME}
	main.cpp
)

target_link_libraries(${PROJECT_NAME}
PRIVATE
	${CGRA_PROJECT}_aggregate
)

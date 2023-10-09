project(${CGRA_PROJECT})

add_executable(${PROJECT_NAME}
	"${PROJECT_SOURCE_DIR}/main.cpp"
)

target_link_libraries(${PROJECT_NAME}
PRIVATE
	${CGRA_PROJECT}_aggregate
)

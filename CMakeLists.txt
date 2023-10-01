################################################################
#                          Created by:                         #
#                    Ben Allen & Joshua Scott                  #
################################################################

cmake_minimum_required(VERSION 3.15)

# export compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Project Name
set(CGRA_PROJECT "infinite-driver" CACHE STRING "Infinite Driving Simulator")

# Project
project("CGRA_PROJECT_${CGRA_PROJECT}" CXX C)

# Enable IDE Project Folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "infinite-driver")


#########################################################
# Force Output Directories
# In general, this isn't a very friendly thing to do, but
# we'll do it anyway so the exes are in a reliable place.
#########################################################

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")


#########################################################
# External Libraries
#########################################################

set(PROJECT_LIB_DIR "${PROJECT_SOURCE_DIR}/lib")

# OpenGL
find_package(OpenGL REQUIRED)

# OpenMP
find_package(OpenMP)

# GLEW
add_subdirectory("${PROJECT_LIB_DIR}/glew-2.1.0/build/cmake")

# GLM
add_subdirectory("${PROJECT_LIB_DIR}/glm-0.9.9.8")

# GLFW
add_subdirectory("${PROJECT_LIB_DIR}/glfw-3.3.8")

# ImGUI
add_subdirectory("${PROJECT_LIB_DIR}/imgui")
target_link_libraries(imgui PRIVATE glfw)

# STB
add_subdirectory("${PROJECT_LIB_DIR}/stb")

# Bullet3
set(BUILD_UNIT_TESTS OFF CACHE BOOL "Build Unit Tests for Bullet3")
set(BUILD_CPU_DEMOS OFF CACHE BOOL "Build CPU Demos for Bullet3")
set(BUILD_EXTRAS OFF CACHE BOOL "Build Extras for Bullet3")
set(BUILD_BULLET3 OFF CACHE BOOL "Build Bullet3 for Bullet3")
set(BUILD_SHARED_LIBS_TEMP "${BUILD_SHARED_LIBS}")
set(BUILD_SHARED_LIBS OFF)
add_subdirectory("${PROJECT_LIB_DIR}/bullet3")
set(BUILD_SHARED_LIBS "${BUILD_SHARED_LIBS_TEMP}")
add_library(Bullet INTERFACE)
target_link_libraries(Bullet 
INTERFACE
	BulletCollision
	BulletDynamics
	BulletInverseDynamics
	BulletSoftBody
	LinearMath
)

target_include_directories(Bullet 
INTERFACE
	"${PROJECT_SOURCE_DIR}/lib/bullet3/src"
)


#########################################################
# Main project executable
#########################################################

add_executable(${CGRA_PROJECT})

target_compile_definitions(${CGRA_PROJECT} PRIVATE "-DCGRA_SRCDIR=\"${PROJECT_SOURCE_DIR}\"")


#########################################################
# set compiler flags
#########################################################

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	target_compile_options(${CGRA_PROJECT}
	PRIVATE
		/std:c++latest
		/utf-8
		/W4 					# full normal warnings
		/MP 					# multithreaded build
		/wd4800 				# disable c4800: forcing x to bool (performance warning)
	)

elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	target_compile_options(${CGRA_PROJECT}
	PRIVATE
		$<$<NOT:$<CONFIG:Debug>>:-O2>
		-std=c++20
		-Wall -Wextra -Wpedantic 	# full normal warnings
		-fvisibility=hidden 		# don't export by default
		-pthread -msse2 			# threading support, enable sse2
		-Werror=return-type 		# promote missing return to error
	)

	# enable coloured output if gcc >= 4.9
	execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
	if (GCC_VERSION VERSION_GREATER 4.9 OR GCC_VERSION VERSION_EQUAL 4.9)
		add_compile_options(-fdiagnostics-color)
	endif()

elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "^(Apple)?Clang$")
	target_compile_options(${CGRA_PROJECT}
	PRIVATE
		$<$<NOT:$<CONFIG:Debug>>:-O2>
		-std=c++20
		-Wall -Wextra -Wpedantic 	# full normal warnings
		-fvisibility=hidden 		# don't export by default
		-pthread 					# threading support
		-Werror=return-type 		# promote missing return to error
	)

	# enable msse on all processors except those that don't have it (apple silicon)
	if (NOT "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "ARM64")
		target_compile_options(${CGRA_PROJECT}
		PRIVATE
			-msse2
		)
	endif ()

endif()


#########################################################
# Link Libraries
#########################################################

target_link_libraries(${CGRA_PROJECT}
PRIVATE
	OpenGL::GL
	glew
	glm::glm
	glfw
	imgui
	stb
	Bullet
)

if (OPENMP_FOUND)
	target_compile_options(${CGRA_PROJECT}
	PRIVATE
		${OpenMP_C_FLAGS}
		${OpenMP_CXX_FLAGS}
	)
	target_compile_definitions(${CGRA_PROJECT}
	PRIVATE
		CGRA_HAVE_OPENMP
	)
endif()


#########################################################
# Include directories
#########################################################

target_include_directories(${CGRA_PROJECT}
PRIVATE
	"${PROJECT_LIB_DIR}/glew-2.1.0/include"
	"${PROJECT_SOURCE_DIR}/include"
)


#########################################################
# Source files
#########################################################
target_sources(${CGRA_PROJECT}
PRIVATE
	"${PROJECT_SOURCE_DIR}/src/application.cpp"
	"${PROJECT_SOURCE_DIR}/src/main.cpp"
	"${PROJECT_SOURCE_DIR}/src/skeleton.cpp"
	"${PROJECT_SOURCE_DIR}/src/skeleton_model.cpp"
	"${PROJECT_SOURCE_DIR}/src/cgra/cgra_geometry.cpp"
	"${PROJECT_SOURCE_DIR}/src/cgra/cgra_gui.cpp"
	"${PROJECT_SOURCE_DIR}/src/cgra/cgra_mesh.cpp"
	"${PROJECT_SOURCE_DIR}/src/cgra/cgra_shader.cpp"
	"${PROJECT_SOURCE_DIR}/src/infd/GLMesh.cpp"
	"${PROJECT_SOURCE_DIR}/src/infd/Shader.cpp"
	"${PROJECT_SOURCE_DIR}/src/infd/render/Renderer.cpp"
	"${PROJECT_SOURCE_DIR}/src/infd/render/Pipeline.cpp"
)


add_subdirectory(res) # add the resource folder to make it appear in IDE

set_property(TARGET ${CGRA_PROJECT} PROPERTY FOLDER "CGRA")

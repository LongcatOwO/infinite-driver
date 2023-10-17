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


if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
	set(BUILD_SHARED_LIBS OFF)
endif()


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
add_subdirectory("${PROJECT_LIB_DIR}/bullet3")
add_library(Bullet INTERFACE)
target_link_libraries(Bullet INTERFACE
	LinearMath
	Bullet3Common
	BulletCollision
	BulletDynamics
	BulletInverseDynamics
	BulletSoftBody
)
target_include_directories(Bullet
INTERFACE
	"${PROJECT_SOURCE_DIR}/lib/bullet3/src"
)


# Clipper2
set(CLIPPER2_EXAMPLES OFF CACHE BOOL "Build examples for Clipper2")
set(CLIPPER2_TESTS OFF CACHE BOOL "Build tests for Clipper2")
set(CLIPPER2_USINGZ OFF CACHE STRING "Build Clipper2Z")
add_subdirectory("${PROJECT_LIB_DIR}/Clipper2")


# poly2tri
set(P2T_BUILD_TESTS OFF CACHE BOOL "Build tests for poly2tri")
set(P2T_BUILD_TESTBED OFF CACHE BOOL "Build testbed for poly2tri")
add_subdirectory("${PROJECT_LIB_DIR}/poly2tri")


#########################################################
# Project Compile Settings Aggregate
#########################################################

add_library(${CGRA_PROJECT}_aggregate)

target_compile_definitions(${CGRA_PROJECT}_aggregate 
PUBLIC 
	"-DCGRA_SRCDIR=\"${PROJECT_SOURCE_DIR}\""
	"-DGLM_FORCE_SILENT_WARNINGS"
)


#########################################################
# set compiler flags
#########################################################

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	target_compile_options(${CGRA_PROJECT}_aggregate
	PUBLIC
		/std:c++latest
		/utf-8
		/W4 					# full normal warnings
		/MP 					# multithreaded build
		/wd4800 				# disable c4800: forcing x to bool (performance warning)
	)

elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	target_compile_options(${CGRA_PROJECT}_aggregate
	PUBLIC
		$<$<NOT:$<CONFIG:Debug>>:-O2>
		-std=c++20
		-Wall -Wextra -Wpedantic 	# full normal warnings
		# -fvisibility=hidden 		# don't export by default
		-pthread -msse2 			# threading support, enable sse2
		-Werror=return-type 		# promote missing return to error
	)

	# enable coloured output if gcc >= 4.9
	execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
	if (GCC_VERSION VERSION_GREATER 4.9 OR GCC_VERSION VERSION_EQUAL 4.9)
		target_compile_options(${CGRA_PROJECT}_aggregate
		PUBLIC
			-fdiagnostics-color
		)
	endif()

elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "^(Apple)?Clang$")
	target_compile_options(${CGRA_PROJECT}_aggregate
	PUBLIC
		$<$<NOT:$<CONFIG:Debug>>:-O2>
		-std=c++20
		-Wall -Wextra -Wpedantic 	# full normal warnings
		# -fvisibility=hidden 		# don't export by default
		-pthread 					# threading support
		-Werror=return-type 		# promote missing return to error
	)

	# enable msse on all processors except those that don't have it (apple silicon)
	if (NOT "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm64")
		target_compile_options(${CGRA_PROJECT}_aggregate
		PUBLIC
			-msse2
		)
	endif ()
	# on mac
	if ( "${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
		# use homebrew c++ stdlib (for std::format et al)
		target_compile_options(${CGRA_PROJECT}_aggregate
		PUBLIC
			-stdlib=libc++
			-fexperimental-library
		)
		target_link_options(${CGRA_PROJECT}_aggregate
		PUBLIC
			-L /opt/homebrew/opt/llvm/lib/c++
		)
	endif ()

endif()



#########################################################
# Link Libraries
#########################################################

target_link_libraries(${CGRA_PROJECT}_aggregate
PUBLIC
	OpenGL::GL
	glew
	glm::glm
	glfw
	imgui
	stb
	Bullet
	Clipper2
	Clipper2utils
	poly2tri
)

if (OPENMP_FOUND)
	target_compile_options(${CGRA_PROJECT}_aggregate
	PUBLIC
		${OpenMP_C_FLAGS}
		${OpenMP_CXX_FLAGS}
	)
	target_compile_definitions(${CGRA_PROJECT}_aggregate
	PUBLIC
		CGRA_HAVE_OPENMP
	)
endif()


#########################################################
# Include directories
#########################################################

target_include_directories(${CGRA_PROJECT}_aggregate
PUBLIC
	"${PROJECT_LIB_DIR}/glew-2.1.0/include"
	"${PROJECT_SOURCE_DIR}/include"
)

target_sources(${CGRA_PROJECT}_aggregate 
PRIVATE
	"${PROJECT_SOURCE_DIR}/src/cgra/cgra_geometry.cpp"
	"${PROJECT_SOURCE_DIR}/src/cgra/cgra_gui.cpp"
	"${PROJECT_SOURCE_DIR}/src/cgra/cgra_mesh.cpp"
	"${PROJECT_SOURCE_DIR}/src/cgra/cgra_shader.cpp"
	"${PROJECT_SOURCE_DIR}/src/infd/Application.cpp"
	"${PROJECT_SOURCE_DIR}/src/infd/BasicModel.cpp"
	"${PROJECT_SOURCE_DIR}/src/infd/GLMesh.cpp"
	"${PROJECT_SOURCE_DIR}/src/infd/Shader.cpp"
	"${PROJECT_SOURCE_DIR}/src/infd/render/RenderComponent.cpp"
	"${PROJECT_SOURCE_DIR}/src/infd/render/Renderer.cpp"
	"${PROJECT_SOURCE_DIR}/src/infd/render/Pipeline.cpp"
	"${PROJECT_SOURCE_DIR}/src/infd/render/Utils.cpp"
	"${PROJECT_SOURCE_DIR}/src/infd/render/Framebuffer.cpp"
	"${PROJECT_SOURCE_DIR}/src/infd/scene/Scene.cpp"
	"${PROJECT_SOURCE_DIR}/src/infd/scene/SceneObject.cpp"
	"${PROJECT_SOURCE_DIR}/src/infd/scene/Transform.cpp"
	"${PROJECT_SOURCE_DIR}/src/infd/generator/Node.cpp"
	"${PROJECT_SOURCE_DIR}/src/infd/generator/ChunkLoader.cpp"
	"${PROJECT_SOURCE_DIR}/src/infd/generator/ChunkPtr.cpp"
	"${PROJECT_SOURCE_DIR}/src/infd/generator/Edge.cpp"
	"${PROJECT_SOURCE_DIR}/src/infd/generator/ChunkGenerator.cpp"
	"${PROJECT_SOURCE_DIR}/src/infd/generator/meshbuilding/Triangle.cpp"
	"${PROJECT_SOURCE_DIR}/src/infd/generator/meshbuilding/Polygon.cpp"
	"${PROJECT_SOURCE_DIR}/src/infd/generator/meshbuilding/RoadMeshBuilder.cpp"
	"${PROJECT_SOURCE_DIR}/src/infd/scene/physics/CollisionShape.cpp"
	"${PROJECT_SOURCE_DIR}/src/infd/scene/physics/PhysicsContext.cpp"
	"${PROJECT_SOURCE_DIR}/src/infd/scene/physics/RigidBody.cpp"
)


add_subdirectory(src)
add_subdirectory(res) # add the resource folder to make it appear in IDE

set_property(TARGET ${CGRA_PROJECT} PROPERTY FOLDER "CGRA")

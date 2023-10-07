project(${CGRA_PROJECT})

add_executable(${PROJECT_NAME}
	"${PROJECT_SOURCE_DIR}/application.cpp"
	"${PROJECT_SOURCE_DIR}/main.cpp"
	"${PROJECT_SOURCE_DIR}/skeleton.cpp"
	"${PROJECT_SOURCE_DIR}/skeleton_model.cpp"
	"${PROJECT_SOURCE_DIR}/cgra/cgra_geometry.cpp"
	"${PROJECT_SOURCE_DIR}/cgra/cgra_gui.cpp"
	"${PROJECT_SOURCE_DIR}/cgra/cgra_mesh.cpp"
	"${PROJECT_SOURCE_DIR}/cgra/cgra_shader.cpp"
	"${PROJECT_SOURCE_DIR}/infd/GLMesh.cpp"
	"${PROJECT_SOURCE_DIR}/infd/Shader.cpp"
	"${PROJECT_SOURCE_DIR}/infd/render/Renderer.cpp"
	"${PROJECT_SOURCE_DIR}/infd/render/Pipeline.cpp"
	"${PROJECT_SOURCE_DIR}/infd/render/Utils.cpp"
	"${PROJECT_SOURCE_DIR}/infd/render/Framebuffer.cpp"
	"${PROJECT_SOURCE_DIR}/infd/scene/PhysicsContext.cpp"
	"${PROJECT_SOURCE_DIR}/infd/scene/Scene.cpp"
	"${PROJECT_SOURCE_DIR}/infd/scene/SceneObject.cpp"
	"${PROJECT_SOURCE_DIR}/infd/scene/Transform.cpp"
)

target_link_libraries(${PROJECT_NAME}
PRIVATE
	${CGRA_PROJECT}_aggregate
)

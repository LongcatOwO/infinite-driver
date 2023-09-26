
#pragma once

// glm
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// project
#include <opengl.hpp>
#include <infd/GLMesh.hpp>
#include <infd/GLObject.hpp>
#include <infd/render/Renderer.hpp>
//#include <skeleton_model.hpp>


// Basic model that holds the shader, mesh and transform for drawing.
// Can be copied and modified for adding in extra information for drawing
// including textures for texture mapping etc.
struct basic_model {
	infd::GLProgram shader;
	infd::GLMesh mesh;
	glm::vec3 color{0.7};
	glm::mat4 modelTransform{1.0};
	GLuint texture = 0;

    explicit basic_model(infd::GLMesh _mesh) : mesh{_mesh} {}

	void draw(const glm::mat4 &view, const glm::mat4 proj);
};


// Main application class
//
class Application {
private:
	// window
	glm::vec2 m_windowsize;
	GLFWwindow *m_window;

	// oribital camera
	float m_pitch = .86;
	float m_yaw = -.86;
	float m_distance = 20;

	// last input
	bool m_leftMouseDown = false;
	glm::vec2 m_mousePosition;

	// drawing flags
	bool m_show_axis = false;
	bool m_show_grid = false;
	bool m_showWireframe = false;
    bool _use_render_pipeline = true;

	// geometry
	basic_model m_model;
    infd::render::RenderSettings _render_settings;
    infd::render::Renderer _renderer;

public:
	// setup
	Application(GLFWwindow *);

	// disable copy constructors (for safety)
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;

	// rendering callbacks (every frame)
	void render();
	void renderGUI();

	// input callbacks
	void cursorPosCallback(double xpos, double ypos);
	void mouseButtonCallback(int button, int action, int mods);
	void scrollCallback(double xoffset, double yoffset);
	void keyCallback(int key, int scancode, int action, int mods);
	void charCallback(unsigned int c);
};

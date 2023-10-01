
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
#include <infd/util/Event.hpp>


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
public:

private:
	// typedefs
	template <typename FunctionSignature>
	using Event = infd::util::Event<FunctionSignature>;

	template <typename FunctionSignature>
	using PublicEvent = infd::util::PublicEvent<FunctionSignature>;

	// window
	glm::vec2 m_windowsize;
	GLFWwindow *m_window;

	// input callback events
	Event<void (double x_pos, double y_pos)> _cursorPosCallbackEvent;
	Event<void (int button, int action, int mods)> _mouseButtonCallbackEvent;
	Event<void (double x_offset, double y_offset)> _scrollCallbackEvent;
	Event<void (int key, int scancode, int action, int mods)> _keyCallbackEvent;
	Event<void (unsigned int c)> _charCallbackEvent;

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

	void initializeInputCallbacks();

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
	void cursorPosCallback(double x_pos, double y_pos) {
		_cursorPosCallbackEvent(x_pos, y_pos);
	}
	void mouseButtonCallback(int button, int action, int mods) {
		_mouseButtonCallbackEvent(button, action, mods);
	}

	void scrollCallback(double x_offset, double y_offset) {
		_scrollCallbackEvent(x_offset, y_offset);
	}

	void keyCallback(int key, int scancode, int action, int mods) {
		_keyCallbackEvent(key, scancode, action, mods);
	}

	void charCallback(unsigned int c) {
		_charCallbackEvent(c);
	}

	PublicEvent<void (double x_pos, double y_pos)>& cursorPosCallbackEvent() {
		return _cursorPosCallbackEvent;
	}

	PublicEvent<void (int button, int action, int mods)>& mouseButtonCallbackEvent() {
		return _mouseButtonCallbackEvent;
	}

	PublicEvent<void (double x_offset, double y_offset)>& scrollCallbackEvent() {
		return _scrollCallbackEvent;
	}

	PublicEvent<void (int key, int scancode, int action, int mods)>& keyCallbackEvent() {
		return _keyCallbackEvent;
	}

	PublicEvent<void (unsigned int c)>& charCallbackEvent() {
		return _charCallbackEvent;
	}
};

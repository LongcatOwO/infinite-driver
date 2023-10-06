// std
#include <string>
#include <utility>

// glm
#include <glm/vec2.hpp>

// project
#include <infd/Application.hpp>


namespace infd {

	void Application::internalCursorPosCallback(double x_pos, double y_pos) {
		// TODO
	}

	void Application::internalMouseButtonCallback(int button, int action, int mods) {
		// TODO
	}

	void Application::internalScrollCallback(double x_offset, double y_offset) {
		// TODO
	}

	void Application::internalKeyCallback(int key, int scancode, int action, int mods) {
		// TODO
	}

	void Application::internalCharCallback(unsigned int c) {
		// TODO
	}

	Application::Application(GLFWWindowPtr window, std::string name) :
		_window(std::move(window)),
		_scene(std::move(name))
	{
		// TODO
	}

	void Application::preLoopInit() {
		// TODO
	}

	void Application::update() {
		// TODO
	}

} // namespace infd

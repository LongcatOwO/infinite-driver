#pragma once

// std
#include <memory>
#include <string>

// glm
#include <glm/vec2.hpp>

// project
#include <opengl.hpp>

// project - util
#include <infd/util/Event.hpp>

// project - scene
#include <infd/scene/Scene.hpp>


namespace infd {

	class GLFWWindowDeleter {
	public:
		void operator()(GLFWwindow *window) const noexcept {
			glfwDestroyWindow(window);
		}
	};

	using GLFWWindowPtr = std::unique_ptr<GLFWwindow, GLFWWindowDeleter>;

	class Application {
		// window
		glm::vec2 _window_size;
		GLFWWindowPtr _window;

		// input callback events
		util::Event<void (double x_pos, double y_pos)> _cursor_pos_event;
		util::Event<void (int button, int action, int mods)> _mouse_button_event;
		util::Event<void (double x_offset, double y_offset)> _scroll_event;
		util::Event<void (int key, int scancode, int action, int mods)> _key_event;
		util::Event<void (unsigned int c)> _char_event;

		bool _left_mouse_down = false;
		glm::vec2 _mouse_position;

		void internalCursorPosCallback(double x_pos, double y_pos);
		void internalMouseButtonCallback(int button, int action, int mods);
		void internalScrollCallback(double x_offset, double y_offset);
		void internalKeyCallback(int key, int scancode, int action, int mods);
		void internalCharCallback(unsigned int c);

		// scene
		infd::scene::Scene _scene;

	public:
		[[nodiscard]] Application(GLFWWindowPtr window, std::string name);

		// disable copy constructors (for safety)
		Application(const Application &) = delete;
		Application& operator=(const Application &) = delete;

		void preLoopInit();
		void update();

		[[nodiscard]] GLFWwindow* window() noexcept {
			return _window.get();
		}

		// ----------input callbacks----------
		void cursorPosCallback(double x_pos, double y_pos) {
			internalCursorPosCallback(x_pos, y_pos);
			_cursor_pos_event(x_pos, y_pos);
		}

		void mouseButtonCallback(int button, int action, int mods) {
			internalMouseButtonCallback(button, action, mods);
			_mouse_button_event(button, action, mods);
		}

		void scrollCallback(double x_offset, double y_offset) {
			internalScrollCallback(x_offset, y_offset);
			_scroll_event(x_offset, y_offset);
		}

		void keyCallback(int key, int scancode, int action, int mods) {
			internalKeyCallback(key, scancode, action, mods);
			_key_event(key, scancode, action, mods);
		}

		void charCallback(unsigned int c) {
			internalCharCallback(c);
			_char_event(c);
		}

		[[nodiscard]] util::PublicEvent<void (double x_pos, double y_pos)>& cursorPosEvent() {
			return _cursor_pos_event;
		}

		[[nodiscard]] util::PublicEvent<void (int button, int action, int mods)>& mouseButtonEvent() {
			return _mouse_button_event;
		}

		[[nodiscard]] util::PublicEvent<void (double x_offset, double y_offset)>& scrollEvent() {
			return _scroll_event;
		}

		[[nodiscard]] util::PublicEvent<void (int key, int scancode, int action, int mods)>& keyEvent() {
			return _key_event;
		}

		[[nodiscard]] util::PublicEvent<void (unsigned int c)>& charEvent() {
			return _char_event;
		}
	};

} // namespace infd

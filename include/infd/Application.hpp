#pragma once

// std
#include <memory>
#include <string>

// glm
#include <glm/vec2.hpp>

// project
#include <opengl.hpp>

// project - infd
#include <infd/BasicModel.hpp>
#include <infd/render/Renderer.hpp>
#include <infd/util/Event.hpp>
#include <infd/scene/Scene.hpp>
#include <infd/render/RenderComponent.hpp>
#include "Wavefront.hpp"
#include "infd/generator/ChunkLoader.hpp"


namespace infd {

	class GLFWWindowDeleter {
	public:
		void operator()(GLFWwindow *window) const noexcept {
			glfwDestroyWindow(window);
		}
	};

	using GLFWWindowPtr = std::unique_ptr<GLFWwindow, GLFWWindowDeleter>;

	class Application {
        constexpr static const float WORLD_SCALE = 300.f;

		// window
		GLFWWindowPtr _window;
		glm::vec2 _window_size;

		scene::Scene _scene;
        infd::generator::ChunkLoader* _chunk_loader;

		// orbital camera
		float _pitch = .86;
		float _yaw = -.86;
		float _distance = 20;

		// last input
		bool _left_mouse_down = false;
		glm::vec2 _mouse_position;

		// drawing flags
		bool _show_axis = false;
		bool _show_grid = false;
		bool _show_wireframe = false;
		bool _use_render_pipeline = true;

		// geometry
		BasicModel _model;
		infd::render::RenderSettings _render_settings;
		infd::render::Renderer _renderer;

		// ----------input callback events----------

		util::Event<void (double x_pos, double y_pos)> _cursor_pos_event;
		util::Event<void (int button, int action, int mods)> _mouse_button_event;
		util::Event<void (double x_offset, double y_offset)> _scroll_event;
		util::Event<void (int key, int scancode, int action, int mods)> _key_event;
		util::Event<void (unsigned int c)> _char_event;

		// implement rendering here
		void internalRender();
		// implement GUI rendering here
		void internalRenderGUI();

		// this calls internalRender, and internalRenderGUI
		// with other setups also.
		void internalDoRender(scene::Scene &);

		void internalCursorPosCallback(double x_pos, double y_pos);
		void internalMouseButtonCallback(int button, int action, int mods);
		void internalScrollCallback(double x_offset, double y_offset);
		void internalKeyCallback(int key, int scancode, int action, int mods);
		void internalCharCallback(unsigned int c);

	public:
		[[nodiscard]] Application(GLFWWindowPtr window, std::string name);

		// disable copy constructors (for safety)
		Application(const Application &) = delete;
		Application& operator=(const Application &) = delete;

		void run();

		[[nodiscard]] GLFWwindow* window() noexcept;
		[[nodiscard]] scene::Scene& scene() noexcept;

		// ----------input callbacks----------

		void cursorPosCallback(double x_pos, double y_pos);
		void mouseButtonCallback(int button, int action, int mods);
		void scrollCallback(double x_offset, double y_offset);
		void keyCallback(int key, int scancode, int action, int mods);
		void charCallback(unsigned int c);


		// ----------input callback events----------

		[[nodiscard]] util::PublicEvent<void (double x_pos, double y_pos)>& cursorPosEvent() noexcept;
		[[nodiscard]] util::PublicEvent<void (int button, int action, int mods)>& mouseButtonEvent() noexcept;
		[[nodiscard]] util::PublicEvent<void (double x_offset, double y_offset)>& scrollEvent() noexcept;
		[[nodiscard]] util::PublicEvent<void (int key, int scancode, int action, int mods)>& keyEvent() noexcept;
		[[nodiscard]] util::PublicEvent<void (unsigned int c)>& charEvent() noexcept;
	};


	// ----------inline implementations----------

	inline void Application::run() {
		_scene.awake();
		_scene.enableTime();
		while(!glfwWindowShouldClose(window())) {
			_scene.updateTime();
		}
	}

	inline GLFWwindow* Application::window() noexcept {
		return _window.get();
	}

	inline scene::Scene& Application::scene() noexcept {
		return _scene;
	}

	inline void Application::cursorPosCallback(double x_pos, double y_pos) {
		_cursor_pos_event(x_pos, y_pos);
		scene().cursorPosCallback(x_pos, y_pos);
	}

	inline void Application::mouseButtonCallback(int button, int action, int mods) {
		_mouse_button_event(button, action, mods);
		scene().mouseButtonCallback(button, action, mods);
	}

	inline void Application::scrollCallback(double x_offset, double y_offset) {
		_scroll_event(x_offset, y_offset);
		scene().scrollCallback(x_offset, y_offset);
	}

	inline void Application::keyCallback(int key, int scancode, int action, int mods) {
		_key_event(key, scancode, action, mods);
		scene().keyCallback(key, scancode, action, mods);
	}

	inline void Application::charCallback(unsigned int c) {
		_char_event(c);
		scene().charCallback(c);
	}

	inline util::PublicEvent<void (double x_pos, double y_pos)>& Application::cursorPosEvent() noexcept {
		return _cursor_pos_event;
	}

	inline util::PublicEvent<void (int button, int action, int mods)>& Application::mouseButtonEvent() noexcept {
		return _mouse_button_event;
	}

	inline util::PublicEvent<void (double x_offset, double y_offset)>& Application::scrollEvent() noexcept {
		return _scroll_event;
	}

	inline util::PublicEvent<void (int key, int scancode, int action, int mods)>& Application::keyEvent() noexcept {
		return _key_event;
	}

	inline util::PublicEvent<void (unsigned int c)>& Application::charEvent() noexcept {
		return _char_event;
	}

} // namespace infd

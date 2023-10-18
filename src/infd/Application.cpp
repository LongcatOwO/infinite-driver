// std
#include <iostream>
#include <string>
#include <utility>

// glm
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

// project
#include <opengl.hpp>

// project - cgra
#include <cgra/cgra_geometry.hpp>
#include <cgra/cgra_gui.hpp>
#include <cgra/cgra_image.hpp>

// project - infd
#include <infd/Application.hpp>
#include <infd/debug/geometry.hpp>
#include <infd/debug/glm.hpp>
#include <infd/Shader.hpp>
#include <infd/Wavefront.hpp>
#include <infd/util/Function.hpp>
#include <infd/util/StaticCastOutPtr.hpp>
#include <infd/scene/KeyboardInputRigidBodyController.hpp>
#include <infd/scene/Scene.hpp>
#include <infd/scene/physics/BoxShape.hpp>
#include <infd/scene/physics/physics.hpp>
#include <infd/scene/physics/StaticPlaneShape.hpp>
#include <infd/scene/physics/BvhTriangleMeshShape.hpp>
#include "infd/generator/ChunkLoader.hpp"


namespace infd {
	void Application::internalCursorPosCallback(double x_pos, double y_pos) {
		if (_left_mouse_down) {
			glm::vec2 half_window_size = _window_size / 2.f;

			// clamp the pitch to [-pi/2, pi/2]
			glm::vec2 mouse_pos_dif_ratio = (glm::vec2{x_pos, y_pos} - _mouse_position)
				/ half_window_size;


			_pitch += mouse_pos_dif_ratio.y * glm::half_pi<float>();
			_pitch = glm::clamp(_pitch, -glm::half_pi<float>(), glm::half_pi<float>());

			_yaw += mouse_pos_dif_ratio.x * glm::pi<float>();

			if (_yaw > glm::pi<float>()) 
				_yaw -= glm::two_pi<float>();
			else if (_yaw < -glm::pi<float>())
				_yaw += glm::two_pi<float>();
		}

		// update mouse position
		_mouse_position = {x_pos, y_pos};
	}

	void Application::internalMouseButtonCallback(int button, int action, int mods) {
		(void) mods;

		// capture is left-mouse down
		if (button == GLFW_MOUSE_BUTTON_LEFT)
			_left_mouse_down = (action == GLFW_PRESS);
	}

	void Application::internalScrollCallback(double x_offset, double y_offset) {
		(void) x_offset;

		_distance *= glm::pow(1.1f, -y_offset);
	}

	void Application::internalKeyCallback(int key, int scancode, int action, int mods) {
		(void) scancode, (void) mods;

		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window(), GLFW_TRUE);
		}
	}

	void Application::internalCharCallback(unsigned int c) {
		(void) c;
	}

	Application::Application(GLFWWindowPtr window, std::string name) :
		_window{std::move(window)},
		_scene{std::move(name)},
		_model{infd::loadWavefrontCases(CGRA_SRCDIR "//res//assets//teapot.obj").build()}
	{
		_cursor_pos_event 	+= util::BindedMemberFunc{&Application::internalCursorPosCallback, 	 *this};
		_mouse_button_event += util::BindedMemberFunc{&Application::internalMouseButtonCallback, *this};
		_scroll_event 		+= util::BindedMemberFunc{&Application::internalScrollCallback, 	 *this};
		_key_event 			+= util::BindedMemberFunc{&Application::internalKeyCallback, 		 *this};
		_char_event 		+= util::BindedMemberFunc{&Application::internalCharCallback, 		 *this};

		_scene.onFrameRender() += util::BindedMemberFunc{&Application::internalDoRender, *this};

		infd::ShaderBuilder sb;
		sb.setShader(GL_VERTEX_SHADER, CGRA_SRCDIR "//res//shaders//color_vert.glsl");
		sb.setShader(GL_FRAGMENT_SHADER, CGRA_SRCDIR "//res//shaders//color_frag.glsl");

		_model.shader = sb.build();
		_model.color = glm::vec3{1, 0, 0};

		glfwGetFramebufferSize(
			this->window(), 
			&_render_settings.screen_size.x, 
			&_render_settings.screen_size.y
		);
		_renderer.setRenderSettings(_render_settings);

		_scene.addSceneObject(std::make_unique<scene::SceneObject>("Physics Context"));

        // TODO: REMOVE
        // Create Demo Objects:
		scene::SceneObject& physicsContext = _scene.addSceneObject(std::make_unique<scene::SceneObject>("Physics Context"));
		physicsContext.emplaceComponent<scene::physics::PhysicsContext>();

        // scene::SceneObject& plane = _scene.addSceneObject(std::make_unique<scene::SceneObject>("Plane"));
		// plane.transform().localPosition({0, 0, 0});
        // plane.transform().localScale(glm::vec3(5,0.01,5));
        // plane.addComponent(
        //         std::make_unique<render::RenderComponent>(
        //                 _renderer,
        //                 infd::loadWavefrontCases(CGRA_SRCDIR + std::string("/res//assets//plane.obj")).build()
        //         )
        // );

		// plane.emplaceComponent<scene::physics::StaticPlaneShape>(glm::vec3(0, 1, 0), 0);
		// plane.emplaceComponent<scene::physics::RigidBody>().mass(0);

        scene::SceneObject& teapot = _scene.addSceneObject(std::make_unique<scene::SceneObject>("Teapot"));
		teapot.transform().localPosition({-5, 30, 2});
		teapot.emplaceComponent<render::RenderComponent>(
			_renderer, 
			infd::loadWavefrontCases(CGRA_SRCDIR "//res//assets//teapot.obj").build()
		);
        teapot.getComponent<render::RenderComponent>()->material.colour = {47/255.f, 196/255.f, 94/255.f};
		{
			auto& box_shape = teapot.emplaceComponent<scene::physics::BoxShape>();
			box_shape.halfSize({3, 3, 3});
			box_shape.createOutlineMesh(_renderer);
		}
        teapot.emplaceComponent<scene::physics::RigidBody>();
		teapot.emplaceComponent<scene::KeyboardInputRigidBodyController>();

        scene::SceneObject& bunny = _scene.addSceneObject(std::make_unique<scene::SceneObject>("Bunny"));
		bunny.transform().localPosition({8, 30, -1.5});
		scene::SceneObject& bunny_mesh = bunny.addChild("Bunny Mesh");
		bunny_mesh.emplaceComponent<render::RenderComponent>(
			_renderer,
			infd::loadWavefrontCases(CGRA_SRCDIR "//res//assets//bunny.obj").build()
		);
		bunny_mesh.transform().localScale(glm::vec3{75});
        bunny_mesh.getComponent<render::RenderComponent>()->material.colour = {232/255.f, 205/255.f, 136/255.f};
		{
			auto& box_shape = bunny.emplaceComponent<scene::physics::BoxShape>();
			box_shape.halfSize({3, 3, 3});
			box_shape.createOutlineMesh(_renderer);
		}
        bunny.emplaceComponent<scene::physics::RigidBody>();

        scene::SceneObject& chunkLoader = _scene.addSceneObject(std::make_unique<scene::SceneObject>("ChunkLoader"));
        auto& loader = chunkLoader.emplaceComponent<generator::ChunkLoader>(chunkLoader, _renderer, 5);
        loader.move(-6,-6);
        loader.transform().localScale(glm::vec3(30));

        scene::SceneObject& light = _scene.addSceneObject(std::make_unique<scene::SceneObject>("Light"));
        light.emplaceComponent<render::DirectionalLightComponent>();

        scene::SceneObject& camera = _scene.addSceneObject(std::make_unique<scene::SceneObject>("camera"));
        camera.transform().localPosition({0, 15, 30});
        camera.emplaceComponent<render::CameraComponent>();
	}

	void Application::internalDoRender(scene::Scene &) {
		// main rendering
		// glEnable(GL_FRAMEBUFFER_SRGB); // use if you know about gamma correction
		internalRender();

		// GUI rendering on top
		// glDisable(GL_FRAMEBUFFER_SRGB); // use if you know about gamme correction
		cgra::gui::newFrame();
		internalRenderGUI();
		cgra::gui::render();

		// swap front and back buffers
		glfwSwapBuffers(window());

		// poll for and process events
		glfwPollEvents();
	}

	void Application::internalRender() {
		// retrieve the window height
		glfwGetFramebufferSize(
			window(),
			util::staticCastOutPtr<int>(&_window_size.x),
			util::staticCastOutPtr<int>(&_window_size.y)
		);

		// projection matrix
		glm::mat4 proj = glm::perspective(1.f, _window_size.x / _window_size.y, 0.1f, 1000.f);

		// view matrix
		glm::mat4 view = 
			glm::rotate(
			glm::rotate(
			glm::translate(glm::mat4{1},
				glm::vec3{0, 0, -_distance}),
				_pitch, glm::vec3{1, 0, 0}),
				_yaw, glm::vec3{0, 1, 0});

		// draw the model
		if (_use_render_pipeline) {
			_render_settings.screen_size = _window_size;
			_renderer.setRenderSettings(_render_settings);
			_renderer.render();
		} else {
			// set the viewport to draw to the entire window
			glViewport(0, 0, _window_size.x, _window_size.y);

			// clear the back-buffer
			glClearColor(0.3f, 0.3f, 0.4f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// enable flags for normal/forward rendering
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);

			if (_show_grid) cgra::drawGrid(view, proj);
			if (_show_axis) cgra::drawAxis(view, proj);
			glPolygonMode(GL_FRONT_AND_BACK, _show_wireframe ? GL_LINE : GL_FILL);

			_model.draw(view, proj);
		}
	}

	void Application::internalRenderGUI() {
		// setup the window
		ImGui::SetNextWindowPos(ImVec2{5, 5}, ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2{300, 420}, ImGuiCond_Once);
		ImGui::Begin("Options", nullptr);

		// display current camera parameters
		ImGui::Text("Application %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::SliderFloat("Pitch", &_pitch, -glm::half_pi<float>(), glm::half_pi<float>(), "%.2f");
		ImGui::SliderFloat("Yaw", &_yaw, -glm::pi<float>(), glm::pi<float>(), "%.2f");
		ImGui::SliderFloat("Distance", &_distance, 0, 100, "%.2f", ImGuiSliderFlags_Logarithmic);

		// helpful drawing options
		ImGui::Checkbox("Show axis", &_show_axis);
		ImGui::SameLine();
		ImGui::Checkbox("Show grid", &_show_grid);
		ImGui::Checkbox("Wireframe", &_show_wireframe);
		ImGui::SameLine();
		if (ImGui::Button("Screenshot")) cgra::rgba_image::screenshot(true);
		ImGui::Checkbox("Use render pipeline", &_use_render_pipeline);

		ImGui::Separator();

		_renderer.gui();
        ImGui::Checkbox("Dither with colour", &_render_settings.dither_colour);
		ImGui::Checkbox("Render un-dithered scene", &_render_settings.render_original);
        ImGui::Checkbox("Render wireframe", &_render_settings.render_wireframe);

		// finish creating window
		ImGui::End();
	}

} // namespace infd

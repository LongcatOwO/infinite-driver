// std
#include <iostream>
#include <memory>
#include <string>
#include <utility>

// bullet
#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletCollision/CollisionShapes/btCapsuleShape.h>

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
#include <infd/scene/FollowTransform.hpp>
#include <infd/scene/KeyboardInputRigidBodyController.hpp>
#include <infd/scene/LookAtParent.hpp>
#include <infd/scene/Scene.hpp>
#include <infd/scene/physics/BoxShape.hpp>
#include <infd/scene/physics/BvhTriangleMeshShape.hpp>
#include <infd/scene/physics/CompoundShape.hpp>
#include <infd/scene/physics/physics.hpp>
#include <infd/scene/physics/StaticPlaneShape.hpp>
#include "infd/generator/ChunkLoader.hpp"


namespace {
	std::unique_ptr<infd::scene::SceneObject> createCar(infd::render::Renderer& renderer) {
		using namespace ::infd::debug;
		using namespace ::infd::render;
		using namespace ::infd::scene;
		using namespace ::infd::scene::physics;

		/*
		 * 0.5H 2L 1W
		 *
		 * 0.5H = 0.25R * 2 + 0.5L
		 */
		std::unique_ptr<SceneObject> car = std::make_unique<SceneObject>("Car");
		SceneObject& car_body = car->addChild("Body");
		car_body.transform().localScale({1.f, 0.5f, 2.f});
		car_body.emplaceComponent<RenderComponent>(renderer, generateBoxMesh())
			.material.colour = {0.8f, 0.8f, 0.f};
		SceneObject& car_front_wheel = car->addChild("Front Wheel");
		car_front_wheel.transform().localRotation(glm::angleAxis(glm::half_pi<Float>(), glm::vec3{0, 0, 1}));
		car_front_wheel.transform().localPosition({0.f, -1.f, -1.5f});
		car_front_wheel.emplaceComponent<RenderComponent>(renderer, generateCapsuleMesh(0.5f, 1.f))
			.material.colour = {0, 1, 0};

		SceneObject& car_back_wheel = car->addChild("Back Wheel");
		car_back_wheel.transform().localRotation(glm::angleAxis(glm::half_pi<Float>(), glm::vec3{0, 0, 1}));
		car_back_wheel.transform().localPosition({0.f, -1.f, 1.5f});
		car_back_wheel.emplaceComponent<RenderComponent>(renderer, generateCapsuleMesh(0.5f, 1.f))
			.material.colour = {1, 0, 0};

		CompoundShape& collision_shape = car->emplaceComponent<CompoundShape>();
		// body collision shape
		collision_shape.addChildShape(std::make_unique<btBoxShape>(btVector3{1.f, 0.5f, 2.f}));
		// front wheel collision shape
		collision_shape.addChildShape(
			std::make_unique<btCapsuleShape>(0.5f, 1.f),
			{0.f, -1.f, -1.5f},
			glm::angleAxis(glm::half_pi<Float>(), glm::vec3{0, 0, 1})
		);
		// back wheel collision shape
		collision_shape.addChildShape(
			std::make_unique<btCapsuleShape>(0.5f, 1.f),
			{0.f, -1.f, 1.5f},
			glm::angleAxis(glm::half_pi<Float>(), glm::vec3{0, 0, 1})
		);
		RigidBody& rigid_body = car->emplaceComponent<RigidBody>();
		rigid_body.angularDamping(0.1);
		rigid_body.centerOfMassPosition({0, -1.f, 0});

		return car;
	}
}

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

		scene::SceneObject& physicsContext = _scene.addSceneObject(std::make_unique<scene::SceneObject>("Physics Context"));
		physicsContext.emplaceComponent<scene::physics::PhysicsContext>();

		scene::SceneObject& car = _scene.addSceneObject(createCar(_renderer));
		car.transform().localPosition({-5, 30, 2});
		car.emplaceComponent<scene::KeyboardInputRigidBodyController>();
		car.emplaceComponent<render::DirectionalLightComponent>();
		
		scene::SceneObject& camera_target =
			_scene.addSceneObject(std::make_unique<scene::SceneObject>("Camera Target"));
		camera_target.emplaceComponent<scene::FollowTransform>().toFollow(&car.transform());

		scene::SceneObject& camera = camera_target.addChild("Camera");
		camera.emplaceComponent<render::CameraComponent>();
		camera.emplaceComponent<render::DitherSettingsComponent>();
		camera.emplaceComponent<scene::LookAtParent>();
        scene::SceneObject& chunkLoader = _scene.addSceneObject(std::make_unique<scene::SceneObject>("ChunkLoader"));
        auto& loader = chunkLoader.emplaceComponent<generator::ChunkLoader>(chunkLoader, _renderer, 2);
        loader.transform().localScale(glm::vec3(WORLD_SCALE));

        _chunk_loader = &loader;

        // scene::SceneObject& light = _scene.addSceneObject(std::make_unique<scene::SceneObject>("Light"));
        // light.emplaceComponent<render::DirectionalLightComponent>();
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
		ImGui::SetNextWindowSize(ImVec2{300, 480}, ImGuiCond_Once);
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
		ImGui::Checkbox("Render un-dithered scene", &_render_settings.render_original);
        ImGui::Checkbox("Render wireframe", &_render_settings.render_wireframe);

		// finish creating window
		ImGui::End();
	}

} // namespace infd

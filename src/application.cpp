
// std
#include <iostream>
#include <string>
#include <chrono>

// glm
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>


// project
#include <application.hpp>
#include <cgra/cgra_geometry.hpp>
#include <cgra/cgra_gui.hpp>
#include <cgra/cgra_image.hpp>
#include <infd/Wavefront.hpp>
#include <infd/Shader.hpp>
#include <misc/cpp/imgui_stdlib.h>

#include <infd/scene/SceneObject.hpp>


using namespace std;
using namespace cgra;
using namespace glm;


void basic_model::draw(const glm::mat4 &view, const glm::mat4 proj) {
	mat4 modelview = view * modelTransform;
	
	glUseProgram(shader); // load shader and variables
	glUniformMatrix4fv(glGetUniformLocation(shader, "uProjectionMatrix"), 1, false, value_ptr(proj));
	glUniformMatrix4fv(glGetUniformLocation(shader, "uModelViewMatrix"), 1, false, value_ptr(modelview));
	glUniform3fv(glGetUniformLocation(shader, "uColor"), 1, value_ptr(color));

	mesh.draw(); // draw
}


Application::Application(GLFWwindow *window) : m_window(window), m_model {infd::loadWavefrontCases(CGRA_SRCDIR + std::string("/res//assets//teapot.obj")).build()} {

	initializeInputCallbacks();
	
	infd::ShaderBuilder sb;
    sb.setShader(GL_VERTEX_SHADER, CGRA_SRCDIR + std::string("//res//shaders//color_vert.glsl"));
	sb.setShader(GL_FRAGMENT_SHADER, CGRA_SRCDIR + std::string("//res//shaders//color_frag.glsl"));

	m_model.shader = sb.build();
	m_model.color = vec3(1, 0, 0);
}


void Application::render() {
	
	// retrieve the window hieght
	int width, height;
	glfwGetFramebufferSize(m_window, &width, &height); 

	m_windowsize = vec2(width, height); // update window size
	glViewport(0, 0, width, height); // set the viewport to draw to the entire window

	// clear the back-buffer
	glClearColor(0.3f, 0.3f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

	// enable flags for normal/forward rendering
	glEnable(GL_DEPTH_TEST); 
	glDepthFunc(GL_LESS);

	// projection matrix
	mat4 proj = perspective(1.f, float(width) / height, 0.1f, 1000.f);

	// view matrix
	mat4 view = translate(mat4(1), vec3(0, 0, -m_distance))
		* rotate(mat4(1), m_pitch, vec3(1, 0, 0))
		* rotate(mat4(1), m_yaw,   vec3(0, 1, 0));


	// helpful draw options
	if (m_show_grid) drawGrid(view, proj);
	if (m_show_axis) drawAxis(view, proj);
	glPolygonMode(GL_FRONT_AND_BACK, (m_showWireframe) ? GL_LINE : GL_FILL);


	// draw the model
    if (_use_render_pipeline) {
        _render_settings.screen = {width, height};
        _render_settings.temp_view = view;
        _render_settings.temp_proj = proj;
        _renderer.setRenderSettings(_render_settings);
        _renderer.render();
    } else {
        m_model.draw(view, proj);
    }
}


void Application::renderGUI() {

	// setup window
	ImGui::SetNextWindowPos(ImVec2(5, 5), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_Once);
	ImGui::Begin("Options", 0);

	// display current camera parameters
	ImGui::Text("Application %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::SliderFloat("Pitch", &m_pitch, -pi<float>() / 2, pi<float>() / 2, "%.2f");
	ImGui::SliderFloat("Yaw", &m_yaw, -pi<float>(), pi<float>(), "%.2f");
	ImGui::SliderFloat("Distance", &m_distance, 0, 100, "%.2f", ImGuiSliderFlags_Logarithmic);

	// helpful drawing options
	ImGui::Checkbox("Show axis", &m_show_axis);
	ImGui::SameLine();
	ImGui::Checkbox("Show grid", &m_show_grid);
	ImGui::Checkbox("Wireframe", &m_showWireframe);
	ImGui::SameLine();
	if (ImGui::Button("Screenshot")) rgba_image::screenshot(true);
    ImGui::Checkbox("Use render pipeline", &_use_render_pipeline);

	
	ImGui::Separator();

	// example of how to use input boxes
	std::string exampleInput;
	if (ImGui::InputText("example input", &exampleInput)) {
		cout << "example input changed to " << exampleInput << endl;
	}

	// finish creating window
	ImGui::End();
}


void Application::initializeInputCallbacks() {
	_cursorPosCallbackEvent += [this](double x_pos, double y_pos) {
		if (m_leftMouseDown) {
			vec2 whsize = m_windowsize / 2.0f;

			// clamp the pitch to [-pi/2, pi/2]
			m_pitch += float(acos(glm::clamp((m_mousePosition.y - whsize.y) / whsize.y, -1.0f, 1.0f))
				- acos(glm::clamp((float(y_pos) - whsize.y) / whsize.y, -1.0f, 1.0f)));
			m_pitch = float(glm::clamp(m_pitch, -pi<float>() / 2, pi<float>() / 2));

			// wrap the yaw to [-pi, pi]
			m_yaw += float(acos(glm::clamp((m_mousePosition.x - whsize.x) / whsize.x, -1.0f, 1.0f))
				- acos(glm::clamp((float(x_pos) - whsize.x) / whsize.x, -1.0f, 1.0f)));
			if (m_yaw > pi<float>()) m_yaw -= float(2 * pi<float>());
			else if (m_yaw < -pi<float>()) m_yaw += float(2 * pi<float>());
		}

		// updated mouse position
		m_mousePosition = vec2(x_pos, y_pos);
	};

	_mouseButtonCallbackEvent += [this](int button, int action, int mods) {
		(void)mods;
		// capture is left-mouse down
		if (button == GLFW_MOUSE_BUTTON_LEFT)
			m_leftMouseDown = (action == GLFW_PRESS); // only other option is GLFW_RELEASE
	};

	_scrollCallbackEvent += [this](double x_offset, double y_offset) {
		(void)x_offset;

		m_distance *= pow(1.1f, -y_offset);
	};

	_keyCallbackEvent += [](int key, int scancode, int action, int mods) {
		(void)scancode, (void)mods;

		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			std::exit(0);
		}
	};

	// _charCallbackEvent += [this](unsigned int c) {};
}

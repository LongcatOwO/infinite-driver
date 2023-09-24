#pragma once

// imgui
#include <imgui.h>

// project
#include <opengl.hpp>


namespace cgra::gui {
	// helper functions to setup, run and shutdown ImGui
	bool init(GLFWwindow* window, bool install_callbacks=false);
	void newFrame();
	void render();
	void shutdown();
}

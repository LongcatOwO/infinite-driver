#include <iostream>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

// project
#include <cgra/cgra_gui.hpp>

using namespace std;


namespace cgra::gui {

        bool init(GLFWwindow* window, bool install_callbacks) {
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

            ImGui::StyleColorsDark();
            ImGui::GetStyle().WindowRounding = 5;

            ImGui_ImplGlfw_InitForOpenGL(window, install_callbacks);
            ImGui_ImplOpenGL3_Init("#version 330");
            return true;
        }

        void newFrame() {
            ImGui_ImplGlfw_NewFrame();
            ImGui_ImplOpenGL3_NewFrame();
            ImGui::NewFrame();
        }

        void render() {
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        void shutdown() {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
//			ImGui::Shutdown();
        }
    }

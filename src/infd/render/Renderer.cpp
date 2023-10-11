//
// Created by Crispin on 26/09/23.
//
#include "infd/render/Renderer.hpp"

#include <utility>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include "infd/Wavefront.hpp"
#include "infd/util/handle_vector.hpp"

namespace infd::render {
    void Renderer::render() {
        _render_settings.camera_pos = _test_camera.pos;
        _render_settings.camera_dir = {_test_camera.angle_x, _test_camera.angle_y, -1};
        auto view_target = _test_camera.pos + _render_settings.camera_dir;
        _render_settings.temp_view = glm::lookAt(_test_camera.pos, view_target, {0, 1, 0});
        _render_settings.pattern_angle = _test_camera.pattern_angle;
        _render_settings.temp_light_pos = _test_light_pos;
        _pipeline.render(_render_components, _render_settings);
    }

    void Renderer::reloadShaders() {
        _pipeline.loadShaders();
    }

    void Renderer::setRenderSettings(RenderSettings settings) {
        if (settings.screen_size != _render_settings.screen_size) {
            _pipeline.screenSizeChanged(settings.screen_size);
        }
        _render_settings = settings;
    }

    void Renderer::gui() {
        if (ImGui::Button("Reload shaders")) {
            reloadShaders();
        }

        ImGui::SliderFloat3("Camera pos", glm::value_ptr(_test_camera.pos), -50, 50);
        ImGui::SliderFloat("View angle X", &_test_camera.angle_x, -1, 1);
        ImGui::SliderFloat("View angle Y", &_test_camera.angle_y, -1, 1);
        ImGui::SliderFloat("Pattern angle", &_test_camera.pattern_angle, 0, glm::pi<float>());
        ImGui::SliderFloat3("Light pos", glm::value_ptr(_test_light_pos), -50, 50);

    }

    RenderComponentHandler Renderer::addRenderComponent(RenderComponent& component) {
        return _render_components.push_back(&component);
    }
}
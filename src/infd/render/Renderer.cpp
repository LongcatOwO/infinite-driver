//
// Created by Crispin on 26/09/23.
//
#include "infd/render/Renderer.hpp"

#include <utility>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include "infd/Wavefront.hpp"

namespace infd::render {
    Renderer::Renderer() {
        _test_items = {
                {loadWavefrontCases(CGRA_SRCDIR + std::string("/res//assets//plane.obj")).build(), glm::scale(glm::mat4 {1}, {5, 0, 5}), {}},
                {loadWavefrontCases(CGRA_SRCDIR + std::string("/res//assets//teapot.obj")).build(), glm::translate(glm::mat4 {1}, {-5, 3, 2}), {}},
                {loadWavefrontCases(CGRA_SRCDIR + std::string("/res//assets//bunny.obj")).build(),
                    glm::translate(glm::mat4 {1}, {8, 4, -1.5}) * glm::scale(glm::mat4 {1}, glm::vec3 {75}), {}}
        };
    }

    void Renderer::render(std::vector<RenderItem> items) {
        auto view_target = _test_camera.pos + glm::vec3 {_test_camera.angle, 0, -1};
        _render_settings.temp_view = glm::lookAt(_test_camera.pos, view_target, {0, 1, 0});
        _pipeline.render(std::move(items), _render_settings);
    }

    void Renderer::render() {
        render(_test_items);
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
        ImGui::SliderFloat3("Camera pos", glm::value_ptr(_test_camera.pos), -100, 100);
        ImGui::SliderFloat("View angle", &_test_camera.angle, -1, 1);
    }

}
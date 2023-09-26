//
// Created by Crispin on 26/09/23.
//
#include "infd/render/Renderer.hpp"

#include <utility>
#include <glm/gtc/matrix_transform.hpp>
#include "infd/Wavefront.hpp"

namespace infd::render {
    Renderer::Renderer() {
        _test_items = {
                {loadWavefrontCases(CGRA_SRCDIR + std::string("/res//assets//plane.obj")).build(), glm::scale(glm::mat4 {1}, {5, 0, 5}), {}},
                {loadWavefrontCases(CGRA_SRCDIR + std::string("/res//assets//teapot.obj")).build(), glm::translate(glm::mat4 {1}, {-5, 3, 2}), {}},
                {loadWavefrontCases(CGRA_SRCDIR + std::string("/res//assets//better_bunny.obj")).build(),
                    glm::translate(glm::mat4 {1}, {8, 4, -1.5}) * glm::scale(glm::mat4 {1}, glm::vec3 {75}), {}}
        };
    }

    void Renderer::render(std::vector<RenderItem> items) {
        _pipeline.render(std::move(items), _render_settings);
    }

    void Renderer::render() {
        render(_test_items);
    }



}
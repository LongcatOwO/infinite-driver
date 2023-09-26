//
// Created by Crispin on 26/09/23.
//
#include "infd/render/Renderer.hpp"

#include <utility>
#include "infd/Wavefront.hpp"

namespace infd::render {
    std::vector<RenderItem> Renderer::_test_items {
            {loadWavefrontCases(CGRA_SRCDIR + std::string("/res//assets//teapot.obj")).build(), glm::mat4 {1}, {}}
    };

    void Renderer::render(std::vector<RenderItem> items) {
        _pipeline.render(std::move(items), _render_settings);
    }

    void Renderer::render() {
        render(_test_items);
    }

}
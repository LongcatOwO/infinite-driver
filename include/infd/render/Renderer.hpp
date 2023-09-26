//
// Created by Crispin on 24/09/23.
//

#pragma once
#include <glm/glm.hpp>
#include <infd/GLMesh.hpp>
#include <infd/render/Pipeline.hpp>

namespace infd::render {
    // every item that gets rendered in the scene tree has one of these,
    // and they all get collected and passed to the Renderer
    // may change shape _somewhat_ as the architecture of the scene tree/
    // component system gets worked out
    struct RenderItem {
        GLMesh mesh;
        glm::mat4 transform {1};

        struct {
            float shininess = 0.5;
            // etc
        } material;
    };
    // other things to consider - how lights/cameras/etc work in the scene

    struct RenderSettings {
        struct {
            int width;
            int height;
        } screen;
        glm::mat4 temp_view;
        glm::mat4 temp_proj;
    };
    // class that does all the rendering - the application class has one of it and calls it to draw
    class Renderer {
        Pipeline _pipeline;
        RenderSettings _render_settings;
        std::vector<RenderItem> _test_items;
     public:
        Renderer();
        // (or later takes/finds scene tree and walks that itself
        void render(std::vector<RenderItem> items);
        // temp for testing, uses dummy info
        void render();
        void setRenderSettings(RenderSettings settings) {
            _render_settings = settings;
        };
    };
}
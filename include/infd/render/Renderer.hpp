//
// Created by Crispin on 24/09/23.
//

#pragma once
#include <glm/glm.hpp>
#include <infd/GLMesh.hpp>

// include/infd/render/renderer.hpp draft
// this is the "public" "interface" of the renderer system
// ideally the only header file for it that stuff outside the renderer needs/uses

namespace infd::render {
    // every item that gets rendered in the scene tree has one of these,
    // and they all get collected and passed to the Renderer
    // may change shape _somewhat_ as the architecture of the scene tree/
    // component system gets worked out
    struct RenderInfo {
        GLMesh mesh;

        glm::mat4 transform_option_1;
        // or...
        struct {
            glm::vec3 pos;
            glm::vec3 scale;
            glm::vec3 rotate;
        } transform_option_2;

        struct {
            float shinyness;
            // etc
        } material;
    };
    // other things to consider - how lights/cameras/etc work in the scene

    struct RenderSettings; // tba, interface with gui probably
    // class that does all the rendering - the application class has one of it and calls it to draw
    class Renderer {
        // tempted to have a Pipeline object that does all the actual work and this serves as an interface for that
        // to keep this class neat
     public:
        // (or later takes/finds scene tree and walks that itself
        void render(std::vector<RenderInfo>);
        // temp for testing, uses dummy info
        void render();
        void setRenderSettings(RenderSettings settings);
    };
}
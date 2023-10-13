//
// Created by Crispin on 24/09/23.
//

#pragma once
#include <glm/glm.hpp>
#include <infd/GLMesh.hpp>
#include "Pipeline.hpp"
#include <infd/util/handle_vector.hpp>
#include "RenderComponent.hpp"

namespace infd::render {

    struct RenderSettings {
        glm::ivec2 screen_size;
        float pattern_angle;
        bool render_original = false;
        bool render_wireframe = false;
        bool dither_colour = true;
    };

    class Renderer {
        Pipeline _pipeline;
        RenderSettings _render_settings;
        util::handle_vector<RenderComponent*> _render_components;
        struct {
            float pattern_angle = 0;
        } _test_camera;
     public:
        static DirectionalLightComponent* _light;
        static CameraComponent* _camera;

        RenderComponentHandler addRenderComponent(RenderComponent& component);
        void render();
        void setRenderSettings(RenderSettings settings);
        void reloadShaders();
        void gui();
    };
}
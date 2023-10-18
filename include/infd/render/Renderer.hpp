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
        bool render_original = false;
        bool render_wireframe = false;
    };

    class Renderer {
        Pipeline _pipeline;
        RenderSettings _render_settings;
        util::handle_vector<RenderComponent*> _render_components;
     public:
        static DirectionalLightComponent* _light;
        static CameraComponent* _camera;
        static DitherSettingsComponent* _dither;

        RenderComponentHandler addRenderComponent(RenderComponent& component);
        void render();
        void setRenderSettings(RenderSettings settings);
        void reloadShaders();
        void gui();
    };
}
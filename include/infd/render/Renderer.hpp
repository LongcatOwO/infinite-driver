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
    using RenderComponentHandler = util::handle_vector<RenderComponent*>::element_handler;

    // other things to consider - how lights/cameras/etc work in the scene

    struct RenderSettings {
        glm::ivec2 screen_size;
        float pattern_angle;
        glm::vec3 camera_dir;
        glm::mat4 temp_view;
        glm::mat4 temp_proj;
        bool render_dither = false;
    };
    // class that does all the rendering - the application class has one of it and calls it to draw
    class Renderer {
        Pipeline _pipeline;
        RenderSettings _render_settings;
        util::handle_vector<RenderComponent*> _render_components;
        struct {
            glm::vec3 pos {0, 15, 30};
            glm::vec3 dir;
            float angle_x = 0;
            float angle_y = 0;
            float pattern_angle = 0;
        } _test_camera;
     public:
        RenderComponentHandler addRenderComponent(RenderComponent& component);
        void render();
        void setRenderSettings(RenderSettings settings);
        void reloadShaders();
        void gui();
    };
}
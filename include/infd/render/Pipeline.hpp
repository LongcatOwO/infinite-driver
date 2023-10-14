//
// Created by Crispin on 26/09/23.
//

#pragma once
#include <vector>
#include "Framebuffer.hpp"
#include "Utils.hpp"
#include "infd/util/handle_vector.hpp"
#include "infd/render/fwd/RenderComponent.hpp"
#include "infd/GLObject.hpp"

namespace infd::render {
    const int shadow_res = 1024;
    // forward declare to avoid recursive inclusion hellscape
    struct RenderSettings;

    class Pipeline {
        GLProgram _main_shader;
        GLProgram _shadow_shader;
        GLProgram _dither_shader;
        GLProgram _blit_shader;
        GLProgram _threshold_blit_shader;
        GLProgram _sky_shader;
        GLProgram _outline_shader;

        Framebuffer _scene_buf;
        Framebuffer _shadow_buf;
        Framebuffer _dither_dome_buf;
        Framebuffer _sky_buf;
        Framebuffer _outline_buf;
        Framebuffer _final_buf;

        GLMesh _fullscreen_mesh = build_fullscreen_texture_mesh();
        GLMesh _sky_sphere;
        GLTexture _dither_texture;
        GLTexture _temp_sphere_texture;
     public:
        Pipeline();
        void render(util::handle_vector<RenderComponent*>&, const RenderSettings& settings,
                    const DirectionalLightComponent& light, const CameraComponent& camera, const DitherSettingsComponent& dither);
        void loadShaders();
        // MUST be called before draw with proper args to init
        void screenSizeChanged(glm::ivec2 new_size);
    };
}

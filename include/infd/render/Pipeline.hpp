//
// Created by Crispin on 26/09/23.
//

#pragma once
#include <vector>
#include "infd/render/Utils.hpp"
#include "infd/GLObject.hpp"

namespace infd::render {
    // forward declare to avoid recursive inclusion hellscape
    struct RenderItem;
    struct RenderSettings;

    class Pipeline {
        GLProgram _main_shader;
        GLProgram _fullscreen_texture_shader;
        bool _buffers_ready = false;
        GLMesh _fullscreen_mesh = build_fullscreen_texture_mesh();
        struct {
            GLFramebuffer buffer;
            GLTexture colour;
            GlRenderBuffer depth;
        } _fb;
     public:
        Pipeline();
        void render(std::vector<RenderItem> items, const RenderSettings& settings);
        void loadShaders();
        // MUST be called before draw with proper args to init
        void screenSizeChanged(std::pair<int, int> new_size);
    };
}

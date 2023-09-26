//
// Created by Crispin on 26/09/23.
//

#pragma once
#include <vector>
#include "infd/GLObject.hpp"

namespace infd::render {
    // forward declare to avoid recursive inclusion hellscape
    struct RenderItem;
    struct RenderSettings;

    class Pipeline {
        GLProgram _main_shader;
     public:
        Pipeline();
        void render(std::vector<RenderItem> items, const RenderSettings& settings);
        void loadShaders();
        void screenSizeChanged(std::pair<int, int> new_size);
    };
}

//
// Created by Crispin on 26/09/23.
//

#pragma once
#include <vector>
//#include <infd/render/Renderer.hpp>

namespace infd::render {
    // forward declare to avoid recursive inclusion hellscape
    struct RenderItem;
    struct RenderSettings;

    class Pipeline {
     public:
        void render(std::vector<RenderItem> items, const RenderSettings& settings);
    };
}

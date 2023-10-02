//
// Created by Crispin on 2/10/23.
//

#pragma once
#include "infd/GLObject.hpp"
#include "infd/ScopeGuard.hpp"

namespace infd::render {

    class Framebuffer {
        bool _valid = false;
        std::pair<int, int> size;
     public:
        GLFramebuffer buffer;
        GLTexture colour;
        GlRenderBuffer depth;
        [[nodiscard]] bool valid() const {
            return _valid;
        }
        void setSize(std::pair<int, int> new_size);
        void renderToScreen();
        void renderToOther(const GLProgram& shader, const Framebuffer& other);
        // call after framebuffer is bound
        void setupDraw() const;
//        [[nodiscard]] auto scopedFramebuffer() const {
//            auto guard = scopedBind(buffer, GL_FRAMEBUFFER);
//
//            return guard;
//        }
    };

}
//
// Created by Crispin on 2/10/23.
//

#pragma once
#include "infd/GLObject.hpp"
#include "infd/ScopeGuard.hpp"
#include "infd/GLMesh.hpp"

namespace infd::render {
    class Framebuffer {
        bool _valid = false;
        std::pair<int, int> _size;
     public:
        GLFramebuffer buffer;
        GLTexture colour;
        GlRenderBuffer depth;
        [[nodiscard]] bool valid() const {
            return _valid;
        }
        void setSize(std::pair<int, int> new_size);
        // bind shader program BEFORE calling these
        void renderToScreen(const GLProgram& shader, const GLMesh& display) const;
        void renderToOther(const GLProgram& shader, const Framebuffer& other, const GLMesh& display) const;
        // call after framebuffer is bound
        void setupDraw() const;
    };

}
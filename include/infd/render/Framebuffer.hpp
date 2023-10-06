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
        glm::ivec2 _size;
     public:
        GLFramebuffer buffer;
        GLTexture colour;
        GlRenderBuffer depth;
        [[nodiscard]] bool valid() const {
            return _valid;
        }
        void setSize(glm::ivec2 new_size);
        // bind shader program BEFORE calling these
        void renderToScreen(const GLProgram& shader, const GLMesh& display, const glm::ivec2 screen_size) const;
        void renderToOther(const GLProgram& shader, const Framebuffer& other, const GLMesh& display) const;
        // call after framebuffer is bound
        void setupDraw() const;
    };

}
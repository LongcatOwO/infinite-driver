//
// Created by Crispin on 2/10/23.
//

#include "infd/render/Framebuffer.hpp"
#include <iostream>

namespace infd::render {
    void Framebuffer::setSize(std::pair<int, int> new_size) {
        auto [width, height] = new_size;
        {
            auto fb_guard       = scopedBind(buffer, GL_FRAMEBUFFER);
            auto colour_guard   = scopedBind(colour, GL_TEXTURE_2D);
            auto depth_guard    = scopedRenderbuffer(depth);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colour, 0);

            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                std::cerr << "Error: Framebuffer not complete" << std::endl;
                abort();
            }
        }

        _valid = true;
    }

    void Framebuffer::renderToScreen() {

    }

    void Framebuffer::renderToOther(const GLProgram& shader, const Framebuffer& other) {

    }

    void Framebuffer::setupDraw() const {
        glViewport(0, 0, size.first, size.second); // set the viewport to draw to the entire window

        glClearColor(0, 0, 0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
    }
}
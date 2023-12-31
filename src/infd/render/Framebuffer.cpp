//
// Created by Crispin on 2/10/23.
//

#include "infd/render/Framebuffer.hpp"
#include "glm/vec4.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <iostream>

namespace infd::render {
    void Framebuffer::setSize(glm::ivec2 new_size) {
        int width = new_size.x; int height = new_size.y;
        {
            auto fb_guard = scopedBind(buffer, GL_FRAMEBUFFER);

            {
                auto colour_guard = scopedBind(colour, GL_TEXTURE_2D);

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colour, 0);
            }

            {
                auto depth_guard = scopedBind(depth, GL_TEXTURE_2D);

                glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                             nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
                glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(glm::vec4{1}));

                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth, 0);
            }

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                std::cerr << "Error: Framebuffer not complete" << std::endl;
                abort();
            }
        }
        _size = new_size;
        _valid = true;
    }

    void Framebuffer::renderToScreen(const GLProgram& shader, const GLMesh& display, glm::ivec2 screen_size, bool clear, Kind kind) const {
        glActiveTexture(GL_TEXTURE0);
        auto texture_guard = kind == Kind::Colour ? scopedBind(colour, GL_TEXTURE_2D) : scopedBind(depth, GL_TEXTURE_2D);
        glUniform1i(glGetUniformLocation(shader, "uFramebuffer"), 0);

        glViewport(0, 0, screen_size.x, screen_size.y);
        if (clear) {
            glClearColor(1, 1, 1, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
        }
        glDisable(GL_DEPTH_TEST);

        display.draw();
    }

    void Framebuffer::renderToOther(const GLProgram& shader, const Framebuffer& other, const GLMesh& display, bool clear, Kind kind) const {
        auto buffer_handle = scopedBind(other.buffer, GL_FRAMEBUFFER);
        glActiveTexture(GL_TEXTURE0);
        auto texture_guard = kind == Kind::Colour ? scopedBind(colour, GL_TEXTURE_2D) : scopedBind(depth, GL_TEXTURE_2D);
        glUniform1i(glGetUniformLocation(shader, "uFramebuffer"), 0);

        glViewport(0, 0, other._size.x, other._size.y);
        if (clear) {
            glClearColor(1, 1, 1, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);
        } else {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        glDisable(GL_DEPTH_TEST);

        display.draw();
        glDisable(GL_BLEND);
    }

    void Framebuffer::setupDraw(glm::vec3 clear_colour) const {
        glViewport(0, 0, _size.x, _size.y); // set the viewport to draw to the entire window

        glClearColor(clear_colour.r, clear_colour.g, clear_colour.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
    }
}

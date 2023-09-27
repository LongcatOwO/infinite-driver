//
// Created by Crispin on 26/09/23.
//

#include "infd/render/Pipeline.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <iostream>
#include <glm/glm.hpp>
#include <cgra/cgra_image.hpp>

#include <infd/Shader.hpp>
#include <infd/ScopeGuard.hpp>
#include <infd/render/Renderer.hpp>


infd::render::Pipeline::Pipeline() {
    loadShaders();
    // load dither texture
    {
        auto texture_guard = scopedBind(_dither_texture, GL_TEXTURE_2D);

        auto tex = cgra::rgba_image {CGRA_SRCDIR + std::string("/res/textures/dithers/16x16-ordered-dither.png")};

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex.size.x, tex.size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.data.data());
    }
}

void infd::render::Pipeline::render(std::vector<RenderItem> items, const infd::render::RenderSettings& settings) {
    if (!_buffers_ready) {
        std::cerr << "Error: Buffers not initialised before render called (screen size not set?)" << std::endl;
        abort();
    }
    auto [width, height] = settings.screen_size;

    using namespace glm;
    // draw scene to buffer
    {
        auto program_guard = scopedProgram(_main_shader);
        auto fb_guard = scopedBind(_fb.buffer, GL_FRAMEBUFFER);

        glViewport(0, 0, width, height); // set the viewport to draw to the entire window

        glClearColor(0.3f, 0.3f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        glUniform3fv(glGetUniformLocation(_main_shader, "uColour"), 1, glm::value_ptr(vec3{1, 0, 1}));
        glUniform3fv(glGetUniformLocation(_main_shader, "uLightPos"), 1, glm::value_ptr(vec3{10}));
        glUniformMatrix4fv(glGetUniformLocation(_main_shader, "uProjectionMatrix"), 1, false, value_ptr(settings.temp_proj));
        glUniformMatrix4fv(glGetUniformLocation(_main_shader, "uViewMatrix"), 1, false, value_ptr(settings.temp_view));

        for (auto& item : items) {
            glUniformMatrix4fv(glGetUniformLocation(_main_shader, "uModelMatrix"), 1, false, value_ptr(item.transform));
            glUniform1f(glGetUniformLocation(_main_shader, "uShininess"), item.material.shininess);
            item.mesh.draw();
        }
    }

    // draw to screen from buffer
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        auto program_guard = scopedProgram(_fullscreen_texture_shader);
        glActiveTexture(GL_TEXTURE0);
        auto texture_guard = scopedBind(_fb.colour, GL_TEXTURE_2D);

        glViewport(0, 0, width, height);
        glClearColor(1, 1, 1, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        _fullscreen_mesh.draw();
    }
}

void infd::render::Pipeline::loadShaders() {
    ShaderBuilder main_shader_build;
    main_shader_build.setShader(GL_VERTEX_SHADER, CGRA_SRCDIR + std::string("//res//shaders//phong_vert.glsl"));
    main_shader_build.setShader(GL_FRAGMENT_SHADER, CGRA_SRCDIR + std::string("//res//shaders//phong_frag.glsl"));
    _main_shader = main_shader_build.build();

    ShaderBuilder fullscreen_texture_build;
    fullscreen_texture_build.setShader(GL_VERTEX_SHADER, CGRA_SRCDIR + std::string("//res//shaders//fullscreen_texture_vert.glsl"));
    fullscreen_texture_build.setShader(GL_FRAGMENT_SHADER, CGRA_SRCDIR + std::string("//res//shaders//fullscreen_texture_frag.glsl"));
    _fullscreen_texture_shader = fullscreen_texture_build.build();
}

void infd::render::Pipeline::screenSizeChanged(std::pair<int, int> new_size) {
    auto [width, height] = new_size;
    {
        auto fb_guard       = scopedBind(_fb.buffer, GL_FRAMEBUFFER);
        auto colour_guard   = scopedBind(_fb.colour, GL_TEXTURE_2D);
        auto depth_guard    = scopedRenderbuffer(_fb.depth);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _fb.colour, 0);

        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _fb.depth);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "Error: Framebuffer not complete" << std::endl;
            abort();
        }
    }

    _buffers_ready = true;
}



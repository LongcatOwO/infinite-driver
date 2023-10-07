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
#include <infd/Wavefront.hpp>


infd::render::Pipeline::Pipeline() : _sky_sphere{loadWavefrontCases(CGRA_SRCDIR + std::string("/res/assets/sky_sphere.obj")).build()} {
    loadShaders();
    // load dither texture
    {
        {
            auto texture_guard = scopedBind(_dither_texture, GL_TEXTURE_2D);

            auto tex = cgra::rgba_image{CGRA_SRCDIR + std::string("/res/textures/dithers/16x16-ordered-dither.png")};
//        auto tex = cgra::rgba_image {CGRA_SRCDIR + std::string("/res/textures/dithers/64x64-blue-noise.png")};

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex.size.x, tex.size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                         tex.data.data());
        }

        {
            auto texture_guard = scopedBind(_temp_sphere_texture, GL_TEXTURE_2D);

            auto tex = cgra::rgba_image{CGRA_SRCDIR + std::string("/res/textures/uv_texture.jpg")};

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex.size.x, tex.size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                         tex.data.data());
        }

    }
}

void infd::render::Pipeline::render(std::vector<RenderItem> items, const infd::render::RenderSettings& settings) {
    if (!(_fx_buf.valid() && _final_buf.valid())) {
        std::cerr << "Error: Buffers not initialised before render called (screen size not set?)" << std::endl;
        abort();
    }

    int width = settings.screen_size.x; int height = settings.screen_size.y;

    using namespace glm;
    // draw scene to buffer
    {
        auto program_guard = scopedProgram(_main_shader);
        auto fb_guard = scopedBind(_fx_buf.buffer, GL_FRAMEBUFFER);
        _fx_buf.setupDraw();

        sendUniform(_main_shader, "uColour", {1, 0, 1});
        sendUniform(_main_shader, "uLightPos", vec3{10});
        sendUniform(_main_shader, "uProjectionMatrix", settings.temp_proj);
        sendUniform(_main_shader, "uViewMatrix", settings.temp_view);

        for (auto& item : items) {
            sendUniform(_main_shader, "uModelMatrix", item.transform);
            sendUniform(_main_shader, "uShininess", item.material.shininess);
            item.mesh.draw();
        }
    }

    // draw dither texture onto skydome
    {
        auto fb_guard = scopedBind(_dither_dome_buf.buffer, GL_FRAMEBUFFER);
        _dither_dome_buf.setupDraw();
        auto program_guard = scopedProgram(_sky_shader);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_CLAMP);

        auto view = glm::lookAt({0, 0, 0}, settings.camera_dir, {0, 1, 0});

        glActiveTexture(GL_TEXTURE1);
        auto sky_texture_guard = scopedBind(_dither_texture, GL_TEXTURE_2D);
        sendUniform(_sky_shader, "uTex", 1);
        sendUniform(_sky_shader, "uScreenSize", (glm::vec2 {width, height}) * 1.f);
        sendUniform(_sky_shader, "uPatternAngle", settings.pattern_angle);
        sendUniform(_sky_shader, "uProjectionMatrix", settings.temp_proj);
        sendUniform(_sky_shader, "uViewMatrix", view);
        sendUniform(_sky_shader, "uModelMatrix", glm::mat4 {1});
        _sky_sphere.draw();
    }

    //draw dither from fx -> final buf
    {
        auto program_guard = scopedProgram(_dither_shader);

        glActiveTexture(GL_TEXTURE1);
        auto dither_texture_guard = scopedBind(_dither_dome_buf.colour, GL_TEXTURE_2D);
        sendUniform(_dither_shader, "uDitherPattern", 1);

        _fx_buf.renderToOther(_dither_shader, _final_buf, _fullscreen_mesh);
    }

    // draw to screen from final buffer
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        if (settings.render_dither) {
            auto program_guard = scopedProgram(_blit_shader);
            _dither_dome_buf.renderToScreen(_blit_shader, _fullscreen_mesh, settings.screen_size);
        } else {
            auto program_guard = scopedProgram(_threshold_blit_shader);
            _final_buf.renderToScreen(_threshold_blit_shader, _fullscreen_mesh, settings.screen_size);
        }
    }
}

void infd::render::Pipeline::loadShaders() {
    ShaderBuilder main_shader_build;
    main_shader_build.setShader(GL_VERTEX_SHADER, CGRA_SRCDIR + std::string("//res//shaders//phong_vert.glsl"));
    main_shader_build.setShader(GL_FRAGMENT_SHADER, CGRA_SRCDIR + std::string("//res//shaders//phong_frag.glsl"));
    _main_shader = main_shader_build.build();

    ShaderBuilder dither_build;
    dither_build.setShader(GL_VERTEX_SHADER, CGRA_SRCDIR + std::string("//res//shaders//fullscreen_vert.glsl"));
    dither_build.setShader(GL_FRAGMENT_SHADER, CGRA_SRCDIR + std::string("//res//shaders//dither_frag.glsl"));
    _dither_shader = dither_build.build();

    ShaderBuilder blit_build;
    blit_build.setShader(GL_VERTEX_SHADER, CGRA_SRCDIR + std::string("//res//shaders//fullscreen_vert.glsl"));
    blit_build.setShader(GL_FRAGMENT_SHADER, CGRA_SRCDIR + std::string("//res//shaders//blit_frag.glsl"));
    _blit_shader = blit_build.build();

    ShaderBuilder threshold_blit_build;
    threshold_blit_build.setShader(GL_VERTEX_SHADER, CGRA_SRCDIR + std::string("//res//shaders//fullscreen_vert.glsl"));
    threshold_blit_build.setShader(GL_FRAGMENT_SHADER, CGRA_SRCDIR + std::string("//res//shaders//threshold_blit_frag.glsl"));
    _threshold_blit_shader = threshold_blit_build.build();

    ShaderBuilder sky_build;
    sky_build.setShader(GL_VERTEX_SHADER, CGRA_SRCDIR + std::string("//res//shaders//phong_vert.glsl"));
    sky_build.setShader(GL_FRAGMENT_SHADER, CGRA_SRCDIR + std::string("//res//shaders//obj_texture_frag.glsl"));
    _sky_shader = sky_build.build();
}

void infd::render::Pipeline::screenSizeChanged(glm::ivec2 new_size) {
    _fx_buf.setSize(new_size * 2);
    _dither_dome_buf.setSize(new_size * 2);
    _final_buf.setSize(new_size * 2);
}



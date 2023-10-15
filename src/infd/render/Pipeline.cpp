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
        std::string texture_paths[] = {
                "/res/textures/dithers/64x64-blue-noise.png",
                "/res/textures/dithers/16x16-ordered-dither.png",
                "/res/textures/dithers/8x8-halftone-dot.png",
                "/res/textures/dithers/8x8-funny-lines.png",
                "/res/textures/dithers/8x8-square-halftone.png"
        };


        for (int i = 0; i < (int)Dithers::Count; i++) {
            auto texture_guard = scopedBind(_dithers[i], GL_TEXTURE_2D);

            auto tex = cgra::rgba_image{CGRA_SRCDIR + texture_paths[i]};

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

void infd::render::Pipeline::render(util::handle_vector<RenderComponent*>& items, const RenderSettings& settings,
                                    const DirectionalLightComponent& light, const CameraComponent& camera, const DitherSettingsComponent& dither) {
    using namespace glm;
    if (!(_scene_buf.valid() && _final_buf.valid())) {
        std::cerr << "Error: Buffers not initialised before render called (screen size not set?)" << std::endl;
        abort();
    }

    int width = settings.screen_size.x; int height = settings.screen_size.y;

    const float shadow_size = 10;
    const float shadow_cam_offset = 4;

    vec3 shadow_cam = light.transform().globalPosition() - (normalize(light.direction) * shadow_cam_offset);

    vec3 up_vec = normalize(shadow_cam) != vec3{0, 1, 0} ? vec3{0, 1, 0} : vec3{0, 0, 1} ;
    auto shadow_view = lookAt(shadow_cam, {0, 0, 0}, up_vec);

    auto shadow_proj = ortho(-shadow_size, shadow_size, -shadow_size, shadow_size, -0.f, 100.f);

    // render shadow buffer
    {
        auto program_guard = scopedProgram(_shadow_shader);
        auto fb_guard = scopedBind(_shadow_buf.buffer, GL_FRAMEBUFFER);
        _shadow_buf.setupDraw();

        sendUniform(_shadow_shader, "uProjectionMatrix", shadow_proj);
        sendUniform(_shadow_shader, "uViewMatrix", shadow_view);

        for (auto& item : items) {
            sendUniform(_shadow_shader, "uModelMatrix", item->transform().globalTransform());
            item->mesh.draw();
        }
    }

    auto proj = camera.proj(settings.screen_size);
    auto view = camera.view();

    if (settings.render_wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    // draw scene to buffer
    {
        auto program_guard = scopedProgram(_main_shader);
        auto fb_guard = scopedBind(_scene_buf.buffer, GL_FRAMEBUFFER);
        _scene_buf.setupDraw(dither.sky_colour);

        sendUniform(_main_shader, "uLightDir", light.direction);
        sendUniform(_main_shader, "uCameraPos", camera.transform().globalPosition());
        sendUniform(_main_shader, "uProjectionMatrix", proj);
        sendUniform(_main_shader, "uViewMatrix", view);

        glActiveTexture(GL_TEXTURE0);
        auto tex_guard = scopedBind(_shadow_buf.depth, GL_TEXTURE_2D);
        sendUniform(_main_shader, "uShadowTex", 0);
        sendUniform(_main_shader, "uShadowMatrix", shadow_proj * shadow_view);

        for (auto& item : items) {
            sendUniform(_main_shader, "uModelMatrix", item->transform().globalTransform());
            sendUniform(_main_shader, "uColour", item->material.colour);
            sendUniform(_main_shader, "uShininess", item->material.shininess);
            item->mesh.draw();
        }
    }

    if (settings.render_wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // draw dither texture onto skydome
    {
        auto fb_guard = scopedBind(_dither_dome_buf.buffer, GL_FRAMEBUFFER);
        _dither_dome_buf.setupDraw();
        auto program_guard = scopedProgram(_sky_shader);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_CLAMP);

        auto sphere_view = glm::lookAt({0, 0, 0}, camera.forward(), {0, 1, 0});

        glActiveTexture(GL_TEXTURE1);
        auto sky_texture_guard = scopedBind(_dithers[(int)dither.dither_pattern], GL_TEXTURE_2D);
        sendUniform(_sky_shader, "uTex", 1);
        sendUniform(_sky_shader, "uScreenSize", glm::vec2 {(float)width, (float)height});
        sendUniform(_sky_shader, "uPatternAngle", dither.pattern_angle);
        sendUniform(_sky_shader, "uFov", camera.fov);
        sendUniform(_sky_shader, "uProjectionMatrix", proj);
        sendUniform(_sky_shader, "uViewMatrix", sphere_view);
        sendUniform(_sky_shader, "uModelMatrix", glm::mat4 {1});
        _sky_sphere.draw();
    }

    // draw outlines from fx -> outline buf
    {
        auto program_guard = scopedProgram(_outline_shader);
        sendUniform(_outline_shader, "uScreenSize", settings.screen_size);
        sendUniform(_outline_shader, "uWidth", 6.f);
        _scene_buf.renderToOther(_outline_shader, _outline_buf, _fullscreen_mesh, true, Framebuffer::Kind::Depth);
    }

    //draw dither from fx -> final buf
    {
        auto program_guard = scopedProgram(_dither_shader);

        glActiveTexture(GL_TEXTURE1);
        auto dither_texture_guard = scopedBind(_dither_dome_buf.colour, GL_TEXTURE_2D);
        sendUniform(_dither_shader, "uDitherPattern", 1);
        sendUniform(_dither_shader, "uDitherColour", dither.dither_colour);

        _scene_buf.renderToOther(_dither_shader, _final_buf, _fullscreen_mesh);
    }

    // render outlines over dithered scene
    {
        if (!settings.render_original) {
            auto program_guard = scopedProgram(_blit_shader);
            _outline_buf.renderToOther(_blit_shader, _final_buf, _fullscreen_mesh, false);
        }
    }

    // draw to screen from final buffer
    {
        if (settings.render_original || settings.render_wireframe) {
            auto program_guard = scopedProgram(_blit_shader);
            _scene_buf.renderToScreen(_blit_shader, _fullscreen_mesh, settings.screen_size);
        } else {
            auto program_guard = scopedProgram(_threshold_blit_shader);
            sendUniform(_threshold_blit_shader, "uThreshold", dither.threshold);
            sendUniform(_dither_shader, "uDitherColour", dither.dither_colour);
            _final_buf.renderToScreen(_threshold_blit_shader, _fullscreen_mesh, settings.screen_size);
//            auto program_guard = scopedProgram(_blit_shader);
//            _scene_buf.renderToScreen(_blit_shader, _fullscreen_mesh, settings.screen_size);
        }
    }
}

void infd::render::Pipeline::loadShaders() {
    ShaderBuilder main_shader_build;
    main_shader_build.setShader(GL_VERTEX_SHADER, CGRA_SRCDIR + std::string("//res//shaders//phong_vert.glsl"));
    main_shader_build.setShader(GL_FRAGMENT_SHADER, CGRA_SRCDIR + std::string("//res//shaders//phong_frag.glsl"));
    _main_shader = main_shader_build.build();

    ShaderBuilder shadow_build;
    shadow_build.setShader(GL_VERTEX_SHADER, CGRA_SRCDIR + std::string("//res//shaders//shadow_vert.glsl"));
    shadow_build.setShader(GL_FRAGMENT_SHADER, CGRA_SRCDIR + std::string("//res//shaders//shadow_frag.glsl"));
    _shadow_shader = shadow_build.build();

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

    ShaderBuilder outline_build;
    outline_build.setShader(GL_VERTEX_SHADER, CGRA_SRCDIR + std::string("//res//shaders//fullscreen_vert.glsl"));
    outline_build.setShader(GL_FRAGMENT_SHADER, CGRA_SRCDIR + std::string("//res//shaders//outline_frag.glsl"));
    _outline_shader = outline_build.build();
}

void infd::render::Pipeline::screenSizeChanged(glm::ivec2 new_size) {
    _scene_buf.setSize(new_size * 2);
    _dither_dome_buf.setSize(new_size * 2);
    _outline_buf.setSize(new_size);
    _final_buf.setSize(new_size * 2);
    _shadow_buf.setSize({shadow_res, shadow_res});
}



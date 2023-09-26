//
// Created by Crispin on 26/09/23.
//

#include "infd/render/Pipeline.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <glm/glm.hpp>

#include <infd/Shader.hpp>
#include <infd/ScopeGuard.hpp>
#include <infd/render/Renderer.hpp>


infd::render::Pipeline::Pipeline() {
    loadShaders();
}

void infd::render::Pipeline::render(std::vector<RenderItem> items, const infd::render::RenderSettings& settings) {
    using namespace glm;
    {
        auto program_guard = scopedProgram(_main_shader);

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
}

void infd::render::Pipeline::loadShaders() {
    ShaderBuilder main_shader_build;
    main_shader_build.setShader(GL_VERTEX_SHADER, CGRA_SRCDIR + std::string("//res//shaders//phong_vert.glsl"));
    main_shader_build.setShader(GL_FRAGMENT_SHADER, CGRA_SRCDIR + std::string("//res//shaders//phong_frag.glsl"));
    _main_shader = main_shader_build.build();
}



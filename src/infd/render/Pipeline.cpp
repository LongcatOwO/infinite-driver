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
    ShaderBuilder main_shader_build;
    main_shader_build.setShader(GL_VERTEX_SHADER, CGRA_SRCDIR + std::string("//res//shaders//color_vert.glsl"));
    main_shader_build.setShader(GL_FRAGMENT_SHADER, CGRA_SRCDIR + std::string("//res//shaders//color_frag.glsl"));
    _main_shader = main_shader_build.build();
}

void infd::render::Pipeline::render(std::vector<RenderItem> items, const infd::render::RenderSettings& settings) {
    using namespace glm;
    {
        scopedProgram(_main_shader);
        glUniform3fv(glGetUniformLocation(_main_shader, "uColor"), 1, glm::value_ptr(vec3{1, 0, 1}));
        glUniformMatrix4fv(glGetUniformLocation(_main_shader, "uProjectionMatrix"), 1, false, value_ptr(settings.temp_proj));

        for (auto& item : items) {
            auto model_view = settings.temp_view * item.transform;
            glUniformMatrix4fv(glGetUniformLocation(_main_shader, "uModelViewMatrix"), 1, false, value_ptr(model_view));
            item.mesh.draw();
        }
    }
}



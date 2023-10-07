//
// Created by crispin on 27/09/23.
//

#pragma once

#include <infd/GLMesh.hpp>
#include <glm/glm.hpp>
#include "glm/gtc/type_ptr.hpp"

namespace infd::render {
    GLMesh build_fullscreen_texture_mesh();

    inline void sendUniform(const GLProgram& shader, const std::string& name, float value) {
        glUniform1f(glGetUniformLocation(shader, name.c_str()), value);
    }

    inline void sendUniform(const GLProgram& shader, const std::string& name, int value) {
        glUniform1i(glGetUniformLocation(shader, name.c_str()), value);
    }

    inline void sendUniform(const GLProgram& shader, const std::string& name, glm::vec2 value) {
        glUniform2fv(glGetUniformLocation(shader, name.c_str()), 1, glm::value_ptr(value));
    }

    inline void sendUniform(const GLProgram& shader, const std::string& name, glm::vec3 value) {
        glUniform3fv(glGetUniformLocation(shader, name.c_str()), 1, glm::value_ptr(value));
    }

    inline void sendUniform(const GLProgram& shader, const std::string& name, glm::mat4 value) {
        glUniformMatrix4fv(glGetUniformLocation(shader, name.c_str()), 1, false, value_ptr(value));
    }
}
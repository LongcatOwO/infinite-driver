//
// Created by crispin on 27/09/23.
//
#include "infd/render/Utils.hpp"

namespace infd::render {
    GLMesh build_fullscreen_texture_mesh() {
        GLMeshBuilder mb;

        mb.vertices = {
                {glm::vec3{-1, -1, 0}, glm::vec3{0}, glm::vec2{0}},
                {glm::vec3{-1,  1, 0}, glm::vec3{0}, glm::vec2{0}},
                {glm::vec3{ 1,  1, 0}, glm::vec3{0}, glm::vec2{0}},
                {glm::vec3{ 1, -1, 0}, glm::vec3{0}, glm::vec2{0}}
        };
        mb.indices = {
                0, 1, 2,
                2, 3, 0
        };

        return mb.build();
    }
}
#pragma once

#include "infd/scene/Scene.hpp"
#include "Renderer.hpp"

namespace infd::render {
    class RenderComponent : public scene::Component {
    public:
        RenderComponent(GLMesh& mesh);
        RenderComponent(GLMesh mesh);

        GLMesh mesh;

        struct {
            float shininess = 20;
        } material;
    };
}
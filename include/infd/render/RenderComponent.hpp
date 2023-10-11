#pragma once

#include "infd/scene/Scene.hpp"
#include "fwd/RenderComponentHandler.hpp"
#include "fwd/Renderer.hpp"
#include "Renderer.hpp"
#include "infd/GLMesh.hpp"

namespace infd::render {
    class RenderComponent : public scene::Component {
    public:
        RenderComponent(Renderer& renderer, GLMesh& mesh);
        RenderComponent(Renderer& renderer, GLMesh mesh);

        GLMesh mesh;

        struct {
            float shininess = 20;
        } material;
    private:
        const RenderComponentHandler _hook;
    };
}
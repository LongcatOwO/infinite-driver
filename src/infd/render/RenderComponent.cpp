#include "infd/render/RenderComponent.hpp"

#include <utility>

namespace infd::render {
    RenderComponent::RenderComponent(Renderer& renderer, GLMesh& mesh) : mesh(mesh), _hook(renderer.addRenderComponent(*this)) {}
    RenderComponent::RenderComponent(Renderer& renderer, GLMesh mesh) : mesh(std::move(mesh)), _hook(renderer.addRenderComponent(*this)) {}
}
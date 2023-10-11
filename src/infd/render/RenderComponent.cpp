#include "infd/render/RenderComponent.hpp"

#include <utility>

namespace infd::render {
    RenderComponent::RenderComponent(GLMesh& mesh) : mesh(mesh) {}
    RenderComponent::RenderComponent(GLMesh mesh) : mesh(std::move(mesh)) {}
}
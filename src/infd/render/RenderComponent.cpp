#include "infd/render/RenderComponent.hpp"

#include "infd/util/exceptions.hpp"
#include "imgui.h"
#include <utility>

namespace infd::render {
    RenderComponent::RenderComponent(Renderer& renderer, GLMesh& mesh) : mesh(mesh), _hook(renderer.addRenderComponent(*this)) {}
    RenderComponent::RenderComponent(Renderer& renderer, GLMesh mesh) : mesh(std::move(mesh)), _hook(renderer.addRenderComponent(*this)) {}

    void DirectionalLightComponent::onAttach() {
        Component::onAttach();
        if (Renderer::_light == nullptr) {
            Renderer::_light = this;
        } else {
            throw infd::util::InvalidStateException("Attempted to attach light but renderer already has a light");
        }
    }

    void DirectionalLightComponent::gui() {
        ImGui::SliderFloat3("Light direction", glm::value_ptr(direction), -1, 1);
    }
}
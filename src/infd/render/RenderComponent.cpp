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

    void CameraComponent::onAttach() {
        Component::onAttach();
        if (Renderer::_camera == nullptr) {
            Renderer::_camera = this;
        } else {
            throw infd::util::InvalidStateException("Attempted to attach camera but renderer already has a camera");
        }
    }

    glm::vec3 CameraComponent::forward() const {
        glm::vec3 local_forward {0, 0, -1};
        return transform().globalRotation() * local_forward;
    }

    glm::mat4 CameraComponent::view() const {
        glm::vec3 pos = transform().globalPosition();
        return glm::lookAt(pos, pos + forward(), {0, 1, 0});
    }

    glm::mat4 CameraComponent::proj(glm::vec2 window_size) const {
        return glm::perspective(fov, window_size.x / window_size.y, 0.1f, 1000.f);
    }

    void CameraComponent::gui() {
        ImGui::SliderFloat("FOV", &fov, 1, 2);

        glm::vec3 v = transform().localPosition();
        ImGui::SliderFloat3("Camera pos", glm::value_ptr(v), -50, 50);
        transform().localPosition(v);

        ImGui::SliderFloat2("Camera rot", glm::value_ptr(euler_rot), -glm::pi<float>(), glm::pi<float>());
        transform().localRotation(euler_rot);
    }
}
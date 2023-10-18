#include "infd/render/RenderComponent.hpp"

#include "infd/util/exceptions.hpp"
#include "imgui.h"
#include <utility>

namespace infd::render {
    RenderComponent::RenderComponent(Renderer& renderer, const GLMesh& mesh) :
        mesh(mesh), _hook(renderer.addRenderComponent(*this)) {}

    RenderComponent::RenderComponent(Renderer& renderer, GLMesh&& mesh) :
        mesh(std::move(mesh)), _hook(renderer.addRenderComponent(*this)) {}

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

        // glm::vec3 v = transform().localPosition();
        // ImGui::SliderFloat3("Camera pos", glm::value_ptr(v), -50, 50);
        // transform().localPosition(v);

        // ImGui::SliderFloat2("Camera rot", glm::value_ptr(euler_rot), -glm::pi<float>(), glm::pi<float>());
        // transform().localRotation(euler_rot);
    }

    void DitherSettingsComponent::onAttach() {
        Component::onAttach();
        if (Renderer::_dither == nullptr) {
            Renderer::_dither = this;
        } else {
            throw infd::util::InvalidStateException("Attempted to attach dither settings but renderer already has dither settings");
        }
    }

    void DitherSettingsComponent::gui() {
        bool change_preset = ImGui::Combo("Preset", reinterpret_cast<int*>(&_preset), "Old-Skool Ordered 1-bit\0"
                                                              "Old-Skool Noise 1-bit\0"
                                                              "Ordered Colour\0"
                                                              "Noise Colour\0"
                                                              "Comic Half-Tone\0"
                                                              "Experimental Lines\0"
                                                              "Experimental Squares\0"
                                                              "Custom\0");
        if (change_preset)
            changePreset(_preset);

        if (_preset == Preset::Custom) {
            ImGui::Combo("Dither pattern", reinterpret_cast<int*>(&dither_pattern), "Blue Noise\0"
                                                                                    "Ordered\0"
                                                                                    "Halftone\0"
                                                                                    "Experimental Lines\0"
                                                                                    "Experimental Square Halftone\0");
            ImGui::SliderFloat("Pattern angle", &pattern_angle, 0, glm::pi<float>());
            ImGui::SliderFloat("Dither threshold", &threshold, 0, 1);
            ImGui::ColorEdit3("'Sky' colour", glm::value_ptr(sky_colour));
            ImGui::Checkbox("Colour dither", &dither_colour);
        }
    }

    void DitherSettingsComponent::changePreset(DitherSettingsComponent::Preset p) {
        switch (p) {
            case Preset::Ordered1Bit:
                dither_pattern = Dithers::Ordered;
                dither_colour = false;
                threshold = 0.33;
                pattern_angle = 0;
                sky_colour = {0, 0, 0};
                break;
            case Preset::Noise1Bit:
                dither_pattern = Dithers::BlueNoise;
                dither_colour = false;
                threshold = 0.5;
                pattern_angle = 0;
                sky_colour = {0, 0, 0};
                break;
            case Preset::OrderedColour:
                dither_pattern = Dithers::Ordered;
                dither_colour = true;
                threshold = 0.5;
                pattern_angle = 0.772;
                sky_colour = {0.512, 0.941, 0.929};
                break;
            case Preset::NoiseColour:
                dither_pattern = Dithers::BlueNoise;
                dither_colour = true;
                threshold = 0.5;
                pattern_angle = 2.720;
                sky_colour = {0.512, 0.941, 0.929};
                break;
            case Preset::Halftone:
                dither_pattern = Dithers::Halftone;
                dither_colour = false;
                threshold = 0.5;
                pattern_angle = 0.526;
                sky_colour = {0, 0, 0};
                break;
            case Preset::ExperimentalLines:
                dither_pattern = Dithers::ExperimentalLines;
                dither_colour = false;
                threshold = 0.5;
                pattern_angle = 0.947;
                sky_colour = {0, 0, 0};
                break;
            case Preset::ExperimentalSquares:
                dither_pattern = Dithers::ExperimentalHalftone;
                dither_colour = false;
                threshold = 0.5;
                pattern_angle = 0;
                sky_colour = {0, 0, 0};
                break;
            case Preset::Custom:
                break;
        }
    }
}

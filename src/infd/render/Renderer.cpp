//
// Created by Crispin on 26/09/23.
//
#include "infd/render/Renderer.hpp"

#include <utility>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include "infd/Wavefront.hpp"
#include "infd/util/handle_vector.hpp"

namespace infd::render {
    DirectionalLightComponent* Renderer::_light = nullptr;
    CameraComponent* Renderer::_camera = nullptr;
    DitherSettingsComponent* Renderer::_dither = nullptr;

    void Renderer::render() {
        if (_light == nullptr) {
            throw infd::util::InvalidStateException("Attempted to render but renderer knows no light");
        }
        if (_camera == nullptr) {
            throw infd::util::InvalidStateException("Attempted to render but renderer knows no camera");
        }
        if (_dither == nullptr) {
            throw infd::util::InvalidStateException("Attempted to render but renderer has no dither settings");
        }

        _pipeline.render(_render_components, _render_settings, *_light, *_camera, *_dither);
    }

    void Renderer::reloadShaders() {
        _pipeline.loadShaders();
    }

    void Renderer::setRenderSettings(RenderSettings settings) {
        if (settings.screen_size != _render_settings.screen_size) {
            _pipeline.screenSizeChanged(settings.screen_size);
        }
        _render_settings = settings;
    }

    void Renderer::gui() {
        if (ImGui::Button("Reload shaders")) {
            reloadShaders();
        }

        ImGui::Separator();
        _dither->gui();
        ImGui::Separator();
        _light->gui();
        ImGui::Separator();
        _camera->gui();
    }

    RenderComponentHandler Renderer::addRenderComponent(RenderComponent& component) {
        return _render_components.push_back(&component);
    }
}

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
            glm::vec3 colour {1, 0, 1};
            float shininess = 20;
        } material;
      private:
        const RenderComponentHandler _hook;
    };

    /*
     * Directional Light Component
     *
     * Precisely 1 of these must exist in the scene - connects itself to Renderer on attach
     * Throws on attach if there is a different light pointer in Renderer,
     * Renderer throws if it tries to render without a light
     *
     * direction is the direction of the light lol
     *
     * NOTE:    Global position is used as the centre of the shadow map so parent it to the player or
     *          something in the middle of the screen
     */

    class DirectionalLightComponent : public scene::Component {
      public:
        glm::vec3 direction {-0.0791808 -0.118771 -0.98976};

        void gui();

      protected:
        void onAttach() override;
    };

    /*
     * Camera Component
     *
     * Precisely 1 of these must exist in the scene - connects itself to Renderer on attach
     * Throws on attach if there is a different camera pointer in Renderer,
     * Renderer throws if it tries to render without a camera
     *
     * fov is the field of view in radians
     * forward() returns a vector pointing the direction the camera is facing
     * view() and proj() return the camera and (perspective) projection matrices
     *
     * CAVEAT:  at the moment the local rotation is overwritten with the rotation from euler_rot
     *          whenever gui() is called, this is mostly just for testing and is something that
     *          probably doesn't need to stay forever
     */

    class CameraComponent : public scene::Component {
      public:
        float fov = 1;

        [[nodiscard]] glm::vec3 forward() const;
        [[nodiscard]] glm::mat4 view() const;
        [[nodiscard]] glm::mat4 proj(glm::vec2 window_size) const;

        void gui();

      protected:
        glm::vec3 euler_rot {0, 0, 0};
        void onAttach() override;
    };

    class DitherSettingsComponent : public scene::Component {
      public:
        float threshold = 0.45;
        float pattern_angle = 0;
        bool dither_colour = true;
        glm::vec3 sky_colour {0, 0, 0};
        Dithers dither_pattern = Dithers::Ordered;

        void gui();
      protected:
        void onAttach() override;
    };
}
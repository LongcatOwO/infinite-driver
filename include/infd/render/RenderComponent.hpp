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

 class DirectionalLightComponent : public scene::Component {
  public:
     glm::vec3 direction {-0.0791808 -0.118771 -0.98976};

     void gui();

  protected:
     void onAttach() override;
 };
}
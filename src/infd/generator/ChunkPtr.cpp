#include <infd/generator/ChunkPtr.hpp>
#include <infd/generator/meshbuilding/PerlinMesh.hpp>
#include <infd/render/RenderComponent.hpp>
#include <infd/scene/physics/BvhTriangleMeshShape.hpp>
#include <infd/scene/physics/physics.hpp>
#include <infd/Wavefront.hpp>


#include <infd/debug/glm.hpp>
#include <iostream>

namespace infd::generator {
    ChunkPtr::ChunkPtr(scene::Component &parent, render::Renderer &renderer, ChunkGenerator &generator) :
            ChunkPtr(parent.sceneObject(), renderer, generator) {}

    ChunkPtr::ChunkPtr(scene::SceneObject& scene, render::Renderer& renderer, ChunkGenerator& generator) {
        scene::SceneObject& chunkSceneObject = scene.addChild((std::stringstream() << "Chunk: " << generator.x << ", "<< generator.y).str());

        auto [mesh, tri_mesh] = meshbuilding::generatePerlinMesh(generator.x, generator.y, generator.perlinNoise);

        auto& render_component = 
            chunkSceneObject.emplaceComponent<render::RenderComponent>(renderer, std::move(mesh));

        constexpr float adjust_scaling = 30;
        render_component.transform().localPosition(glm::vec3{generator.x, 0, generator.y} * adjust_scaling);
        render_component.transform().localScale(glm::vec3{adjust_scaling});

        chunkSceneObject.emplaceComponent<scene::physics::BvhTriangleMeshShape>(std::move(tri_mesh));
        chunkSceneObject.emplaceComponent<scene::physics::RigidBody>().mass(0);

        _chunkScenePointer = &chunkSceneObject;
    }

    void ChunkPtr::detach() {
        (void)_chunkScenePointer->removeFromParent();
        _detached = true;
    }
}

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

        render_component.transform().localPosition({generator.x, 0, generator.y});

        chunkSceneObject.emplaceComponent<scene::physics::BvhTriangleMeshShape>(std::move(tri_mesh));
        chunkSceneObject.emplaceComponent<scene::physics::RigidBody>().mass(0);

        _chunkScenePointer = &chunkSceneObject;
    }

    void ChunkPtr::detach() {
        (void)_chunkScenePointer->removeFromParent();
        _detached = true;
    }
}

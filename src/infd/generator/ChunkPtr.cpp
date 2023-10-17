#include <infd/generator/ChunkPtr.hpp>
#include <infd/generator/meshbuilding/PerlinMesh.hpp>
#include <infd/generator/meshbuilding/RoadMeshBuilder.hpp>
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

        auto [perlin_mesh, perlin_collision_mesh] = meshbuilding::generatePerlinMesh(generator.x, generator.y, generator.perlinNoise);
        auto [road_mesh, road_collision_mesh] = meshbuilding::RoadMeshBuilder(generator).build();

        chunkSceneObject.transform().localPosition({generator.x, 0, generator.y});

        chunkSceneObject.emplaceComponent<render::RenderComponent>(renderer, std::move(perlin_mesh));

        chunkSceneObject.emplaceComponent<scene::physics::BvhTriangleMeshShape>(std::move(perlin_collision_mesh));
        chunkSceneObject.emplaceComponent<scene::physics::RigidBody>().mass(0);

        auto& roadSceneObject = chunkSceneObject.addChild((std::stringstream() << "Roads: " << generator.x << ", "<< generator.y).str());

        auto& roadObj = roadSceneObject.emplaceComponent<render::RenderComponent>(renderer, std::move(road_mesh));

        roadObj.material.colour = glm::vec3(0.5);
        
        roadSceneObject.emplaceComponent<scene::physics::BvhTriangleMeshShape>(std::move(road_collision_mesh));
        roadSceneObject.emplaceComponent<scene::physics::RigidBody>().mass(0);

        _chunkScenePointer = &chunkSceneObject;
    }

    void ChunkPtr::detach() {
        (void)_chunkScenePointer->removeFromParent();
        _detached = true;
    }
}

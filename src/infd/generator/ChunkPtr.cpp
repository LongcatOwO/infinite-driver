#include <infd/generator/ChunkPtr.hpp>
#include <infd/generator/meshbuilding/PerlinMesh.hpp>
#include <infd/generator/meshbuilding/RoadMeshBuilder.hpp>
#include <infd/render/RenderComponent.hpp>
#include <infd/scene/physics/BvhTriangleMeshShape.hpp>
#include <infd/scene/physics/physics.hpp>
#include <infd/Wavefront.hpp>

#include <infd/debug/glm.hpp>
#include <iostream>
#include "infd/generator/meshbuilding/BuildingMeshBuilder.hpp"

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

        helpers::RandomType random(generator.seed);

        for (Clipper2Lib::PathD& path : generator.cycles) {
            auto [building_mesh, building_collision_mesh] = meshbuilding::BuildingMeshBuilder(generator, path, random).build();

            auto& buildingSceneObject = chunkSceneObject.addChild((std::stringstream() << "Building: " << &path).str());

            auto& buildingObj = roadSceneObject.emplaceComponent<render::RenderComponent>(renderer, std::move(building_mesh));

            buildingObj.material.colour = glm::vec3(buildingColourDist(random), buildingColourDist(random), buildingColourDist(random));

            if (building_collision_mesh->getNumTriangles() > 0) {
                buildingSceneObject.emplaceComponent<scene::physics::BvhTriangleMeshShape>(std::move(building_collision_mesh));
                buildingSceneObject.emplaceComponent<scene::physics::RigidBody>().mass(0);
            }
        }

        _chunkScenePointer = &chunkSceneObject;
    }

    void ChunkPtr::detach() {
        (void)_chunkScenePointer->removeFromParent();
        _detached = true;
    }
}

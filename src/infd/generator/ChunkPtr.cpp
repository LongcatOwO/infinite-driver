#include "infd/generator/ChunkPtr.hpp"

#include "infd/generator/meshbuilding/PerlinMesh.hpp"
#include "infd/render/RenderComponent.hpp"
#include "infd/Wavefront.hpp"

namespace infd::generator {
    ChunkPtr::ChunkPtr(scene::Component &parent, render::Renderer &renderer, ChunkGenerator &generator) :
            ChunkPtr(parent.sceneObject(), renderer, generator) {}

    ChunkPtr::ChunkPtr(scene::SceneObject& scene, render::Renderer& renderer, ChunkGenerator& generator) {
        scene::SceneObject& chunkSceneObject = scene.addChild((std::stringstream() << "Chunk: " << generator.x << ", "<< generator.y).str());

        auto& mesh = chunkSceneObject.emplaceComponent<render::RenderComponent>(
                renderer,
                meshbuilding::generatePerlinMesh(generator.x, generator.y, generator.perlinNoise)
        );

        mesh.transform().localPosition(glm::vec3(generator.x, 0, generator.y));

        _chunkScenePointer = &chunkSceneObject;
    }

    void ChunkPtr::detach() {
        (void)_chunkScenePointer->removeFromParent();
        _detached = true;
    }
}
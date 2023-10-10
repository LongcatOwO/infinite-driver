#pragma once

#include "ChunkGenerator.hpp"
#include "infd/scene/Scene.hpp"
#include "infd/render/Renderer.hpp"

namespace infd::generator {
    class ChunkPtr {
        bool _detached = false;

        scene::SceneObject* _chunkScenePointer;

    public:
        ChunkPtr(scene::SceneObject& scene, render::Renderer& renderer, ChunkGenerator& generator);
        ChunkPtr(scene::Component& parent, render::Renderer& renderer, ChunkGenerator& generator);

        void detach();
    };
}
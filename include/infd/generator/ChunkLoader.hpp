#pragma once

#include <vector>
#include <random>
#include <BulletCollision/CollisionShapes/btTriangleMesh.h>
#include <infd/scene/Scene.hpp>
#include <infd/render/Renderer.hpp>
#include "ChunkPtr.hpp"
#include "PerlinNoise.hpp"


namespace infd::generator {
class ChunkLoader : public infd::scene::Component {
    private:
        int _radius;

        int _x = 0;
        int _y = 0;

        int _xOffset = 0;
        int _yOffset = 0;

        int _diameter;
        unsigned int _seed;
        std::vector<ChunkPtr> _chunks;

        PerlinNoise _perlinNoise;

        render::Renderer& _renderer;

        void regenAll();

        void replace(int x, int y, int xOffset, int yOffset);

    public:
        ChunkLoader(scene::SceneObject& scene, render::Renderer& renderer, int radius, unsigned int seed = 0, int x = 0, int y = 0);
        ChunkPtr& operator()(int x, int y);
        void move(int x, int y);
        void center(float x, float y);
        void detachAll();
        void onFrameUpdate() override;
    };
}

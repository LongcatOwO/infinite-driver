#pragma once

#include <vector>
#include <random>
#include "ChunkPtr.hpp"
#include "PerlinNoise.hpp"

namespace infd::generator {
    class ChunkLoader {
    private:
        int _x = 0;
        int _y = 0;

        int _xOffset = 0;
        int _yOffset = 0;

        int _diameter;
        unsigned int _seed;
        std::vector<ChunkPtr> _chunks;

        PerlinNoise _perlinNoise;

        void regenAll();

        void replace(int x, int y, int xOffset, int yOffset);
    public:
        ChunkLoader(int radius, unsigned int seed = 0);
        ~ChunkLoader();
        ChunkPtr& operator()(int x, int y);
        void move(int x, int y);

        inline ChunkLoader(const ChunkLoader&) = delete;
    };
}

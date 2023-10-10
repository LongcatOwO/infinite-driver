#include "infd/generator/ChunkLoader.hpp"
#include "infd/generator/util/helpers.hpp"
#include "infd/generator/ChunkGenerator.hpp"

namespace infd::generator {
    ChunkLoader::ChunkLoader(int radius, unsigned int seed) {
        _perlinNoise = PerlinNoise(seed);
        _diameter = radius + radius + 1;
        _seed = seed;

        _chunks.reserve(_diameter * _diameter);
        for (int x = 0; x < _diameter; x++) {
            for (int y = 0; y < _diameter; y++) {
                //TODO: replace with call to ChunkGenerator build methods, which should return a ptr to use here :)
                ChunkGenerator generator(x+_x, y+_y, _seed, _perlinNoise);
                _chunks.emplace_back(generator);
            }
        }
    }

    void ChunkLoader::regenAll() {
        for (int x = 0; x < _diameter; x++) {
            for (int y = 0; y < _diameter; y++) {
                replace(x, y, _x, _y);
            }
        }

        _xOffset = 0;
        _yOffset = 0;
    }

    void ChunkLoader::replace(int x, int y, int xOffset, int yOffset) {
        ChunkGenerator generator(x+xOffset, y+yOffset, _seed, _perlinNoise);
        ChunkPtr& ptr = _chunks[x * _diameter + y];
        ptr.detach();
        ptr = ChunkPtr(generator);
    }

    ChunkPtr& ChunkLoader::operator()(int x, int y) {
        int dx = helpers::positiveModulo(x + _xOffset, _diameter);
        int dy = helpers::positiveModulo(y + _yOffset, _diameter);

        return _chunks[dx * _diameter + dy];
    }

    void ChunkLoader::move(int x, int y) {
        int dx = x - _x;
        int dy = y - _y;

        int xPrev = _x;
        int yPrev = _y;

        _x = x;
        _y = y;

        _xOffset += dx;
        _yOffset += dy;

        // If no _chunks can be preserved.
        if (abs(dx) >= _diameter || abs(dy) >= _diameter) {
            regenAll();
            return;
        }

        // Vertical rectangles.
        for (int ix = _diameter - dx; ix < _diameter; ix++) {
            for (int iy = 0; iy < _diameter; iy++) {
                replace(ix, iy, xPrev+dx, yPrev+dy);
            }
        }
        for (int ix = 0; ix < -dx; ix++) {
            for (int iy = 0; iy < _diameter; iy++) {
                replace(ix, iy, xPrev+dx, yPrev+dy);
            }
        }

        bool dxSign = dx > 0;

        // Horizontal rectangles.
        for (int ix = dxSign * -dx; ix < _diameter - dxSign * dx; ix++) {
            for (int iy = _diameter - dy; iy < _diameter; iy++) {
                replace(ix, iy, xPrev+dx, yPrev+dy);
            }
        }
        for (int ix = dxSign * -dx; ix < _diameter - dxSign * dx; ix++) {
            for (int iy = 0; iy < -dy; iy++) {
                replace(ix, iy, xPrev+dx, yPrev+dy);
            }
        }
    }

    ChunkLoader::~ChunkLoader() {
        // Clean up all children.
        for (ChunkPtr& ptr : _chunks) {
            ptr.detach();
        }
    }
}
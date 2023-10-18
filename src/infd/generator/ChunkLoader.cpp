#include "infd/generator/ChunkLoader.hpp"
#include "infd/generator/util/helpers.hpp"
#include "infd/generator/ChunkGenerator.hpp"

namespace infd::generator {
    ChunkLoader::ChunkLoader(scene::SceneObject& scene, render::Renderer& renderer, int radius, unsigned int seed, int x, int y) :
        _radius(radius), _x(x-radius), _y(y-radius), _diameter(radius+radius+1), _seed(seed), _perlinNoise(PerlinNoise(seed)), _renderer(renderer)
    {
        _chunks.reserve(_diameter * _diameter);
        for (int x = 0; x < _diameter; x++) {
            for (int y = 0; y < _diameter; y++) {
                ChunkGenerator generator(x+_x, y+_y, _seed, _perlinNoise);
                _chunks.emplace_back(scene, _renderer, generator);
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
        ChunkPtr& ptr = operator()(x , y);
        ptr.detach();
        ptr = ChunkPtr(*this, _renderer, generator);
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

    void ChunkLoader::detachAll() {
        for (ChunkPtr& ptr : _chunks) {
            ptr.detach();
        }
    }

    void ChunkLoader::center(float x, float y) {
        int xTransform = std::floor(x) - _radius;
        int yTransform = std::floor(y) - _radius;
        if (xTransform != _x || yTransform != y) {
            move(xTransform, yTransform);
        }
    }

    void ChunkLoader::onFrameUpdate() {
        glm::vec3 cameraPosition = _renderer._camera->transform().globalPosition();
        glm::vec3 scale = transform().localScale();
        center(cameraPosition.x / scale.x, cameraPosition.z / scale.z);
    }
}
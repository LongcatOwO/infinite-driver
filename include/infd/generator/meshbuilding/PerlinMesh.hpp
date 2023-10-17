#pragma once

// std
#include <memory>
#include <vector>

// glm
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

// bullet
#include <BulletCollision/CollisionShapes/btTriangleMesh.h>

// project
#include <infd/GLMesh.hpp>

// project - generator
#include <infd/generator/ChunkGenerator.hpp>
#include <infd/generator/PerlinNoise.hpp>
#include <infd/generator/meshbuilding/Triangle.hpp>

// project - math
#include <infd/math/glm_bullet.hpp>


namespace infd::generator::meshbuilding {
    /**
     * Constructs a unit mesh based off the values from the given perlin noise
     */
    inline auto generatePerlinMesh(int offsetX, int offsetY, PerlinNoise& noise, unsigned int subdivisions = 20) {
        GLMeshBuilder meshBuilder;

        float subdivisionSize = 1.f/static_cast<float>(subdivisions);

        unsigned int index = 0;

        std::unique_ptr<btTriangleMesh> tri_mesh{new btTriangleMesh()};

        for (unsigned int x = 0; x < subdivisions; x++) {
            for (unsigned int y = 0; y < subdivisions; y++) {
                float relX = x * subdivisionSize;
                float relY = y * subdivisionSize;
                float noiseX = relX + offsetX;
                float noiseY = relY + offsetY;

                glm::vec3 xy1 = glm::vec3(
                        relX,
                        PERLIN_TERRAIN_FACTOR * ChunkGenerator::scaledPerlin(noiseX, noiseY, noise),
                        relY
                );
                glm::vec3 xy2 = glm::vec3(
                        relX+subdivisionSize,
                        PERLIN_TERRAIN_FACTOR * ChunkGenerator::scaledPerlin(noiseX+subdivisionSize, noiseY, noise),
                        relY
                );
                glm::vec3 xy3 = glm::vec3(
                        relX,
                        PERLIN_TERRAIN_FACTOR * ChunkGenerator::scaledPerlin(noiseX, noiseY+subdivisionSize, noise),
                        relY+subdivisionSize
                );
                glm::vec3 xy4 = glm::vec3(
                        relX+subdivisionSize,
                        PERLIN_TERRAIN_FACTOR * ChunkGenerator::scaledPerlin(noiseX+subdivisionSize, noiseY+subdivisionSize, noise),
                        relY+subdivisionSize
                );

                Triangle t1(xy3, xy2, xy1);
                Triangle t2(xy2, xy3, xy4);

                t1.addToMesh(meshBuilder, index);
                t2.addToMesh(meshBuilder, index);

                t1.addToCollision(*tri_mesh);
                t2.addToCollision(*tri_mesh);
            }
        }

        struct Result {
            GLMesh mesh;
            std::unique_ptr<btTriangleMesh> tri_mesh;
        };

        return Result{meshBuilder.build(), std::move(tri_mesh)};
    }
}

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

// project - math
#include <infd/math/glm_bullet.hpp>


namespace infd::generator::meshbuilding {
    inline void generateBasicTri(GLMeshBuilder& mb, btTriangleMesh& tri_mesh, std::vector<glm::vec3> nodes, unsigned int& index) {
        MeshVertex a, b, c;

        a.pos = nodes.at(0);
        b.pos = nodes.at(1);
        c.pos = nodes.at(2);

        glm::vec3 normal = glm::normalize(glm::cross(a.pos - b.pos, a.pos - c.pos));
        a.norm = normal;
        b.norm = normal;
        c.norm = normal;

        mb.vertices.push_back(a);
        mb.vertices.push_back(b);
        mb.vertices.push_back(c);

        mb.indices.push_back(index++);
        mb.indices.push_back(index++);
        mb.indices.push_back(index++);

        tri_mesh.addTriangle(math::toBullet(a.pos), math::toBullet(b.pos), math::toBullet(c.pos));
    }

    /**
     * Constructs a unit mesh based off the values from the given perlin noise
     */
    inline auto generatePerlinMesh(int offsetX, int offsetY, PerlinNoise& noise, unsigned int subdivisions = 5) {
        GLMeshBuilder meshBuilder;

        float scale = 1;

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
                        subdivisionSize * ChunkGenerator::scaledPerlin(noiseX, noiseY, noise),
                        relY
                ) * scale;
                glm::vec3 xy2 = glm::vec3(
                        relX+subdivisionSize,
                        subdivisionSize * ChunkGenerator::scaledPerlin(noiseX+subdivisionSize, noiseY, noise),
                        relY
                ) * scale;
                glm::vec3 xy3 = glm::vec3(
                        relX,
                        subdivisionSize * ChunkGenerator::scaledPerlin(noiseX, noiseY+subdivisionSize, noise),
                        relY+subdivisionSize
                ) * scale;
                glm::vec3 xy4 = glm::vec3(
                        relX+subdivisionSize,
                        subdivisionSize * ChunkGenerator::scaledPerlin(noiseX+subdivisionSize, noiseY+subdivisionSize, noise),
                        relY+subdivisionSize
                ) * scale;

                generateBasicTri(meshBuilder, *tri_mesh, {xy3, xy2, xy1}, index);
                generateBasicTri(meshBuilder, *tri_mesh, {xy2, xy3, xy4}, index);
            }
        }

        struct Result {
            GLMesh mesh;
            std::unique_ptr<btTriangleMesh> tri_mesh;
        };

        return Result{meshBuilder.build(), std::move(tri_mesh)};
    }
}

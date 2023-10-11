#pragma once

namespace infd::generator::meshbuilding {
    inline void generateBasicTri(GLMeshBuilder& mb, std::vector<glm::vec3> nodes, unsigned int& index) {
        MeshVertex a, b, c;

        auto itt = nodes.begin();
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
    }

    /**
     * Constructs a unit mesh based off the values from the given perlin noise
     */
    inline GLMesh generatePerlinMesh(int offsetX, int offsetY, PerlinNoise& noise, unsigned int subdivisions = 5) {
        GLMeshBuilder meshBuilder;

        float scale = 1;

        float subdivisionSize = 1.f/static_cast<float>(subdivisions);

        unsigned int index = 0;

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

                generateBasicTri(meshBuilder, {xy3, xy2, xy1}, index);
                generateBasicTri(meshBuilder, {xy2, xy3, xy4}, index);
            }
        }
        return meshBuilder.build();
    }
}
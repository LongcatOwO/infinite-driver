#pragma once

#include "infd/GLMesh.hpp"
#include "BulletCollision/CollisionShapes/btTriangleMesh.h"
#include "infd/generator/PerlinNoise.hpp"
#include "infd/generator/ChunkGenerator.hpp"
#include "Polygon.hpp"

namespace infd::generator::meshbuilding {
    using namespace Clipper2Lib;

    class BuildingMeshBuilder {
        struct Result {
            GLMesh mesh;
            std::unique_ptr<btTriangleMesh> tri_mesh;
        };

        float x;
        float y;

        GLMeshBuilder mb;
        unsigned int index = 0;

        helpers::RandomType& random;

        std::unique_ptr<btTriangleMesh> tri_mesh{new btTriangleMesh()};

        PerlinNoise& noise;
        PathD originPath;

        constexpr static const float scaleFactor = 1000.f;

        void processHull(PathD& path, float height, float depth);
        void processRoof(PathD& path, float height);
        void processWalls(PathD& path, float height, float depth);

        // as known as a "commie block". very simple
        void generateKhrushchevka(PathD basis, float floorHeight);

        void generateSkyscraper(PathD& basis, float floorHeight, float min_radius, PointD& center);

        glm::vec2 findHeightBounds(PathD& path);

        static PathD unionPath(PathD& a, PathD& b);

        void drawCollidingTriangle(Triangle &tri);
        void drawTriangle(Triangle &tri);

        static PathD removeVertices(PathD& source, unsigned int count, unsigned int index);
        static PathD& scalePath(PathD& path, float factor);
        static PathD shrinkPath(PathD& path, float factor);
        static auto findRadiusBounds(PathD& path);

        static PathD generatePolygon(float radius, unsigned int sides, const PointD& origin = PointD(0,0));
    public:
        BuildingMeshBuilder(ChunkGenerator &generator, PathD& path, helpers::RandomType& random);
        [[nodiscard]] BuildingMeshBuilder::Result build();
    };
}
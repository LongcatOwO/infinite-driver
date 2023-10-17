#pragma once

#include <infd/generator/Node.hpp>
#include "infd/GLMesh.hpp"
#include "infd/generator/PerlinNoise.hpp"
#include "infd/generator/ChunkGenerator.hpp"
#include "Polygon.hpp"

namespace infd::generator::meshbuilding {
    class RoadMeshBuilder {
        struct Result {
            GLMesh mesh;
            std::unique_ptr<btTriangleMesh> tri_mesh;
        };

        struct Offset {
            glm::vec2 cull;
            float tangent;
            float midAngle;
        };

        float x;
        float y;

        GLMeshBuilder mb;
        unsigned int index = 0;
        std::unique_ptr<btTriangleMesh> tri_mesh{new btTriangleMesh()};

        PerlinNoise& noise;
        std::vector<std::shared_ptr<Node>>& nodes;

        void generateNode(Node& node);
        void generateIntersection(Node& node);
        void generateSegment(Edge& edge, float offset);
        void processTriangle(Triangle& triangle);
        void processIntersectionWall(p2t::Point& a, p2t::Point& b, float height, Node& node);

        void drawTriangle(Triangle& tri);
        void drawCollidingTriangle(Triangle& tri);

        static void emplaceOffset(Edge& a, Edge& b, float angle, std::vector<Offset>& offsets);
        static void emplaceVertex(Offset& a, Offset& b, Edge& edge, Polygon& output);
        static float calculateTangent(float angle);
        static glm::vec2 calculateIntersection(float basisAngle, float halfAngle, bool flipAxis = false);

    public:
        RoadMeshBuilder(ChunkGenerator& generator);
        [[nodiscard]] RoadMeshBuilder::Result build();
    };
}

#include <infd/generator/meshbuilding/RoadMeshBuilder.hpp>
#include "infd/generator/meshbuilding/Triangle.hpp"
#include "infd/generator/meshbuilding/Polygon.hpp"

#include "infd/generator/ChunkGenerator.hpp"
#include "infd/util/Function.hpp"

#include <poly2tri/poly2tri.h>


namespace infd::generator::meshbuilding {

    RoadMeshBuilder::RoadMeshBuilder(ChunkGenerator& generator) :
        x(generator.x), y(generator.y), noise(generator.perlinNoise), nodes(generator.nodes) {}

    RoadMeshBuilder::Result RoadMeshBuilder::build() {
        index = 0;

        for (std::shared_ptr<Node>& node : nodes) {
            generateNode(*node);
        }

        return Result{mb.build(), std::move(tri_mesh)};
    }

    void RoadMeshBuilder::generateNode(Node &node) {
        //If it only has one edge, no need to generate the adaptive join geometry.
        if (node.neighbours.size() == 1) {
            generateSegment(node.neighbours[0], 0);
            return;
        }

        generateIntersection(node);
    }

    void RoadMeshBuilder::generateIntersection(Node &node) {
        std::vector<Offset> offsets;
        std::vector<Edge>& edges = node.neighbours;

        for (unsigned int i = 0; i < edges.size()-1; i++) {
            emplaceOffset(edges[i], edges[i+1], edges[i+1].angle - edges[i].angle, offsets);
        }
        emplaceOffset(
                edges.back(),
                edges.front(),
                edges.front().angle - edges.back().angle + glm::two_pi<float>(),
                offsets
        );

        for (unsigned int i = 1; i < offsets.size(); i++) {
            generateSegment(edges[i], std::max(offsets[i].tangent, offsets[i-1].tangent));
        }
        generateSegment(edges.front(), std::max(offsets.back().tangent, offsets.front().tangent));

        Polygon output;

        for (unsigned int i = 0; i < edges.size()-1; i++) {
            emplaceVertex(offsets[i], offsets[i+1], edges[i+1], output);
        }
        emplaceVertex(offsets.back(), offsets.front(), edges.front(), output);

        float height = PERLIN_TERRAIN_FACTOR * ChunkGenerator::scaledPerlin(node.x+x, node.y+y, noise) + ROAD_HEIGHT;

        for (unsigned int i = 0; i < output.points.size()-1; i++) {
            processIntersectionWall(output.points[i], output.points[i+1], height, node);
        }
        processIntersectionWall(output.points.back(), output.points.front(), height, node);

        std::vector<p2t::Triangle*> mesh = output.triangulate();

        for (p2t::Triangle* tri : mesh) {
            Triangle t = Triangle::convertTo(*tri, glm::vec3(height), glm::vec2(node.x, node.y));
            drawCollidingTriangle(t);
        }
    }

    void RoadMeshBuilder::generateSegment(Edge &edge, float offset) {
        Node& to = *edge.to;
        Node& from = *edge.from;

        glm::vec2 midPoint(
                (from.x + to.x)/2,
                (from.y + to.y)/2
        );

        glm::vec2 basis(
                from.x + sin(edge.angle) * offset,
                from.y + cos(edge.angle) * offset
        );

        float basisHeight = PERLIN_TERRAIN_FACTOR * ChunkGenerator::scaledPerlin(from.x+x, from.y+y, noise) + ROAD_HEIGHT;
        float midPointHeight = PERLIN_TERRAIN_FACTOR * ChunkGenerator::scaledPerlin(midPoint.x+x, midPoint.y+y, noise) + ROAD_HEIGHT;

        glm::vec3 xy1(
                basis.x + sin(edge.angle+glm::half_pi<float>())*ROAD_WIDTH,
                basisHeight,
                basis.y + cos(edge.angle+glm::half_pi<float>())*ROAD_WIDTH
        );
        glm::vec3 xy2(
                basis.x + sin(edge.angle-glm::half_pi<float>())*ROAD_WIDTH,
                basisHeight,
                basis.y + cos(edge.angle-glm::half_pi<float>())*ROAD_WIDTH
        );
        glm::vec3 xy3(
                midPoint.x + sin(edge.angle-glm::half_pi<float>())*ROAD_WIDTH,
                midPointHeight,
                midPoint.y + cos(edge.angle-glm::half_pi<float>())*ROAD_WIDTH
        );
        glm::vec3 xy4(
                midPoint.x + sin(edge.angle+glm::half_pi<float>())*ROAD_WIDTH,
                midPointHeight,
                midPoint.y + cos(edge.angle+glm::half_pi<float>())*ROAD_WIDTH
        );

        auto drawCollisionFunc = util::BindedMemberFunc(&RoadMeshBuilder::drawCollidingTriangle, *this);
        auto drawFunc = util::BindedMemberFunc(&RoadMeshBuilder::drawTriangle, *this);

        processQuad(xy1, xy2, xy3, xy4, drawCollisionFunc);

        processVerticalWall(xy2, xy3, -2*ROAD_HEIGHT, drawFunc);
        processVerticalWall(xy4, xy1, -2*ROAD_HEIGHT, drawFunc);
    }

    glm::vec2 RoadMeshBuilder::calculateIntersection(float basisAngle, float halfAngle, bool flipAxis) {
        float diagonal = ROAD_WIDTH / sin(halfAngle * (1 - (flipAxis * 2)));
        return {
                sin(basisAngle+halfAngle)*diagonal,
                cos(basisAngle+halfAngle)*diagonal
        };
    }

    float RoadMeshBuilder::calculateTangent(float angle) {
        return ROAD_WIDTH / tan(angle);
    }

    void RoadMeshBuilder::emplaceOffset(Edge &a, Edge &b, float angle, std::vector<Offset> &offsets) {
        float midAngle = angle/2;

        offsets.emplace_back(
                calculateIntersection(a.angle, midAngle),
                calculateTangent(midAngle),
                midAngle
        );
    }

    void RoadMeshBuilder::emplaceVertex(RoadMeshBuilder::Offset &a, RoadMeshBuilder::Offset &b, Edge &edge, Polygon& output) {
        glm::vec2 p;
        if (a.tangent > b.tangent) {
            p = calculateIntersection(edge.angle, a.midAngle);
        } else {
            p = calculateIntersection(edge.angle, -b.midAngle, true);
        }

        output.addPoint(a.cull);
        output.addPoint(p);
    }

    void RoadMeshBuilder::processTriangle(Triangle &triangle) {
        triangle.addToMesh(mb, index);
    }

    void RoadMeshBuilder::processIntersectionWall(p2t::Point &a, p2t::Point &b, float height,  Node &node) {
        auto drawFunc = util::BindedMemberFunc(&RoadMeshBuilder::drawTriangle, *this);
        processVerticalWall(glm::vec3(a.x+node.x, height, a.y+node.y),
                            glm::vec3(b.x+node.x, height, b.y+node.y), -2*ROAD_HEIGHT, drawFunc);
    }

    void RoadMeshBuilder::drawTriangle(Triangle &tri) {
        tri.addToMesh(mb, index);
    }

    void RoadMeshBuilder::drawCollidingTriangle(Triangle &tri) {
        tri.addToMesh(mb, index);
        tri.addToCollision(*tri_mesh);
    }
}
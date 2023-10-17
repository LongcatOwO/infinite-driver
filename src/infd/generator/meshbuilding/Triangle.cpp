#include <infd/generator/meshbuilding/Triangle.hpp>
#include <iostream>

#include "glm/vec3.hpp"
#include "glm/geometric.hpp"
#include "infd/math/glm_bullet.hpp"
#include "poly2tri/poly2tri.h"

namespace infd::generator::meshbuilding {
    Triangle::Triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c) :
        a(a),
        b(b),
        c(c),
        norm(glm::normalize(glm::cross(a - b, a - c))) {}

    void Triangle::addToMesh(GLMeshBuilder &mb, unsigned int &index) {
        mb.vertices.emplace_back(a, norm);
        mb.vertices.emplace_back(b, norm);
        mb.vertices.emplace_back(c, norm);

        mb.indices.push_back(index++);
        mb.indices.push_back(index++);
        mb.indices.push_back(index++);
    }

    void Triangle::addToCollision(btTriangleMesh &tri_mesh) {
        tri_mesh.addTriangle(
                math::toBullet(a),
                math::toBullet(b),
                math::toBullet(c)
        );
    }

    Triangle Triangle::convertTo(p2t::Triangle &tri, glm::vec3 yValues, glm::vec2 pos) {
        // p2t::Triangle winding is cw, Triangle is ccw, therefore reversed indices
        return {
                glm::vec3(tri.GetPoint(2)->x + pos.x, yValues.x, tri.GetPoint(2)->y + pos.y),
                glm::vec3(tri.GetPoint(1)->x + pos.x, yValues.y, tri.GetPoint(1)->y + pos.y),
                glm::vec3(tri.GetPoint(0)->x + pos.x, yValues.z, tri.GetPoint(0)->y + pos.y)
        };
    }
}
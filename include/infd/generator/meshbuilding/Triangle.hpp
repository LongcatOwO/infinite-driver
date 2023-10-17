#pragma once

#include <functional>
#include "glm/vec3.hpp"
#include "infd/GLMesh.hpp"
#include "BulletCollision/CollisionShapes/btTriangleMesh.h"
#include "poly2tri/common/shapes.h"

namespace infd::generator::meshbuilding {
    struct Triangle {
        glm::vec3 a;
        glm::vec3 b;
        glm::vec3 c;
        glm::vec3 norm;

        Triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c);

        static Triangle convertTo(p2t::Triangle& tri, glm::vec3 yValues = glm::vec3(0), glm::vec2 pos = glm::vec2(0));

        void addToMesh(GLMeshBuilder& mb, unsigned int& index);
        void addToCollision(btTriangleMesh& tri_mesh);
    };

    /**
     * Build and processes a simple quad. Points are assumed to be ordered ccw
     */
    template <typename Fn>
    inline void processQuad(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& d, Fn&& func) {
        Triangle t1(a,b,c);
        Triangle t2(a,c,d);
        func(t1);
        func(t2);
    }

    template <typename Fn>
    inline void processVerticalWall(const glm::vec3& a, const glm::vec3& b, float height, Fn&& func) {
        glm::vec3 verticalOffset(0, height, 0);
        processQuad(a, b, b+verticalOffset, a+verticalOffset, func);
    }
}
#pragma once

#include <vector>
#include <memory>
#include "poly2tri/poly2tri.h"
#include "glm/vec2.hpp"
#include "Triangle.hpp"

namespace infd::generator::meshbuilding {
    class Polygon {
        std::unique_ptr<p2t::CDT> polygon = nullptr;
    public:
        std::vector<p2t::Point> points;
        void addPoint(glm::vec2& point);
        std::vector<p2t::Triangle*> triangulate();
    };
}

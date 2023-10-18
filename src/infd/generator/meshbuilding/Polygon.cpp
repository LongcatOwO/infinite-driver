#include <infd/generator/meshbuilding/Polygon.hpp>
#include "poly2tri/poly2tri.h"

namespace infd::generator::meshbuilding {

    const double EPSILON = 1e-12;

    void Polygon::addPoint(glm::vec2& point) {
        double x = point.x;
        double y = point.y;

        if (!points.empty()) {
            p2t::Point& last = points.back();

            //if the point is too close to the last point it will cause errors, skip this vertex.
            if (std::abs(last.x - x) < EPSILON || std::abs(last.y - y) < EPSILON) {
                return;
            }
        }

        points.emplace_back(x, y);
    }

    std::vector<p2t::Triangle*> Polygon::triangulate() {
        std::vector<p2t::Point*> pointers;
        pointers.reserve(points.size());

        for (p2t::Point& point : points) {
            pointers.push_back(&point);
        }

        polygon = std::make_unique<p2t::CDT>(pointers);

        polygon->Triangulate();
        return polygon->GetTriangles();
    }
}
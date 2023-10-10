#include "infd/generator/Edge.hpp"
#include "infd/generator/Node.hpp"

#include <cmath>
#include <iostream>

namespace infd::generator {
    Edge::Edge(generator::Node* from, generator::Node* to) : from(from), to(to) {
        angle = std::atan2(to->x - from->x, to->y - from->y);
    }

    bool Edge::operator<(Edge &other) const {
        return angle < other.angle;
    }

    bool Edge::operator<(const Edge &other) const {
        return angle < other.angle;
    }
}
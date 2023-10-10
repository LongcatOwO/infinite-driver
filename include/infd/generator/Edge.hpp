#pragma once

#include <infd/generator/fwd/Node.hpp>

namespace infd::generator {
    class Edge {
    public:
        Node *from;
        Node *to;
        float angle;
        bool visited = false;

        Edge(Node* from, Node* to);
        bool operator<(Edge& other) const;
        bool operator<(const Edge& other) const;
    };
}
#pragma once

#include "infd/generator/Edge.hpp"
#include <vector>
#include <memory>

namespace infd::generator {
    class Node {
    public:
        // Basic Node fields
        float x;
        float y;
        float angle;
        bool isRoot;

        unsigned int depth;
        unsigned int category;

        std::vector<Edge> neighbours;

        Node(float x, float y, float angle, bool isRoot = true, unsigned int depth = 1, unsigned int category = 0);
        Node* findNearest(Node* initial, std::vector<std::shared_ptr<Node>> &pool) const;

        void addNeighbour(Node* neighbour);
        void removeNeighbour(Node* neighbour);
        void sortNeighbours();
        Edge& nextNeighbour(float angle);

        float sqDist(Node& other) const;
    };
}

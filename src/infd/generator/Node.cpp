#include "infd/generator/Node.hpp"
#include "infd/generator/ChunkGenerator.hpp"

#include <algorithm>
#include <memory>
#include <iostream>

namespace infd::generator {
    float Node::sqDist(Node &other) const {
        return (x - other.x) * (x - other.x) + (y - other.y) * (y - other.y);
    }

    Node::Node(float x, float y, float angle, bool isRoot, unsigned int depth, unsigned int category) : x(x), y(y), angle(angle),
                                                                                 isRoot(isRoot), depth(depth), category(std::min(category, MAX_DEPTH)) {}

    Node* Node::findNearest(Node* initial, std::vector<std::shared_ptr<Node>> &pool) const {
        Node* nearest = initial;
        float dist = sqDist(*nearest);

        for (std::shared_ptr<Node>& node: pool) {
            float newDist = sqDist(*node);
            if (newDist < dist) {
                nearest = node.get();
                dist = newDist;
            }
        }

        return nearest;
    }

    void Node::addNeighbour(Node* neighbour) {
        neighbours.emplace_back(this, neighbour);
    }

    void Node::sortNeighbours() {
        std::sort(neighbours.begin(), neighbours.end());
    }

    void Node::removeNeighbour(Node* node) {
        neighbours.erase(
                std::remove_if(
                        neighbours.begin(),
                        neighbours.end(),
                        [&node](Edge &other) { return other.to == node; }
                ),
                neighbours.end()
        );
    }

    Edge& Node::nextNeighbour(float _angle) {
        for (Edge& edge : neighbours) {
            if (edge.angle > _angle) {
                return edge;
            }
        }
        return neighbours.front();
    }
}
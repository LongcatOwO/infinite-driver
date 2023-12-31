#include <deque>
#include <unordered_set>
#include "infd/generator/ChunkGenerator.hpp"
#include "infd/generator/util/helpers.hpp"
#include "infd/generator/PerlinNoise.hpp"

namespace infd::generator {
    void ChunkGenerator::populateRoots(std::vector<std::shared_ptr<Node>> &roots) {
        auto x_ = static_cast<float>(x);
        auto y_ = static_cast<float>(y);

        float centerHighwayFactor = scaledPerlin(x_, y_,perlinNoise);
        float upHighwayFactor = scaledPerlin(x_, y_-1,perlinNoise);
        float downHighwayFactor = scaledPerlin(x_, y_+1,perlinNoise);
        float leftHighwayFactor = scaledPerlin(x_-1, y_,perlinNoise);
        float rightHighwayFactor = scaledPerlin(x_+1, y_,perlinNoise);

        int upRoads = static_cast<int>(std::lround(rootDistribution((upHighwayFactor + centerHighwayFactor) / 2)));
        int downRoads = static_cast<int>(std::lround(rootDistribution((downHighwayFactor + centerHighwayFactor) / 2)));
        int leftRoads = static_cast<int>(std::lround(rootDistribution((leftHighwayFactor + centerHighwayFactor) / 2)));
        int rightRoads = static_cast<int>(std::lround(rootDistribution((rightHighwayFactor + centerHighwayFactor) / 2)));

        roots.reserve(upRoads + downRoads + leftRoads + rightRoads);

        populateRoots(roots, x_, y_-1, 5, upRoads, 1, [](float f){ return f; }, [](float){ return 0.f; });
        populateRoots(roots, x_, y_, 5, downRoads, 3, [](float f){ return f; }, [](float){ return 1.f; });
        populateRoots(roots, x_-1, y_, 7, leftRoads, 0, [](float){ return 0.f; }, [](float f){ return f; });
        populateRoots(roots, x_, y_, 7, rightRoads, 2, [](float){ return 1.f; }, [](float f){ return f; });
    }

    void ChunkGenerator::populateRoots(std::vector<std::shared_ptr<Node>> &roots, float x_, float y_, unsigned int seed, size_t roadCount,
                                       float angleMultiple,
                                       float (*assignX)(float), float (*assignY)(float)) {
        auto random = helpers::generateRandom<helpers::RandomType>(x_, y_, seed);
        std::vector<float> upFloats = helpers::paddedDistribution(roadCount, BORDER_ROOT_PADDING, BORDER_PADDING, random);
        for (float f : upFloats) {
            float x_ = assignX(f);
            float y_ = assignY(f);
            float angle = angleMultiple*glm::half_pi<float>() + angleDist(random);
            roots.push_back(std::make_shared<Node>(x_, y_, angle));
        }
    }

    ChunkGenerator::ChunkGenerator(int x, int y, unsigned int seed, PerlinNoise &perlinNoise) : x(x), y(y), seed(seed), perlinNoise(perlinNoise) {
        std::vector<std::shared_ptr<Node>> roots;
        populateRoots(roots);
        generateNetwork(roots, GENERATION_DEPTH);
        trimNetwork();
        sortEdges();
        findCycles();
    }

    float ChunkGenerator::rootDistribution(float value) {
        return MAX_BORDER_ROOTS * value * value * value * (4 - 3 * value);
    }

    void ChunkGenerator::generateNetwork(std::vector<std::shared_ptr<Node>>& roots, unsigned int depth) {
        helpers::RandomType random(seed);

        nodes = roots;
        std::deque<Node*> nodeQueue;
        for (std::shared_ptr<Node>& node : nodes) {
            nodeQueue.push_front(node.get());
        }

        while (!nodeQueue.empty()) {
            Node* node = nodeQueue.back();
            nodeQueue.pop_back();

            if (node->depth > depth) continue;

            if (probabilityDist(random) < BRANCH_ROAD_CHANCE && node->neighbours.size() < MAX_NEIGHBOURS) {
                addNode(node, nodeQueue, random, -glm::half_pi<float>(), 1);
            }

            if (node->neighbours.size() < MAX_NEIGHBOURS) {
                addNode(node, nodeQueue, random, 0.f, 0);
            }

            if (probabilityDist(random) < BRANCH_ROAD_CHANCE && node->neighbours.size() < MAX_NEIGHBOURS) {
                addNode(node, nodeQueue, random, glm::half_pi<float>(), 1);
            }
        }
    }

    void ChunkGenerator::addNode(Node* parent, std::deque<Node*> &nodeQueue, helpers::RandomType random, float angleOffset, unsigned int offset = 0) {
        float angle = parent->angle + angleDist(random) + angleOffset;

        float x_ = parent->x + ROAD_LENGTH * cosf(angle);
        float y_ = parent->y + ROAD_LENGTH * sinf(angle);

        if (x_ < 0 || x_ > 1 || y_ < 0 || y_ > 1) {
            return;
        }

        Node neighbour(x_, y_, angle, false, parent->depth+1, parent->depth+offset);

        Node* nearest = neighbour.findNearest(parent, nodes);

        if (nearest != parent) {
            for (Edge& edge : parent->neighbours) {
                if (edge.to == nearest) {
                    return;
                }
            }

            parent->addNeighbour(nearest);
            nearest->addNeighbour( parent);
            return;
        }

        nodes.push_back(std::make_shared<Node>(neighbour));
        nodeQueue.push_front(nodes.back().get());

        parent->addNeighbour(nodes.back().get());
        nodes.back()->addNeighbour(parent);
    }

    void ChunkGenerator::trimNetwork() {
        std::unordered_set<Node*> trimUnique;

        for (std::shared_ptr<Node>& node : nodes) {
            if (node->neighbours.size() <= 1) {
                trimUnique.insert(node.get());
            }
        }

        while (!trimUnique.empty()) {
            Node* node = *trimUnique.begin();
            trimUnique.erase(node);

            if (!node->isRoot && node->neighbours.size() <= 1) {
                for (Edge& edge : node->neighbours) {
                    trimUnique.insert(edge.to);
                    edge.to->removeNeighbour(node);
                }

                nodes.erase(
                        std::remove_if(
                                nodes.begin(),
                                nodes.end(),
                                [&node](std::shared_ptr<Node>& other){ return other.get() == node;}
                        ),
                        nodes.end()
                );
            }
        }
    }

    void ChunkGenerator::sortEdges() {
        for (std::shared_ptr<Node>& node : nodes) {
            node->sortNeighbours();
        }
    }

    void ChunkGenerator::findCycles() {
        using namespace Clipper2Lib;
        for (std::shared_ptr<Node>& node : nodes) {
            for (Edge& edge : node->neighbours) {
                if (edge.visited) continue;
                edge.visited = true;

                PathD cycle;
                cycle.emplace_back(node->x, node->y);

                Node* initial = node.get();

                Node* from = initial;
                Node* to = edge.to;

                while (to != initial) {
                    float angle = std::atan2(from->x - to->x, from->y - to->y);
                    cycle.emplace_back(to->x, to->y);

                    from = to;
                    Edge& nextEdge = to->nextNeighbour(angle);

                    nextEdge.visited = true;
                    to = nextEdge.to;
                }

                if (cycle.size() < MIN_CYCLE || cycle.size() > MAX_CYCLE) continue;

                cycles.push_back(cycle);
            }
        }
    }

    float ChunkGenerator::scaledPerlin(float x, float y, PerlinNoise &noise) {
        return (noise.sample<float>(x, y, generator::PERLIN_DEPTH) + 0.25f) * 2.f;
    }
}

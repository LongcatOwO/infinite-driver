#pragma once

#include "Node.hpp"
#include "PerlinNoise.hpp"
#include "glm/gtc/constants.hpp"
#include <random>
#include <deque>

#include <clipper2/clipper.h>

namespace infd::generator {
    static const float ROAD_LENGTH = 0.05f;
    static const float ROAD_WIDTH = 0.005f;
    static const float ROAD_HEIGHT = 0.001f;
    static const float PERLIN_DEPTH = 1.f;
    static const float MAX_BORDER_ROOTS = 3.2f;
    static const float BORDER_ROOT_PADDING = ROAD_LENGTH * 2;
    static const float BORDER_ROOT_ANGLE = glm::half_pi<float>() / 32;
    static const float BORDER_PADDING = 0.05f;

    static const float PERLIN_TERRAIN_FACTOR = 0.03f;

    //TODO: replace with dynamic based on perlin noise?
    static const unsigned int GENERATION_DEPTH = 25;

    static const unsigned int MAX_CYCLE = 8;
    static const unsigned int MIN_CYCLE = 3;

    static const unsigned int MAX_NEIGHBOURS = 4;
    static const float BRANCH_ROAD_CHANCE = 0.15f;

    static std::uniform_real_distribution<float> angleDist(-BORDER_ROOT_ANGLE, BORDER_ROOT_ANGLE);
    static std::uniform_real_distribution<float> probabilityDist(0, 1);

    class ChunkGenerator {
    public:
        ChunkGenerator(int x, int y, unsigned int seed, PerlinNoise& perlinNoise);

        unsigned int seed;

        int x;
        int y;
        PerlinNoise& perlinNoise;

        std::vector<std::shared_ptr<Node>> nodes;
        std::vector<Clipper2Lib::PathD> cycles;
        static float scaledPerlin(float x, float y, PerlinNoise& noise);
    private:
        static float rootDistribution(float value);

        void populateRoots(std::vector<std::shared_ptr<Node>>& roots);
        void generateNetwork(std::vector<std::shared_ptr<Node>>& roots, unsigned int depth);
        void trimNetwork();
        void sortEdges();
        void findCycles();

        void addNode(Node* parent, std::deque<Node*>& nodeQueue, helpers::RandomType random, float angleOffset);

        static void populateRoots(std::vector<std::shared_ptr<Node>> &roots, float x, float y, unsigned int seed, size_t roadCount,
                           float angleMultiple,
                           float (*assignX)(float), float (*assignY)(float));
    };
}
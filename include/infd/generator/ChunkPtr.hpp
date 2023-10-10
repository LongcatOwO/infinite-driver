#pragma once

#include "ChunkGenerator.hpp"

namespace infd::generator {
    class ChunkPtr {
        bool _detached = false;

    public:
        ChunkPtr(ChunkGenerator& generator);

        void detach();
    };
}
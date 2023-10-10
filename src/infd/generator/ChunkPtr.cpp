#include "infd/generator/ChunkPtr.hpp"

namespace infd::generator {
    ChunkPtr::ChunkPtr(ChunkGenerator& generator) {
        //TODO
    }

    void ChunkPtr::detach() {
        _detached = true;
    }
}
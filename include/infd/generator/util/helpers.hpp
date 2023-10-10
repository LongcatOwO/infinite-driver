#pragma once

#include <vector>
#include <random>
#include <stdexcept>

namespace infd::generator::helpers {
    // Alias to allow for replacement if required for performance reasons
    using RandomType = std::mt19937;

    /**
     * Produces a hash for a given unsigned int.
     * Sourced from <a href="https://github.com/skeeto/hash-prospector">here</a>
     */
    inline unsigned int skeetoHash(unsigned int x) {
        x ^= x >> 16;
        x *= 0x7feb352d;
        x ^= x >> 15;
        x *= 0x846ca68b;
        x ^= x >> 16;
        return x;
    }

    /**
     * Generates a true modulo with correct behaviour for negative values.
     */
    inline int positiveModulo(int i, int n) {
        return (i % n + n) % n;
    }

    /**
     * Combines 2 unsigned ints into a unique single int value.
     * Sourced from <a href="http://szudzik.com/ElegantPairing.pdf">here</a>
     */
    inline unsigned int szudsikCombinator(unsigned int x, unsigned int y) {
        return x >= y ? x * x + x + y : y * y + x;
    }

    inline unsigned int szudsikSignedCombinator(int x, int y) {
        unsigned int offset = (x < 0) + 2 * (y < 0);
        return offset + 4 * szudsikCombinator(abs(x), abs(y));
    }

    /**
     * Generates a new random generator, seeded with a given world location.
     */
    template<class Generator>
    Generator generateRandom(int x, int y, unsigned int seed = 0) {
        return Generator(skeetoHash(szudsikSignedCombinator(x, y)+seed));
    }

    /**
     * Generates a random padded distribution of points between 0 and 1;
     */
    template <class T, class Generator>
    std::vector<T> paddedDistribution(size_t count, T radius, T edgePadding, Generator &random) {
        std::uniform_real_distribution<T> dist(0, 1);

        T padding = 2*(radius*count + edgePadding);

        if (padding > 1) throw std::runtime_error("Not enough space for random distribution.");

        std::vector<T> values;
        values.reserve(count);

        for (unsigned int i = 0; i < count; i++) {
            values.push_back(dist(random) * (1 - padding) + radius + edgePadding + (2 * radius * i));
        }

        return values;
    }
}
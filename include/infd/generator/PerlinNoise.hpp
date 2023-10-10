#pragma once

#include <vector>
#include <algorithm>
#include <numeric>
#include "infd/generator/util/helpers.hpp"

namespace infd::generator {
    /**
     * Based off source found <a href="https://mrl.cs.nyu.edu/~perlin/noise/">here</a>
     */
    class PerlinNoise {
    private:
        std::vector<int> _permutationVector;

        template<class T>
        static T fade(T t);

        template<class T>
        static T lerp(T t, T a, T b);

        template<class T>
        static T grad(int hash, T x, T y, T z);

    public:
        inline PerlinNoise();
        inline PerlinNoise(unsigned int seed);

        template<class T>
        T sample(T x, T y, T z);
    };

    template<class T>
    T PerlinNoise::fade(T t) {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }

    template<class T>
    T PerlinNoise::lerp(T t, T a, T b) {
        return a + t * (b - a);
    }

    template<class T>
    T PerlinNoise::grad(int hash, T x, T y, T z) {
        int h = hash & 15;
        T u = h < 8 ? x : y;
        T v = h < 4 ? y : h == 12 || h == 14 ? x : z;
        return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
    }

    PerlinNoise::PerlinNoise(unsigned int seed) {
        _permutationVector.resize(256);

        std::iota(_permutationVector.begin(), _permutationVector.end(), 0);

        helpers::RandomType random(seed);

        std::shuffle(_permutationVector.begin(), _permutationVector.end(), random);

        _permutationVector.insert(
                _permutationVector.end(),
                _permutationVector.begin(),
                _permutationVector.end()
        );
    }

    PerlinNoise::PerlinNoise() = default;

    template<class T>
    T PerlinNoise::sample(T x, T y, T z) {
        int x_unit = (int) floor(x) & 255;
        int y_unit = (int) floor(y) & 255;
        int z_unit = (int) floor(z) & 255;
        x -= floor(x);
        y -= floor(y);
        z -= floor(z);
        T u = fade(x);
        T v = fade(y);
        T w = fade(z);

        int a = _permutationVector[x_unit] + y_unit;
        int aa = _permutationVector[a] + z_unit;
        int ab = _permutationVector[a + 1] + z_unit;
        int b = _permutationVector[x_unit + 1] + y_unit;
        int ba = _permutationVector[b] + z_unit;
        int bb = _permutationVector[b + 1] + z_unit;

        return lerp(w, lerp(v, lerp(u, grad(_permutationVector[aa], x, y, z),
                                    grad(_permutationVector[ba], x - 1, y, z)),
                            lerp(u, grad(_permutationVector[ab], x, y - 1, z),
                                 grad(_permutationVector[bb], x - 1, y - 1, z))),
                    lerp(v, lerp(u, grad(_permutationVector[aa + 1], x, y, z - 1),
                                 grad(_permutationVector[ba + 1], x - 1, y, z - 1)),
                         lerp(u, grad(_permutationVector[ab + 1], x, y - 1, z - 1),
                              grad(_permutationVector[bb + 1], x - 1, y - 1, z - 1))));
    }
}
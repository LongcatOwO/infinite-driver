// Created by Phuwasate Lutchanont

#pragma once

// std
#include <ostream>

// glm
#include <glm/detail/qualifier.hpp>
#include <glm/detail/setup.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace infd::debug {

	template <typename CharT, typename Traits, glm::length_t L, typename T, glm::qualifier Q>
	std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const glm::vec<L, T, Q>& v) {
		os << '[' << v[0];
		for (glm::length_t i = 1; i < L; ++i)
			os << ", " << v[i];
		os << ']';
		return os;
	}

} // namespace infd::debug

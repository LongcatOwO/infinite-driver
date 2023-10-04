#pragma once

// bullet
#include <LinearMath/btVector3.h>

// glm
#include <glm/detail/qualifier.hpp>
#include <glm/vec3.hpp>


namespace infd::math {
	template <glm::qualifier Q>
	btVector3 toBullet(const glm::vec<3, btScalar, Q> &v) noexcept {
		return { v.x, v.y, v.z };
	}

	template <glm::qualifier Q = glm::defaultp>
	glm::vec<3, btScalar, Q> toGLM(const btVector3 &v) noexcept {
		return { v.x(), v.y(), v.z() };
	}
}

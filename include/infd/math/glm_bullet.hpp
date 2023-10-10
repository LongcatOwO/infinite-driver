#pragma once

// bullet
#include <LinearMath/btMatrix3x3.h>
#include <LinearMath/btQuaternion.h>
#include <LinearMath/btTransform.h>
#include <LinearMath/btVector3.h>

// glm
#include <glm/detail/qualifier.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>


namespace infd::math {
	template <glm::qualifier Q>
	btVector3 toBullet(const glm::vec<3, btScalar, Q>& v) noexcept {
		return { v.x, v.y, v.z };
	}

	template <glm::qualifier Q = glm::defaultp>
	glm::vec<3, btScalar, Q> toGlm(const btVector3& v) noexcept {
		return { v.x(), v.y(), v.z() };
	}

	template <glm::qualifier Q>
	btQuaternion toBullet(const glm::qua<btScalar, Q>& q) noexcept {
		return { q.x, q.y, q.z, q.w };
	}

	template <glm::qualifier Q = glm::defaultp>
	glm::qua<btScalar, Q> toGlm(const btQuaternion& q) noexcept {
		return { q.w(), q.x(), q.y(), q.z() };
	}

	template <glm::qualifier Q>
	btMatrix3x3 toBullet(const glm::mat<3, 3, btScalar, Q>& m) noexcept {
		return {
			m[0][0], m[1][0], m[2][0],
			m[0][1], m[1][1], m[2][1],
			m[0][2], m[1][2], m[2][2]
		};
	}

	template <glm::qualifier Q = glm::defaultp>
	glm::mat<3, 3, btScalar, Q> toGlm(const btMatrix3x3& m) noexcept {
		return {
			m[0][0], m[1][0], m[2][0],
			m[0][1], m[1][1], m[2][1],
			m[0][2], m[1][2], m[2][2]
		};
	}

	/*
	 * Only for affine transformation, otherwise lossy.
	 */
	template <glm::qualifier Q>
	btTransform toBullet(const glm::mat<4, 4, btScalar, Q>& m) noexcept {
		return btTransform(toBullet(glm::mat<3, 3, btScalar, Q>(m)), toBullet(glm::vec<3, btScalar, Q>(m[3])));
	}

	template <glm::qualifier Q = glm::defaultp>
	glm::mat<4, 4, btScalar, Q> toGlm(const btTransform& t) noexcept {
		// rotation
		glm::mat<4, 4, btScalar, Q> m(toGlm(t.getBasis()));
		// translation
		m[3] = glm::vec<4, btScalar, Q>(toGlm(t.getOrigin()), 1);

		return m;
	}

}

#pragma once

// bullet
#include <BulletCollision/CollisionShapes/btCapsuleShape.h>
#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include <LinearMath/btScalar.h>

// glm
#include <glm/vec3.hpp>

// project - math
#include <infd/math/glm_bullet.hpp>

// project - scene::physics
#include <infd/scene/physics/physics.hpp>

namespace infd::scene::physics {

	class CapsuleShape : public CollisionShape {
		btCapsuleShape _capsule_shape;
		glm::vec<3, Float> _scale;

		[[nodiscard]] btCollisionShape& getBtCollisionShape() noexcept override {
			return _capsule_shape;
		}

		[[nodiscard]] const btCollisionShape& getBtCollisionShape() const noexcept override {
			return _capsule_shape;
		}

		void syncCollisionShapeScaling() noexcept override {
			_capsule_shape.setLocalScaling(math::toBullet(_scale * transform().localScale())); 
		}

	public:
		CapsuleShape(btScalar radius, btScalar height) : 
			_capsule_shape(radius, height),
			_scale(1) {}

		glm::vec<3, Float> scale() const noexcept {
			return _scale;
		}

		void scale(const glm::vec<3, Float>& value) noexcept {
			_scale = value;
		}

	}; // class CapsuleShape

} // namespace infd::scene::physics

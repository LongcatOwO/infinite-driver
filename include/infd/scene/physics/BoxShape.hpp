#pragma once

// bullet
#include <btBulletDynamicsCommon.h>

// glm
#include <glm/vec3.hpp>

// project - math
#include <infd/math/glm_bullet.hpp>

// project - scene::physics
#include <infd/scene/physics/physics.hpp>

namespace infd::scene::physics {

	class BoxShape : public CollisionShape {
	public:
		static constexpr glm::vec<3, Float> default_half_size{1, 1, 1};

	private:
		btBoxShape _box_shape;
		glm::vec<3, Float> _half_size;

		[[nodiscard]] btCollisionShape& getBtCollisionShape() override {
			return _box_shape;
		}

		[[nodiscard]] const btCollisionShape& getBtCollisionShape() const override {
			return _box_shape;
		}

		void syncCollisionShapeScaling() override {
			_box_shape.setLocalScaling(
				math::toBullet(_half_size * transform().localScale())
			);
		}

	public:
		[[nodiscard]] BoxShape(const glm::vec<3, Float>& half_size = default_half_size) noexcept :
			_box_shape({1, 1, 1}),
			_half_size(half_size) {}

		[[nodiscard]] glm::vec<3, Float> halfSize() const noexcept {
			return _half_size;
		}

		void halfSize(const glm::vec<3, Float>& value) noexcept {
			_half_size = value;
		}
	};

} // namespace infd::scene::physics

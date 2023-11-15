// Created by Phuwasate Lutchanont

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

	class StaticPlaneShape : public CollisionShape {
		btStaticPlaneShape _static_plane_shape;

		[[nodiscard]] btCollisionShape& getBtCollisionShape() noexcept override {
			return _static_plane_shape;
		}

		[[nodiscard]] const btCollisionShape& getBtCollisionShape() const noexcept override {
			return _static_plane_shape;
		}

		void syncCollisionShapeScaling() override {}

	public:
		StaticPlaneShape(const glm::vec<3, Float>& plane_normal, Float plane_constant) noexcept :
			_static_plane_shape(math::toBullet(plane_normal), plane_constant) {}

	}; // class StaticPlaneShape

} // namespace infd::scene::physics

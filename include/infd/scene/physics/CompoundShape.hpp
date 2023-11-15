// Created by Phuwasate Lutchanont

#pragma once

// std
#include <memory>

// bullet
#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include <BulletCollision/CollisionShapes/btCompoundShape.h>

// glm
#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>

// project - math
#include <infd/math/glm_bullet.hpp>

// project - scene::physics
#include <infd/scene/physics/physics.hpp>

namespace infd::scene::physics {

	class CompoundShape : public CollisionShape {
		btCompoundShape _compound_shape;

		[[nodiscard]] btCollisionShape& getBtCollisionShape() noexcept override {
			return _compound_shape;
		}

		[[nodiscard]] const btCollisionShape& getBtCollisionShape() const noexcept override {
			return _compound_shape;
		}

		void syncCollisionShapeScaling() override {
			_compound_shape.setLocalScaling(math::toBullet(transform().localScale()));
		}
		
	public:
		void addChildShape(
			std::unique_ptr<btCollisionShape> collision_shape,
			const glm::vec<3, Float>& local_position 	= {0, 0, 0},
			const glm::qua<Float>& local_rotation 		= {1, 0, 0, 0}
		) noexcept {
			_compound_shape.addChildShape(
				btTransform{
					math::toBullet(local_rotation),
					math::toBullet(local_position)
				}, 
				collision_shape.release()
			);
		}

		~CompoundShape() noexcept {
			int size;
			while ((size = _compound_shape.getNumChildShapes())) {
				btCollisionShape* child_shape = _compound_shape.getChildShape(size - 1);
				_compound_shape.removeChildShapeByIndex(size - 1);
				delete child_shape;
			}
		}
	}; // class CompoundShape

} // namespace infd::scene::physics

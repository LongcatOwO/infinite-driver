#pragma once

// std
#include <memory>

// bullet
#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include <BulletCollision/CollisionShapes/btTriangleMesh.h>
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>

// project - scene::physics
#include <infd/scene/physics/physics.hpp>


#include <iostream>
#include <infd/debug/glm.hpp>


namespace infd::scene::physics {

	class BvhTriangleMeshShape : public CollisionShape {
		std::unique_ptr<btTriangleMesh> _triangle_mesh;
		btBvhTriangleMeshShape _triangle_mesh_shape;

		btCollisionShape& getBtCollisionShape() noexcept override {
			return _triangle_mesh_shape;
		}

		const btCollisionShape& getBtCollisionShape() const noexcept override {
			return _triangle_mesh_shape;
		}

		void syncCollisionShapeScaling() override {
			_triangle_mesh_shape.setLocalScaling(
				math::toBullet(transform().localScale())
			);
		}

	public:
		[[nodiscard]] BvhTriangleMeshShape(std::unique_ptr<btTriangleMesh> triangle_mesh) noexcept :
			_triangle_mesh(std::move(triangle_mesh)),
			_triangle_mesh_shape(_triangle_mesh.get(), true)
		{}

	}; // class BvhTriangleMeshShape

} // namespace infd::scene::physics

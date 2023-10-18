#pragma once

// bullet
#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include <BulletCollision/CollisionShapes/btCylinderShape.h>

// project - debug
#include <infd/debug/geometry.hpp>

// project - math
#include <infd/math/glm_bullet.hpp>

// project - render
#include <infd/render/Renderer.hpp>
#include <infd/render/RenderComponent.hpp>

// project - scene::physics
#include <infd/scene/physics/physics.hpp>


namespace infd::scene::physics {

	class CylinderShape : public CollisionShape {
	public:
		static constexpr glm::vec<3, Float> default_half_size{1, 1, 1};

	private:
		btCylinderShape _cylinder_shape;
		glm::vec<3, Float> _half_size;
		Transform* _outline_mesh = nullptr;

		[[nodiscard]] btCollisionShape& getBtCollisionShape() noexcept override {
			return _cylinder_shape;
		}

		[[nodiscard]] const btCollisionShape& getBtCollisionShape() const noexcept override {
			return _cylinder_shape;
		}

		void syncCollisionShapeScaling() override {
			_cylinder_shape.setLocalScaling(math::toBullet(_half_size * transform().localScale()));
		}


	public:
		[[nodiscard]] CylinderShape(const glm::vec<3, Float>& half_size = default_half_size) noexcept :
			_cylinder_shape({1, 1, 1}),
			_half_size(half_size) {}

		[[nodiscard]] glm::vec<3, Float> halfSize() const noexcept {
			return _half_size;
		}

		void halfSize(const glm::vec<3, Float>& value) noexcept {
			_half_size = value;
			if (_outline_mesh)
				_outline_mesh->localScale(_half_size);
		}

		void createOutlineMesh(render::Renderer &renderer) override {
			if (!_outline_mesh) {
				_outline_mesh = &transform().addChild("outline");
				_outline_mesh
					->emplaceComponent<render::RenderComponent>(renderer, debug::generateCylinderOutline())
					.material.colour = default_outline_color;
				_outline_mesh->localScale(_half_size);
			}
		}

	}; // class CylinderShape

} // namespace infd::scene::physics

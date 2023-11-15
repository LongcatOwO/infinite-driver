// Created by Phuwasate Lutchanont

#pragma once

// std
#include <memory>

// bullet
#include <btBulletCollisionCommon.h>

// glm
#include <glm/vec3.hpp>

// project - scene
#include <infd/scene/exceptions.hpp>
#include <infd/scene/Scene.hpp>

// project - renderer
#include <infd/render/Renderer.hpp>

// project - forward declarations
#include <infd/scene/physics/fwd/CollisionShape.hpp>
#include <infd/scene/physics/fwd/PhysicsContext.hpp>
#include <infd/scene/physics/fwd/RigidBody.hpp>


namespace infd::scene::physics {

	class CollisionShape : public Component {
		friend class PhysicsContext;
		friend class RigidBody;

		void onAttach() override;

	protected:
		[[nodiscard]] virtual btCollisionShape& getBtCollisionShape() noexcept = 0;
		[[nodiscard]] virtual const btCollisionShape& getBtCollisionShape() const noexcept = 0;
		virtual void syncCollisionShapeScaling() = 0;

	public:
		static constexpr glm::vec<3, Float> default_outline_color{0, 1, 0};

		virtual void createOutlineMesh(render::Renderer& renderer) {
			(void) renderer;
		};

	}; // class CollisionBound

} // namespace infd::scene::physics

#pragma once

// std
#include <memory>

// bullet
#include <btBulletCollisionCommon.h>

// project - scene
#include <infd/scene/exceptions.hpp>
#include <infd/scene/Scene.hpp>

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
		[[nodiscard]] virtual btCollisionShape& getBtCollisionShape() = 0;
		[[nodiscard]] virtual const btCollisionShape& getBtCollisionShape() const = 0;
		virtual void syncCollisionShapeScaling() = 0;

	}; // class CollisionBound

} // namespace infd::scene::physics

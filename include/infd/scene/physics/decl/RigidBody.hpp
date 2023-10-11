#pragma once

// std
#include <iostream>
#include <memory>

// bullet
#include <btBulletDynamicsCommon.h>

// glm
#include <glm/detail/qualifier.hpp>
#include <glm/vec3.hpp>

// project - scene
#include <infd/scene/Scene.hpp>

// project - util
#include <infd/util/aligned_containers.hpp>
#include <infd/util/OwnableProperty.hpp>

// project - forward declarations
#include <infd/scene/physics/fwd/CollisionShape.hpp>
#include <infd/scene/physics/fwd/RigidBody.hpp>

// project - declarations
#include <infd/scene/physics/decl/PhysicsContext.hpp>


namespace infd::scene::physics {

	class RigidBody final : public Component {
		friend class PhysicsContext;
		friend class PhysicsContext::RigidBodyLifeSpan;

		using LifeSpanHandle = PhysicsContext::RigidBodyLifeSpanHandle;

		PhysicsContext *_physics_context = nullptr;
		CollisionShape *_collision_shape = nullptr;

		std::unique_ptr<btMotionState> _motion_state;
		std::unique_ptr<btRigidBody> _rigid_body;
		LifeSpanHandle _life_span_handle;

		util::PropertyOwner<bool> _transform_ignore_parent;

		bool _debugged = false;

		/*
		 * @throws MissingRootSceneObjectException if a root SceneObject with PhysicsContext cannot be found.
		 */
		void internalFindPhysicsContext();

		/*
		 * @throws MissingComponentException if there is no CollisionBound on the same SceneObject.
		 */
		void internalFindCollisionBound();

		void internalInitializeRigidBody();

		void internalSyncTransformToMotionState() noexcept;
		void internalSyncMotionStateToTransform() noexcept;

		/*
		 * @throws MissingRootSceneObjectException if no root scene object is found with 
		 * PhysicsContext
		 * @throws MissingComponentException if there is no CollisionBound on the same SceneObject.
		 */
		void onAwake() override;

		void onAttach() override;

		void onDetach() override;

	public:
		RigidBody();

		/*
		 * @throws InvalidStateException if this component is not yet awakened.
		 */
		PhysicsContext& physicsContext();

		/*
		 * @throws InvalidStateException if this component is not yet awakened.
		 */
		const PhysicsContext& physicsContext() const;

		/*
		 * @throws InvalidStateException if this component is not yet awakened.
		 */
		CollisionShape& collisionShape();

		/*
		 * @throws InvalidStateException if this component is not yet awakened.
		 */
		const CollisionShape& collisionShape() const;


		btScalar mass() const noexcept;
		void mass(btScalar mass) noexcept;

		template <glm::qualifier Q = glm::defaultp>
		glm::vec<3, btScalar, Q> inertia() const noexcept;

		template <glm::qualifier Q>
		void inertia(const glm::vec<3, btScalar, Q>& inertia) noexcept;

		template <glm::qualifier Q = glm::defaultp>
		void setMassProperties(btScalar mass, const glm::vec<3, btScalar, Q>& inertia) noexcept;

	}; // class Rigidbody

} // namespace infd::scene::physics

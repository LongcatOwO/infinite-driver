// Created by Phuwasate Lutchanont

#pragma once

// std
#include <memory>

// bullet
#include <btBulletDynamicsCommon.h>

// glm
#include <glm/detail/qualifier.hpp>
#include <glm/gtc/quaternion.hpp>
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

		PhysicsContext* _physics_context = nullptr;
		CollisionShape* _collision_shape = nullptr;

		std::unique_ptr<btMotionState> _motion_state{nullptr};
		std::unique_ptr<btRigidBody> _rigid_body;
		LifeSpanHandle _life_span_handle;

		util::PropertyOwner<bool> _transform_ignore_parent;

		/*
		 * @throws MissingRootSceneObjectException if a root SceneObject with PhysicsContext cannot be found.
		 */
		void internalFindPhysicsContext();

		/*
		 * @throws MissingComponentException if there is no CollisionBound on the same SceneObject.
		 */
		void internalFindCollisionBound();

		void internalInitializeRigidBody();

		/*
		 * @throws MissingRootSceneObjectException if no root scene object is found with 
		 * PhysicsContext
		 * @throws MissingComponentException if there is no CollisionBound on the same SceneObject.
		 */
		void onAwake() override;

		void onDetach() override;

	public:
		RigidBody();

		/*
		 * @throws InvalidStateException if this component is not yet awakened.
		 */
		[[nodiscard]] PhysicsContext& physicsContext();

		/*
		 * @throws InvalidStateException if this component is not yet awakened.
		 */
		[[nodiscard]] const PhysicsContext& physicsContext() const;

		/*
		 * @throws InvalidStateException if this component is not yet awakened.
		 */
		[[nodiscard]] CollisionShape& collisionShape();

		/*
		 * @throws InvalidStateException if this component is not yet awakened.
		 */
		[[nodiscard]] const CollisionShape& collisionShape() const;


		btScalar mass() const noexcept;
		void mass(btScalar mass) noexcept;

		template <glm::qualifier Q = glm::defaultp>
		[[nodiscard]] glm::vec<3, btScalar, Q> inertia() const noexcept;

		template <glm::qualifier Q>
		void inertia(const glm::vec<3, btScalar, Q>& inertia) noexcept;

		template <glm::qualifier Q = glm::defaultp>
		void setMassProperties(btScalar mass, const glm::vec<3, btScalar, Q>& inertia) noexcept;

		[[nodiscard]] bool isKinematic() const noexcept;
		void isKinematic(bool value) noexcept;

		[[nodiscard]] bool isStatic() const noexcept;

		[[nodiscard]] btScalar friction() const noexcept;
		void friction(btScalar value) noexcept;

		[[nodiscard]] btScalar restitution() const noexcept;
		void restitution(btScalar value) noexcept;

		[[nodiscard]] glm::vec<3, Float> gravity() const noexcept;
		void gravity(const glm::vec<3, Float>& value) noexcept;

		[[nodiscard]] btScalar linearDamping() const noexcept;
		void linearDamping(btScalar value) noexcept;

		[[nodiscard]] btScalar angularDamping() const noexcept;
		void angularDamping(btScalar value) noexcept;

		auto damping() const noexcept;
		void damping(btScalar linear, btScalar angular) noexcept;

		void applyCentralForce(const glm::vec<3, Float>& force) noexcept;
		void applyCentralImpulse(const glm::vec<3, Float>& impulse) noexcept;

		void applyForce(const glm::vec<3, Float>& force, const glm::vec<3, Float>& relative_position) noexcept;
		void applyImpulse(const glm::vec<3, Float>& impulse, const glm::vec<3, Float>& relative_position) noexcept;

		void applyTorque(const glm::vec<3, Float>& torque) noexcept;
		void applyTorqueImpulse(const glm::vec<3, Float>& torque_impulse) noexcept;

		[[nodiscard]] glm::vec<3, Float> linearVelocity() const noexcept;
		void linearVelocity(const glm::vec<3, Float>& value) noexcept;

		[[nodiscard]] glm::vec<3, Float> angularVelocity() const noexcept;
		void angularVelocity(const glm::vec<3, Float>& value) noexcept;

		[[nodiscard]] glm::vec<3, Float> centerOfMassPosition() const noexcept;
		void centerOfMassPosition(const glm::vec<3, Float>& value) noexcept;

	}; // class Rigidbody

} // namespace infd::scene::physics

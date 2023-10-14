#pragma once

// glm
#include <glm/detail/qualifier.hpp>
#include <glm/vec3.hpp>

// bullet
#include <btBulletDynamicsCommon.h>

// project - math
#include <infd/math/glm_bullet.hpp>

// project - declarations
#include <infd/scene/physics/decl/CollisionShape.hpp>
#include <infd/scene/physics/decl/PhysicsContext.hpp>
#include <infd/scene/physics/decl/RigidBody.hpp>


namespace infd::scene::physics {

	inline PhysicsContext& RigidBody::physicsContext() {
		return const_cast<PhysicsContext&>(const_cast<const RigidBody&>(*this).physicsContext());
	}

	inline const PhysicsContext& RigidBody::physicsContext() const {
		if (!_physics_context)
			throw util::InvalidStateException(std::format(
					"[Rigidbody(\"{}\")::physicsContext()]: PhysicsContext is not yet registered.",
					sceneObject().name()
				));

		return *_physics_context;
	}

	inline CollisionShape& RigidBody::collisionShape() {
		return const_cast<CollisionShape&>(const_cast<const RigidBody&>(*this).collisionShape());
	}

	inline const CollisionShape& RigidBody::collisionShape() const {
		if (!_collision_shape)
			throw util::InvalidStateException(std::format(
					"[RigidBody(\"{}\")::collisionShape()]: CollisionShape is not yet registered.",
					sceneObject().name()
				));

		return *_collision_shape;
	}

	inline btScalar RigidBody::mass() const noexcept {
		return _rigid_body->getMass();
	}

	inline void RigidBody::mass(btScalar mass) noexcept {
		setMassProperties(mass, inertia());
	}

	template <glm::qualifier Q>
	glm::vec<3, btScalar, Q> RigidBody::inertia() const noexcept {
		return math::toGlm(_rigid_body->getLocalInertia());
	}

	template <glm::qualifier Q>
	void RigidBody::inertia(const glm::vec<3, btScalar, Q>& inertia) noexcept {
		setMassProperties(mass(), inertia);
	}

	template <glm::qualifier Q>
	void RigidBody::setMassProperties(btScalar mass, const glm::vec<3, btScalar, Q> &inertia) noexcept {
		_rigid_body->setMassProps(mass, math::toBullet(inertia));
	}

	inline bool RigidBody::isKinematic() const noexcept {
		return !!(_rigid_body->getCollisionFlags() & btCollisionObject::CF_KINEMATIC_OBJECT);
	}

	inline void RigidBody::isKinematic(bool value) noexcept {
		if (value) {
			_rigid_body->setCollisionFlags(
				_rigid_body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT
			);
		} else {
			_rigid_body->setCollisionFlags(
				_rigid_body->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT
			);
		}
	}

	inline bool RigidBody::isStatic() const noexcept {
		return !!(_rigid_body->getCollisionFlags() & btCollisionObject::CF_STATIC_OBJECT);
	}

} // namespace infd::scene::physics

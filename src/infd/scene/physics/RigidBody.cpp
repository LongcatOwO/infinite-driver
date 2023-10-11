// std
#include <format>
#include <iostream>

// bullet
#include <btBulletDynamicsCommon.h>

// prject - scene
#include <infd/scene/exceptions.hpp>

// project - math
#include <infd/math/glm_bullet.hpp>

// project - util
#include <infd/util/StaticCastOutPtr.hpp>

// project - debug
#include <infd/debug/glm.hpp>

// project - scene::physics
#include <infd/scene/physics/physics.hpp>


namespace infd::scene::physics {

	RigidBody::RigidBody() : 
		_motion_state(new btDefaultMotionState()), 
		_rigid_body(new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(1, _motion_state.get(), nullptr))) {
	}

	void RigidBody::internalFindPhysicsContext() {
		_physics_context = scene().findComponentInRoots<PhysicsContext>();
		if (!_physics_context)
			throw MissingRootSceneObjectException(std::format(
					"[Rigidbody(\"{}\")::findPhysicsContext()]: cannot find PhysicsContext in any root scene objects",
					sceneObject().name()
				));
	}

	void RigidBody::internalFindCollisionBound() {
		_collision_shape = getComponent<CollisionShape>();
		if (!_collision_shape)
			throw MissingComponentException(std::format(
					"[Rigidbody(\"{}\")::findCollisionBound()]: cannot find CollisionBound on the same SceneObject",
					sceneObject().name()
				));
	}

	void RigidBody::internalInitializeRigidBody() {
		btCollisionShape* shape = &_collision_shape->getBtCollisionShape();
		btScalar mass = _rigid_body->getMass();
		btVector3 inertia;
		if (mass != 0)
			shape->calculateLocalInertia(mass, inertia);
		else
			inertia = {0, 0, 0};

		_motion_state->setWorldTransform(math::toBullet(transform().globalTransform()));
		_rigid_body->setCollisionShape(shape);
	}

	void RigidBody::internalSyncTransformToMotionState() noexcept {
		btTransform t;
		_motion_state->getWorldTransform(t);
		transform().localPosition(math::toGlm(t.getOrigin()));
		transform().localRotation(math::toGlm(t.getRotation()));
	}

	void RigidBody::internalSyncMotionStateToTransform() noexcept {
		_motion_state->setWorldTransform(btTransform{
			math::toBullet(transform().localRotation()),
			math::toBullet(transform().localPosition())
		});
	}

	void RigidBody::onAwake() {
		internalFindPhysicsContext();
		internalFindCollisionBound();
		internalInitializeRigidBody();
		_life_span_handle = physicsContext().internalAddRigidBody(*this);
	}

	void RigidBody::onAttach() {
		if (!transform().ignoreParent().setOwner(&_transform_ignore_parent)) {
			throw util::InvalidStateException(std::format(
					"[RigidBody(\"{}\")::onAttach()]: Transform's ignoreParent already owned.",
					sceneObject().name()
				));
		}
		_transform_ignore_parent.set(true);
	}

	void RigidBody::onDetach() {
		_transform_ignore_parent.release();
		_life_span_handle.destroy_element();
		_rigid_body->setMassProps(1, {0, 0, 0});
	}

} // namespace infd::scene::physics
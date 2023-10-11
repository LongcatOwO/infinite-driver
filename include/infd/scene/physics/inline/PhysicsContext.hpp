#pragma once

// std
#include <chrono>

// bullet
#include <btBulletDynamicsCommon.h>

// project - declarations
#include <infd/scene/physics/decl/PhysicsContext.hpp>
#include <infd/scene/physics/decl/RigidBody.hpp>


namespace infd::scene::physics {

	inline void PhysicsContext::RigidBodyLifeSpan::takeOwnership(RigidBodyLifeSpan& other) noexcept {
		_physics_context = other._physics_context;
		_rigid_body = other._rigid_body;
		other._physics_context = nullptr;
		other._rigid_body = nullptr;
	}

	inline void PhysicsContext::RigidBodyLifeSpan::registerRigidBody() noexcept {
		if (!_physics_context) return;
		_physics_context->_dynamics_world->addRigidBody(_rigid_body->_rigid_body.get());
	}

	inline void PhysicsContext::RigidBodyLifeSpan::unregisterRigidBody() noexcept {
		if (!_physics_context) return;
		_physics_context->_dynamics_world->removeRigidBody(_rigid_body->_rigid_body.get());
	}

	inline PhysicsContext::RigidBodyLifeSpan::RigidBodyLifeSpan(
		PhysicsContext& physics_context, 
		RigidBody& rigid_body
	) noexcept :
		_physics_context(&physics_context),
		_rigid_body(&rigid_body)
	{
		registerRigidBody();
	}

	inline PhysicsContext::RigidBodyLifeSpan::RigidBodyLifeSpan(RigidBodyLifeSpan&& other) noexcept {
		takeOwnership(other);
	}

	inline auto PhysicsContext::RigidBodyLifeSpan::operator=(RigidBodyLifeSpan&& other) noexcept
	-> RigidBodyLifeSpan& {
		unregisterRigidBody();
		takeOwnership(other);
		return *this;
	}

	inline PhysicsContext::RigidBodyLifeSpan::~RigidBodyLifeSpan() noexcept {
		unregisterRigidBody();
	}

	inline RigidBody& PhysicsContext::RigidBodyLifeSpan::rigidBody() noexcept {
		return *_rigid_body;
	}

	inline const RigidBody& PhysicsContext::RigidBodyLifeSpan::rigidBody() const noexcept {
		return *_rigid_body;
	}

	inline auto PhysicsContext::internalAddRigidBody(RigidBody& rigid_body) noexcept
	-> RigidBodyLifeSpanHandle {
		return _rigid_body_life_spans.emplace_back(*this, rigid_body);
	}

} // namespace infd::scene::physics

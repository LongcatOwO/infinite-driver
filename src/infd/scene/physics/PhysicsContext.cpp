// Created by Phuwasate Lutchanont

// bullet
#include <btBulletDynamicsCommon.h>

// project - scene
#include <infd/scene/physics/physics.hpp>

// project - math
#include <infd/math/glm_bullet.hpp>

namespace infd::scene::physics {

	void PhysicsContext::onPhysicsUpdate() {
		for (RigidBodyLifeSpan& life_span : _rigid_body_life_spans) {
			life_span.rigidBody().collisionShape().syncCollisionShapeScaling();
		}

		_dynamics_world->stepSimulation(
			std::chrono::duration_cast<std::chrono::duration<btScalar>>(
				physicsDeltaTime()
			).count(),
			1,
			std::chrono::duration_cast<std::chrono::duration<btScalar>>(
				scene().physicsInterval()
			).count()
		);
	}

	PhysicsContext::PhysicsContext() noexcept :
		_collision_configuration(new btDefaultCollisionConfiguration()),
		_dispatcher(new btCollisionDispatcher(_collision_configuration.get())),
		_broadphase(new btDbvtBroadphase()),
		_constraint_solver(new btSequentialImpulseConstraintSolver()),
		_dynamics_world(new btDiscreteDynamicsWorld(
			_dispatcher.get(),
			_broadphase.get(),
			_constraint_solver.get(),
			_collision_configuration.get()
		))
	{
		_dynamics_world->setGravity(math::toBullet(default_gravity));
	}

} // namespace infd::scene

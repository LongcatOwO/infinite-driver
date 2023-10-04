// bullet
#include <btBulletDynamicsCommon.h>

// project - scene
#include <infd/scene/PhysicsContext.hpp>

// project - math
#include <infd/math/glm_bullet.hpp>

namespace infd::scene {
	PhysicsContext::PhysicsContext() noexcept :
		_collision_configuration(new btDefaultCollisionConfiguration()),
		_dispatcher(new btCollisionDispatcher(_collision_configuration.get())),
		_overlapping_pair_cache(new btDbvtBroadphase()),
		_constraint_solver(new btSequentialImpulseConstraintSolver()),
		_dynamics_world(new btDiscreteDynamicsWorld(
			_dispatcher.get(),
			_overlapping_pair_cache.get(),
			_constraint_solver.get(),
			_collision_configuration.get()
		))
	{
		using namespace math;
		_dynamics_world->setGravity(toBullet(default_gravity));
	}
}

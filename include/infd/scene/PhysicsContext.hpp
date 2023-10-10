#pragma once

// std
#include <memory>

// glm
#include <glm/vec3.hpp>

// bullet
#include <btBulletDynamicsCommon.h>

// project - util
#include <infd/util/aligned_containers.hpp>

// project - scene
#include <infd/scene/definitions.hpp>


namespace infd::scene {
	class PhysicsContext final {
		static constexpr glm::vec<3, Float> default_gravity{0, -10, 0};

		std::unique_ptr<btCollisionConfiguration> _collision_configuration;
		std::unique_ptr<btDispatcher> _dispatcher;
		std::unique_ptr<btBroadphaseInterface> _overlapping_pair_cache;
		std::unique_ptr<btConstraintSolver> _constraint_solver;
		std::unique_ptr<btDynamicsWorld> _dynamics_world;
		util::aligned_vector<std::unique_ptr<btCollisionShape>> _collision_shapes;

	public:
		[[nodiscard]] PhysicsContext() noexcept;
	}; // class PhysicsContext
} // namespace infd::scene

#pragma once

// std
#include <memory>

// glm
#include<glm/vec3.hpp>

// bullet
#include <btBulletDynamicsCommon.h>

// project - scene
#include <infd/scene/Scene.hpp>

// project - util
#include <infd/util/handle_vector.hpp>

// project - forward declarations
#include <infd/scene/physics/fwd/PhysicsContext.hpp>
#include <infd/scene/physics/fwd/RigidBody.hpp>

namespace infd::scene::physics {

	class PhysicsContext final : public Component {
		friend class RigidBody;

		class RigidBodyLifeSpan {
			friend class PhysicsContext;

			PhysicsContext *_physics_context;
			RigidBody *_rigid_body;

			void takeOwnership(RigidBodyLifeSpan& other) noexcept;
			void registerRigidBody() noexcept;
			void unregisterRigidBody() noexcept;

		public:
			RigidBodyLifeSpan(PhysicsContext& physics_context, RigidBody& rigid_body) noexcept;
			RigidBodyLifeSpan(RigidBodyLifeSpan&& other) noexcept;
			RigidBodyLifeSpan& operator=(RigidBodyLifeSpan&& other) noexcept;
			~RigidBodyLifeSpan() noexcept;

			RigidBodyLifeSpan& operator=(const RigidBodyLifeSpan&) = delete;
			RigidBodyLifeSpan(const RigidBodyLifeSpan&) = delete;

			RigidBody& rigidBody() noexcept;
			const RigidBody& rigidBody() const noexcept;

		}; // RigidBodyLifeSpan

		using Float = float;
		using RigidBodyLifeSpanHandle = util::handle_vector<RigidBodyLifeSpan>::element_handler;

		static constexpr glm::vec<3, Float> default_gravity{0, -10, 0};

		std::unique_ptr<btCollisionConfiguration> _collision_configuration;
		std::unique_ptr<btDispatcher> _dispatcher;
		std::unique_ptr<btBroadphaseInterface> _broadphase;
		std::unique_ptr<btConstraintSolver> _constraint_solver;
		std::unique_ptr<btDynamicsWorld> _dynamics_world;

		util::handle_vector<RigidBodyLifeSpan> _rigid_body_life_spans;

		[[nodiscard]] RigidBodyLifeSpanHandle internalAddRigidBody(RigidBody& rigid_body) noexcept;

		void onPhysicsUpdate() override;

	public:
		[[nodiscard]] PhysicsContext() noexcept;


	}; // class PhysicsContext

} // namespace infd::scene::physics

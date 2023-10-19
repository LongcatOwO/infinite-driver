#pragma once

// std
#include <bitset>
#include <format>

// project
#include <opengl.hpp>

// project - util
#include <infd/util/exceptions.hpp>
#include <infd/util/Function.hpp>

// project - infd::scene
#include <infd/scene/Scene.hpp>

// project - infd::scene::physics
#include <infd/scene/physics/physics.hpp>

namespace infd::scene {

	class KeyboardInputRigidBodyController : public scene::Component {
		using RigidBody = physics::RigidBody;

		Scene* _scene = nullptr;

		RigidBody* _rigid_body = nullptr;
		Float _acceleration = 30;
		Float _turn_acceleration = 10;
		
		enum Direction {
			Forward = 0,
			Backward,
			Leftward,
			RightWard,
		};

		std::bitset<4> _moving{};

		char _stabilize_key = 'e';
		bool _is_stabilizing = false;
		Float _stabilizing_velocity = 20;

		void onAwake() override;

		void onDetach() override;

		void onPhysicsUpdate() override;

		void keyCallback(int key, int scancode, int action, int mods);

	private:
		Float acceleration() const noexcept {
			return _acceleration;
		}

		void acceleration(const Float& value) noexcept {
			_acceleration = value;
		}

		Float turn_acceleration() const noexcept {
			return _turn_acceleration;
		}

		void turn_acceleration(const Float& turn_acceleration) noexcept {
			_turn_acceleration = turn_acceleration;
		}

		char stabilizeKey() const noexcept {
			return _stabilize_key;
		}

		void stabilizeKey(char value) noexcept {
			_stabilize_key = value;
		}

		Float stabilizingVelocity() const noexcept {
			return _stabilizing_velocity;
		}

		void stabilizationVelocity(const Float& value) noexcept {
			_stabilizing_velocity = value;
		}
	}; // class KeyboardInputRigidBodyController

} // namespace infd::scene

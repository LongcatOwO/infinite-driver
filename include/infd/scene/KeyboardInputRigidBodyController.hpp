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

		RigidBody* _rigid_body = nullptr;
		Float _acceleration = 50;
		Float _turn_acceleration = 50;
		Float _acceleration_force;
		Float _turn_acceleration_force;
		
		std::bitset<4> _moving{};
		enum Direction {
			Forward = 0,
			Backward,
			Leftward,
			RightWard,
		};

		void onAwake() override;

		void onDetach() override;

		void onPhysicsUpdate() override;

		void keyCallback(int key, int scancode, int action, int mods);

		void updateAccelerationForce() {
			_acceleration_force = _acceleration * _rigid_body->mass();
		}

		void updateTurnAccelerationForce() {
			_turn_acceleration_force = _turn_acceleration * _rigid_body->mass();
		}

	private:
		Float acceleration() const noexcept {
			return _acceleration;
		}

		void acceleration(const Float& value) noexcept {
			_acceleration = value;
			updateAccelerationForce();
		}

		Float turn_acceleration() const noexcept {
			return _turn_acceleration;
		}

		void turn_acceleration(const Float& turn_acceleration) noexcept {
			_turn_acceleration = turn_acceleration;
			updateTurnAccelerationForce();
		}
	}; // class KeyboardInputRigidBodyController

} // namespace infd::scene

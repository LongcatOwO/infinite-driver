// Created by Phuwasate Lutchanont

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

// project - infd::scene::render
#include <infd/render/RenderComponent.hpp>

// project - infd::scene::physics
#include <infd/scene/physics/physics.hpp>

namespace infd::scene {

	class KeyboardInputRigidBodyController : public scene::Component {
		using RigidBody = physics::RigidBody;

		Scene* _scene = nullptr;
		render::CameraComponent* _camera = nullptr;

		RigidBody* _rigid_body = nullptr;
		Float _min_speed_to_turn = 5;
		Float _max_speed = 50;
		Float _max_turning_loss = 0.3;

		Float _acceleration = 50;
		Float _turn_acceleration = 10;
		
		enum Direction {
			Forward = 0,
			Backward,
			Leftward,
			RightWard,
		};

		std::bitset<4> _moving{};

		int _stabilize_roll_key = GLFW_KEY_E;
		bool _is_stabilizing_roll = false;
		int _stabilize_direction_key = GLFW_KEY_Q;
		bool _is_stabilizing_direction = false;

		Float _stabilizing_velocity = 20;

		void onAwake() override;

		void onDetach() override;

		void onPhysicsUpdate() override;

		void keyCallback(int key, int scancode, int action, int mods);
		
		void stabilizeRoll();

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

		int stabilizeRollKey() const noexcept {
			return _stabilize_roll_key;
		}

		void stabilizeRollKey(int value) noexcept {
			_stabilize_roll_key = value;
		}

		int stabilizeDirectionKey() const noexcept {
			return _stabilize_direction_key;
		}

		void stabilizeDirectionKey(int value) noexcept {
			_stabilize_direction_key = value;
		}

		Float stabilizingVelocity() const noexcept {
			return _stabilizing_velocity;
		}

		void stabilizationVelocity(const Float& value) noexcept {
			_stabilizing_velocity = value;
		}
	}; // class KeyboardInputRigidBodyController

} // namespace infd::scene

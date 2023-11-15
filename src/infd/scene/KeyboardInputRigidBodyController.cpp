// Created by Phuwasate Lutchanont

// glm
#include <glm/common.hpp>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/vector_angle.hpp>

// project
#include <infd/scene/KeyboardInputRigidBodyController.hpp>
#include <infd/render/RenderComponent.hpp>

namespace infd::scene {

	void KeyboardInputRigidBodyController::onAwake() {
		_rigid_body = getComponent<RigidBody>();
		if (!_rigid_body)
			throw util::InvalidStateException(std::format(
				"[KeyboardInputRigidBodyController(\"{}\")::onAwake()]: RigidBody not found",
				sceneObject().name()
			));

		_camera = scene().findComponent<render::CameraComponent>();
		if (!_camera)
			throw util::InvalidStateException(std::format(
				"[KeyboardInputRigidBodyController(\"{}\")::onAwake()]: Camera not found in Scene",
				sceneObject().name()
			));
		
		_scene = &scene();
		_scene->keyEvent() += util::BindedMemberFunc(&KeyboardInputRigidBodyController::keyCallback, *this);
	}

	void KeyboardInputRigidBodyController::onDetach() {
		_scene->keyEvent() -= util::BindedMemberFunc(&KeyboardInputRigidBodyController::keyCallback, *this);
	}

	void KeyboardInputRigidBodyController::onPhysicsUpdate() {
		if (_moving[Direction::Forward]) {
			Float acceleration_factor = glm::max(
				0.f,
				1.f - glm::dot(_rigid_body->linearVelocity(), transform().localFront()) / _max_speed
			);
			_rigid_body->applyCentralForce(
				transform().localFront() * _acceleration * acceleration_factor * _rigid_body->mass()
			);
		}

		if (_moving[Direction::Backward]) {
			Float acceleration_factor = glm::max(
				0.f,
				1.f - glm::dot(_rigid_body->linearVelocity(), -transform().localFront()) / _max_speed
			);
			_rigid_body->applyCentralForce(
				-transform().localFront() * _acceleration * acceleration_factor * _rigid_body->mass()
			);
		}

		if (_moving[Direction::Leftward]) {
			Float speed = glm::length(_rigid_body->linearVelocity());
			if (speed >= _min_speed_to_turn) {
				Float speed_ratio = glm::clamp(speed / _max_speed, 0.f, 1.f);
				Float acceleration_ratio = glm::mix(1.f, 1.f - _max_turning_loss, speed_ratio);
				_rigid_body->applyTorque(
					transform().localUp() * _turn_acceleration * acceleration_ratio * _rigid_body->mass()
				);
			}
		}

		if (_moving[Direction::RightWard]) {
			Float speed = glm::length(_rigid_body->linearVelocity());
			if (speed >= _min_speed_to_turn) {
				Float speed_ratio = glm::clamp(speed / _max_speed, 0.f, 1.f);
				Float acceleration_ratio = glm::mix(1.f, 1.f - _max_turning_loss, speed_ratio);
				_rigid_body->applyTorque(
					-transform().localUp() * _turn_acceleration * acceleration_ratio * _rigid_body->mass()
				);
			}
		}

		stabilizeRoll();
	}

	void KeyboardInputRigidBodyController::keyCallback(int key, int scancode, int action, int mods) {
		(void) scancode, (void) mods;
		switch (key) {
			case GLFW_KEY_W: _moving[Direction::Forward] 	= action != GLFW_RELEASE; break;
			case GLFW_KEY_S: _moving[Direction::Backward] 	= action != GLFW_RELEASE; break;
			case GLFW_KEY_A: _moving[Direction::Leftward] 	= action != GLFW_RELEASE; break;
			case GLFW_KEY_D: _moving[Direction::RightWard] 	= action != GLFW_RELEASE; break;
			default: {
				if (key == _stabilize_roll_key)
					_is_stabilizing_roll = action != GLFW_RELEASE;
				else if (key == _stabilize_direction_key)
					_is_stabilizing_direction = action != GLFW_RELEASE;
			}
		}
	}

	void KeyboardInputRigidBodyController::stabilizeRoll() {
		glm::vec3 desired_right = glm::cross(transform().localFront(), glm::vec3{0, 1, 0});
		glm::vec3 stabilizing_dir = glm::cross(transform().localRight(), desired_right);
		float stabilizing_ratio = 
			glm::pow(glm::angle(transform().localRight(), desired_right) / glm::pi<float>(), 2);

		if (glm::length(stabilizing_dir) <= 1e-4f)
			stabilizing_dir = transform().localFront();
		else
			stabilizing_dir = glm::normalize(stabilizing_dir);

		if (_is_stabilizing_roll)
			_rigid_body->angularVelocity(
				stabilizing_dir * stabilizing_ratio * _stabilizing_velocity * _rigid_body->mass()
			);
		else
			_rigid_body->applyTorque(
				stabilizing_dir * stabilizing_ratio * _stabilizing_velocity * _rigid_body->mass()
			);
	}

} // namespace infd::scene

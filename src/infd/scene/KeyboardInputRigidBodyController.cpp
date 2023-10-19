// glm
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/vector_angle.hpp>

// project
#include <infd/scene/KeyboardInputRigidBodyController.hpp>

namespace infd::scene {

	void KeyboardInputRigidBodyController::onAwake() {
		_rigid_body = getComponent<RigidBody>();
		if (!_rigid_body)
			throw util::InvalidStateException(std::format(
				"[KeyboardInputRigidBodyController(\"{}\")::onAwake()]: RigidBody not found",
				sceneObject().name()
			));
		
		_scene = &scene();
		_scene->keyEvent() += util::BindedMemberFunc(&KeyboardInputRigidBodyController::keyCallback, *this);
	}

	void KeyboardInputRigidBodyController::onDetach() {
		_scene->keyEvent() -= util::BindedMemberFunc(&KeyboardInputRigidBodyController::keyCallback, *this);
	}

	void KeyboardInputRigidBodyController::onPhysicsUpdate() {
		if (_moving[Direction::Forward])
			_rigid_body->applyCentralForce(transform().localFront() * _acceleration * _rigid_body->mass());

		if (_moving[Direction::Backward])
			_rigid_body->applyCentralForce(-transform().localFront() * _acceleration * _rigid_body->mass());

		if (_moving[Direction::Leftward])
			_rigid_body->applyTorque(transform().localUp() * _turn_acceleration * _rigid_body->mass());

		if (_moving[Direction::RightWard])
			_rigid_body->applyTorque(-transform().localUp() * _turn_acceleration * _rigid_body->mass());

		glm::vec3 desired_right = glm::cross(transform().localFront(), glm::vec3{0, 1, 0});
		glm::vec3 stabilizing_dir = glm::cross(transform().localRight(), desired_right);
		float stabilizing_ratio = 
			glm::pow(glm::angle(transform().localRight(), desired_right) / glm::pi<float>(), 2);

		if (glm::length(stabilizing_dir) <= 1e-4f)
			stabilizing_dir = transform().localFront();
		else
			stabilizing_dir = glm::normalize(stabilizing_dir);

		if (_is_stabilizing)
			_rigid_body->angularVelocity(
				stabilizing_dir * stabilizing_ratio * _stabilizing_velocity * _rigid_body->mass()
			);
		else
			_rigid_body->applyTorque(
				stabilizing_dir * stabilizing_ratio * _stabilizing_velocity / 2.f * _rigid_body->mass()
			);
	}

	void KeyboardInputRigidBodyController::keyCallback(int key, int scancode, int action, int mods) {
		(void) scancode, (void) mods;
		switch (key) {
		break; case GLFW_KEY_W: _moving[Direction::Forward] 	= action != GLFW_RELEASE;
		break; case GLFW_KEY_S: _moving[Direction::Backward] 	= action != GLFW_RELEASE;
		break; case GLFW_KEY_A: _moving[Direction::Leftward] 	= action != GLFW_RELEASE;
		break; case GLFW_KEY_D: _moving[Direction::RightWard] 	= action != GLFW_RELEASE;
		break; case GLFW_KEY_E: _is_stabilizing 				= action != GLFW_RELEASE;
		}
	}


} // namespace infd::scene

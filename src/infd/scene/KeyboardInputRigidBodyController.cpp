#include <infd/scene/KeyboardInputRigidBodyController.hpp>

namespace infd::scene {

	void KeyboardInputRigidBodyController::onAwake() {
		_rigid_body = getComponent<RigidBody>();
		if (!_rigid_body)
			throw util::InvalidStateException(std::format(
				"[KeyboardInputRigidBodyController(\"{}\")::onAwake()]: RigidBody not found",
				sceneObject().name()
			));

		updateAccelerationForce();
		updateTurnAccelerationForce();
		
		scene().keyEvent() += util::BindedMemberFunc(&KeyboardInputRigidBodyController::keyCallback, *this);
	}

	void KeyboardInputRigidBodyController::onDetach() {
		scene().keyEvent() -= util::BindedMemberFunc(&KeyboardInputRigidBodyController::keyCallback, *this);
	}

	void KeyboardInputRigidBodyController::onPhysicsUpdate() {
		if (_moving[Direction::Forward])
			_rigid_body->applyCentralForce(transform().localFront() * _acceleration_force);

		if (_moving[Direction::Backward])
			_rigid_body->applyCentralForce(-transform().localFront() * _acceleration_force);

		if (_moving[Direction::Leftward])
			_rigid_body->applyTorque(transform().localUp() * _turn_acceleration_force);

		if (_moving[Direction::RightWard])
			_rigid_body->applyTorque(-transform().localUp() * _turn_acceleration_force);
	}

	void KeyboardInputRigidBodyController::keyCallback(int key, int scancode, int action, int mods) {
		(void) scancode, (void) mods;
		switch (key) {
		break; case GLFW_KEY_W: _moving[Direction::Forward] 	= action == GLFW_PRESS;
		break; case GLFW_KEY_S: _moving[Direction::Backward] 	= action == GLFW_PRESS;
		break; case GLFW_KEY_A: _moving[Direction::Leftward] 	= action == GLFW_PRESS;
		break; case GLFW_KEY_D: _moving[Direction::RightWard] 	= action == GLFW_PRESS;
		}
	}


} // namespace infd::scene

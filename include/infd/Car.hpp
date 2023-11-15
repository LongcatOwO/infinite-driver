// Created by Phuwasate Lutchanont

#pragma once

// std
#include <format>

// project
#include <opengl.hpp>
#include <infd/scene/Scene.hpp>
#include <infd/scene/physics/physics.hpp>
#include <infd/util/exceptions.hpp>
#include <infd/util/Function.hpp>

namespace infd {

	class Car : public scene::Component {
		using RigidBody = scene::physics::RigidBody;

		RigidBody* _rigid_body;

		bool _moving_forward = false;
		bool _moving_backward = false;
		bool _moving_leftward = false;
		bool _moving_rightward = false;

		void onAwake() override {
			_rigid_body = getComponent<RigidBody>();
			if (!_rigid_body)
				_rigid_body = &emplaceComponent<RigidBody>();

			scene().keyEvent() += util::BindedMemberFunc(&Car::keyCallback, *this);
		}

		void onDetach() override {
			scene().keyEvent() -= util::BindedMemberFunc(&Car::keyCallback, *this);
		}

		void onPhysicsUpdate() override {
		}

		void keyCallback(int key, int scancode, int action, int mods) {
			(void) scancode, (void) mods;
			switch (key) {
			break; case GLFW_KEY_W: _moving_forward 	= action == GLFW_PRESS; 
			break; case GLFW_KEY_S: _moving_backward 	= action == GLFW_PRESS;
			break; case GLFW_KEY_A: _moving_leftward 	= action == GLFW_PRESS;
			break; case GLFW_KEY_D: _moving_rightward 	= action == GLFW_PRESS;
			}
		}

	}; // class Car

} // namespace infd

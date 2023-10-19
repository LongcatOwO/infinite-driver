// std
#include <format>

// glm
#include <glm/exponential.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/trigonometric.hpp>
#include <glm/vec3.hpp>

// project
#include <opengl.hpp>
#include <infd/scene/LookAtParent.hpp>
#include <infd/util/exceptions.hpp>
#include <infd/util/Function.hpp>

namespace infd::scene {

	void LookAtParent::onAwake() {
		if (!sceneObject().parent())
			throw util::InvalidStateException(std::format(
				"[LookAtParent(\"{}\")::onAwake()]: SceneObject must have parent",
				sceneObject().name()
			));
		_scene = &scene();

		_scene->cursorPosEvent() += util::BindedMemberFunc(&LookAtParent::cursorPosCallback, *this);
		_scene->mouseButtonEvent() += util::BindedMemberFunc(&LookAtParent::mouseButtonCallback, *this);
		_scene->scrollEvent() += util::BindedMemberFunc(&LookAtParent::scrollCallback, *this);
		updateTransform();
	}

	void LookAtParent::onDetach() {
		_scene->cursorPosEvent() -= util::BindedMemberFunc(&LookAtParent::cursorPosCallback, *this);
		_scene->mouseButtonEvent() -= util::BindedMemberFunc(&LookAtParent::mouseButtonCallback, *this);
		_scene->scrollEvent() -= util::BindedMemberFunc(&LookAtParent::scrollCallback, *this);
	}

	void LookAtParent::onFrameUpdate() {
		updateTransform();
	}

	void LookAtParent::updateTransform() noexcept {
		Float y = glm::sin(_pitch);
		Float horizontal = glm::cos(_pitch);
		Float x = glm::sin(_yaw) * horizontal;
		Float z = glm::cos(_yaw) * horizontal;
		transform().localPosition(glm::vec<3, Float>{x, y, z} * distanceFromParent());
		transform().localRotation(glm::quatLookAt(-glm::vec<3, Float>{x, y, z}, {0, 1, 0}));
	}

	void LookAtParent::cursorPosCallback(double x_pos, double y_pos) noexcept {
		if (!_cursor_pos_init) {
			_cursor_pos_init = true;
			_last_x = x_pos;
			_last_y = y_pos;
			return;
		}

		if (_right_click_down) {
			double x_dif = x_pos - _last_x;
			double y_dif = y_pos - _last_y;

			_yaw 	-= static_cast<Float>(x_dif * _sensitivity_x * glm::half_pi<double>());
			_pitch 	+= static_cast<Float>(y_dif * _sensitivity_y * glm::half_pi<double>());
		}

		_last_x = x_pos;
		_last_y = y_pos;
	}

	void LookAtParent::mouseButtonCallback(int button, int action, int mods) noexcept {
		(void) mods;
		if (button == GLFW_MOUSE_BUTTON_RIGHT)
			_right_click_down = action == GLFW_PRESS;
	}

	void LookAtParent::scrollCallback(double x_offset, double y_offset) noexcept {
		(void) x_offset;
		_distance_from_parent *= glm::pow(_zoom_factor, -y_offset);
	}

} // namespace infd::scene

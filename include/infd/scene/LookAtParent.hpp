#pragma once

#include <infd/scene/Scene.hpp>

namespace infd::scene {

	class LookAtParent : public Component {
		Float _distance_from_parent = 50.f;
		Float _yaw = 0;
		Float _pitch = 0;
		double _last_x, _last_y;
		double _cursor_pos_init = false;
		double _sensitivity_x = 0.001;
		double _sensitivity_y = 0.001;
		double _zoom_factor = 1.05;

		bool _right_click_down = false;

		void onAwake() override;
		void onDetach() override;
		void onFrameUpdate() override;

		void updateTransform() noexcept;
		void cursorPosCallback(double x_pos, double y_pos) noexcept;
		void mouseButtonCallback(int button, int action, int mods) noexcept;
		void scrollCallback(double x_offset, double y_offset) noexcept;

	public:
		[[nodiscard]] Float distanceFromParent() const noexcept {
			return _distance_from_parent;
		}

		void distanceFromParent(const Float& value) noexcept {
			_distance_from_parent = value;
		}

		[[nodiscard]] double sensitivityX() const noexcept {
			return _sensitivity_x;
		}

		[[nodiscard]] double sensitivityY() const noexcept {
			return _sensitivity_y;
		}

		void sensitivityX(double value) noexcept {
			_sensitivity_x = value;
		}

		void sensitivityY(double value) noexcept {
			_sensitivity_y = value;
		}

		[[nodiscard]] Float yaw() const noexcept {
			return _yaw;
		}

		[[nodiscard]] Float pitch() const noexcept {
			return _pitch;
		}

		void yaw(const Float& value) noexcept {
			_yaw = value;
		}

		void pitch(const Float& value) noexcept {
			_pitch = value;
		}

	}; // class LookAtParent

} // namespace infd::scene

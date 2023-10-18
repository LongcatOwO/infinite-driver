#pragma once

// glm
#include <glm/vec3.hpp>

// project
#include <infd/scene/Scene.hpp>

namespace infd::scene {

	class FollowTransform : public Component {
		glm::vec<3, Float> _position_offset{0, 15.f, 0};

		Transform* _to_follow = nullptr;

		void onAwake() override;
		void onFrameUpdate() override;

	public:
		Transform* toFollow() const noexcept {
			return _to_follow;
		}

		void toFollow(Transform* to_follow) noexcept {
			_to_follow = to_follow;
		}

		glm::vec<3, Float> positionOffset() const noexcept {
			return _position_offset;
		}

		void positionOffset(const glm::vec<3, Float>& value) noexcept {
			_position_offset = value;
		}

	}; // class FollowSceneObject

} // namespace scene::physics

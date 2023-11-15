// Created by Phuwasate Lutchanont

// std
#include <format>

// project
#include <infd/scene/FollowTransform.hpp>
#include <infd/util/exceptions.hpp>

namespace infd::scene {

	void FollowTransform::onAwake() {
		if (sceneObject().parent())
			throw util::InvalidStateException(std::format(
				"[FollowTransform(\"{}\")::onAwake()]: Component should be placed on root object.",
				sceneObject().name()
			));
	}

	void FollowTransform::onFrameUpdate() {
		if (!_to_follow) return;
		transform().localPosition(_to_follow->globalPosition() + _position_offset);
	}

} // namespace infd::scene

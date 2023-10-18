#pragma once

// bullet
#include <LinearMath/btMotionState.h>
#include <LinearMath/btScalar.h>
#include <LinearMath/btTransform.h>

// project - math
#include <infd/math/glm_bullet.hpp>

// project - scene
#include <infd/scene/Scene.hpp>

namespace infd::scene::physics {

	ATTRIBUTE_ALIGNED16(class)
	TransformMotionState : public btMotionState {
		Transform* _transform;

	public:
		TransformMotionState(Transform& transform) noexcept : _transform(&transform) {}

		void getWorldTransform(btTransform& transform) const noexcept override {
			transform = math::toBullet(_transform->localTransform());
		}

		void setWorldTransform(const btTransform& transform) noexcept override {
			_transform->localPosition(math::toGlm(transform.getOrigin()));
			_transform->localRotation(math::toGlm(transform.getRotation()));
		}

	}; // TransformMotionState

} // namespace infd::scene::physics

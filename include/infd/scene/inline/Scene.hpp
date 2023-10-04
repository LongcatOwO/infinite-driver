#pragma once

// std
#include <cassert>
#include <format>
#include <utility>

// project - math
#include <infd/math/DynamicRatio.hpp>

// project - util
#include <infd/util/exceptions.hpp>

// project - scene
#include <infd/scene/definitions.hpp>
#include <infd/util/Event.hpp>
#include <infd/scene/PhysicsContext.hpp>
#include <infd/scene/Timer.hpp>

// declarations
#include <infd/scene/decl/Scene.hpp>
#include <infd/scene/decl/SceneObject.hpp>


namespace infd::scene {

	inline Scene::Scene(std::string name) noexcept : _name(std::move(name)) {
		_frame_timer.onIntervalComplete() += [this](Timer &) {
			frameUpdate();
			_on_frame_update(*this);
		};

		_physics_timer.onIntervalComplete() += [this](Timer &) {
			physicsUpdate();
			_on_physics_update(*this);
		};
	}

	inline Float Scene::frameRate() const noexcept {
		return _frame_timer.rate();
	}

	inline void Scene::frameRate(Float value) noexcept {
		_frame_timer.rate(value);
	}

	inline Float Scene::physicsRate() const noexcept {
		return _physics_timer.rate();
	}

	inline void Scene::physicsRate(Float value) noexcept {
		_physics_timer.rate(value);
	}

	inline void Scene::resetTime() noexcept {
		_frame_timer.reset();
		_physics_timer.reset();
	}

	inline void Scene::enableTime() noexcept {
		_frame_timer.enable();
		_physics_timer.enable();
	}

	inline void Scene::disableTime() noexcept {
		_frame_timer.disable();
		_physics_timer.disable();
	}

	inline bool Scene::isTimeEnabled() const noexcept {
		assert(_frame_timer.isEnabled() == _physics_timer.isEnabled());
		return _frame_timer.isEnabled();
	}

	inline void Scene::isTimeEnabled(bool value) noexcept {
		_frame_timer.isEnabled(value);
		_physics_timer.isEnabled(value);
	}

	inline void Scene::updateTime() {
		_frame_timer.update();
		_physics_timer.update();
	}

	inline Float Scene::frameDeltaTime() const noexcept {
		return _frame_timer.deltaTime();
	}

	inline Float Scene::physicsDeltaTime() const noexcept {
		return _physics_timer.deltaTime();
	}

	inline PhysicsContext& Scene::physicsContext() noexcept {
		return _physics_context;
	}

	inline const PhysicsContext& Scene::physicsContext() const noexcept {
		return _physics_context;
	}

	inline util::PublicEvent<void (Scene &)>& Scene::onFrameUpdate() noexcept {
		return _on_frame_update;
	}

	inline util::PublicEvent<void (Scene &)>& Scene::onPhysicsUpdate() noexcept {
		return _on_physics_update;
	}

} // namespace infd::scene

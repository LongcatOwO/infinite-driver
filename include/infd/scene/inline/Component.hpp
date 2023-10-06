#pragma once

// std
#include <concepts>
#include <memory>
#include <utility>

// project - scene
#include <infd/scene/definitions.hpp>

// declarations
#include <infd/scene/decl/Component.hpp>
#include <infd/scene/decl/Scene.hpp>
#include <infd/scene/decl/SceneObject.hpp>
#include <infd/scene/decl/Transform.hpp>

namespace infd::scene {

	inline Scene& Component::scene() noexcept {
		return sceneObject().scene();
	}

	inline const Scene& Component::scene() const noexcept {
		return sceneObject().scene();
	}

	inline SceneObject& Component::sceneObject() noexcept {
		return *_scene_object;
	}

	inline const SceneObject& Component::sceneObject() const noexcept {
		return *_scene_object;
	}

	inline Transform& Component::transform() noexcept {
		return sceneObject().transform();
	}

	inline const Transform& Component::transform() const noexcept {
		return sceneObject().transform();
	}

	inline Float Component::frameDeltaTime() const noexcept {
		return scene().frameDeltaTime();
	}

	inline Float Component::physicsDeltaTime() const noexcept {
		return scene().physicsDeltaTime();
	}

	template <std::derived_from<Component> T>
	T& Component::addComponent(std::unique_ptr<T> component) noexcept {
		return sceneObject().addComponent(std::move(component));
	}

	template <std::derived_from<Component> T, typename ...Args>
	T& Component::emplaceComponent(Args &&...args) {
		return sceneObject().emplaceComponent<T>(std::forward<Args>(args)...);
	}

	template <typename T>
	T* Component::getComponent() noexcept {
		return sceneObject().getComponent<T>();
	}

	template <typename T>
	const T* Component::getComponent() const noexcept {
		return sceneObject().getComponent<T>();
	}

} // namespace infd::scene

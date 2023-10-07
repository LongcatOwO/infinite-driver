#pragma once

// std
#include <concepts>
#include <memory>
#include <utility>

// project - scene
#include <infd/scene/definitions.hpp>

// project - util
#include <infd/util/concepts.hpp>
#include <infd/util/exceptions.hpp>

// declarations
#include <infd/scene/decl/Component.hpp>
#include <infd/scene/decl/Scene.hpp>
#include <infd/scene/decl/SceneObject.hpp>
#include <infd/scene/decl/Transform.hpp>

namespace infd::scene {

	inline Scene& Component::scene() {
		return sceneObject().scene();
	}

	inline const Scene& Component::scene() const {
		return sceneObject().scene();
	}

	inline SceneObject& Component::sceneObject() {
		if (!_scene_object)
			throw util::InvalidStateException(
					"[Component(<NULL SceneObject>)::sceneObject()]: component is not attached to a SceneObject"
				);
		return *_scene_object;
	}

	inline const SceneObject& Component::sceneObject() const {
		if (!_scene_object)
			throw util::InvalidStateException(
					"[Component(<NULL SceneObject>)::sceneObject()]: component is not attached to a SceneObject"
				);
		return *_scene_object;
	}

	inline Transform& Component::transform() {
		return sceneObject().transform();
	}

	inline const Transform& Component::transform() const {
		return sceneObject().transform();
	}

	inline Float Component::frameDeltaTime() const {
		return scene().frameDeltaTime();
	}

	inline Float Component::physicsDeltaTime() const {
		return scene().physicsDeltaTime();
	}

	template <std::derived_from<Component> T>
	T& Component::addComponent(std::unique_ptr<T> component) {
		return sceneObject().addComponent(std::move(component));
	}

	template <std::derived_from<Component> T, typename ...Args>
	T& Component::emplaceComponent(Args &&...args) {
		return sceneObject().emplaceComponent<T>(std::forward<Args>(args)...);
	}

	template <typename T>
	T* Component::getComponent() {
		return sceneObject().getComponent<T>();
	}

	template <typename T>
	const T* Component::getComponent() const {
		return sceneObject().getComponent<T>();
	}

	template <typename T>
	util::view_of<T &> auto Component::getComponentsView() {
		return sceneObject().getComponentsView<T>();
	}

	template <typename T>
	util::view_of<const T &> auto Component::getComponentsView() const {
		return sceneObject().getComponentsView<T>();
	}

	template <typename T>
	std::vector<T *> Component::getComponentPointers() {
		return sceneObject().getComponentPointers<T>();
	}

	template <typename T>
	std::vector<const T *> Component::getComponentPointers() const {
		return sceneObject().getComponentPointers<T>();
	}

	template <typename T>
	util::random_access_move_only_range_of<T &> auto Component::getComponents() {
		return sceneObject().getComponents<T>();
	}

	template <typename T>
	util::random_access_move_only_range_of<const T &> auto Component::getComponents() const {
		return sceneObject().getComponents<T>();
	}

	template <typename T>
	T* Component::getComponentInChildren(bool include_self) {
		return sceneObject().getComponentInChildren<T>(include_self);
	}

	template <typename T>
	const T* Component::getComponentInChildren(bool include_self) const {
		return sceneObject().getComponentInChildren<T>(include_self);
	}

	template <typename T>
	std::vector<T *> Component::getComponentPointersInChildren(bool include_self) {
		return sceneObject().getComponentPointersInChildren<T>(include_self);
	}

	template <typename T>
	std::vector<const T *> Component::getComponentPointersInChildren(bool include_self) const {
		return sceneObject().getComponentPointersInChildren<T>(include_self);
	}

	template <typename T>
	util::random_access_move_only_range_of<T &> auto Component::getComponentsInChildren(bool include_self) {
		return sceneObject().getComponentsInChildren<T>(include_self);
	}

	template <typename T>
	util::random_access_move_only_range_of<const T &> auto Component::getComponentsInChildren(bool include_self) const {
		return sceneObject().getComponentsInChildren<T>(include_self);
	}

	inline std::unique_ptr<Component> Component::removeFromSceneObject() {
		return sceneObject().internalUncheckedRemoveComponent(*this);
	}

} // namespace infd::scene

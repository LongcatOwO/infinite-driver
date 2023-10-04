#pragma once

// std
#include <algorithm>
#include <concepts>
#include <format>
#include <memory>
#include <ranges>
#include <string>
#include <utility>
#include <vector>

// project - util
#include <infd/util/concepts.hpp>
#include <infd/util/exceptions.hpp>

// declarations
#include <infd/scene/decl/Component.hpp>
#include <infd/scene/decl/Scene.hpp>
#include <infd/scene/decl/SceneObject.hpp>
#include <infd/scene/decl/Transform.hpp>


namespace infd::scene {

	inline void SceneObject::internalAttachComponentRoutine(Component &component) {
		component._scene_object = this;
		component.onAttach();
	}

	inline SceneObject::SceneObject(std::string name) noexcept :
		_name(std::move(name)),
		_transform(&emplaceComponent<Transform>())
	{}

	inline std::string& SceneObject::name() noexcept {
		return _name;
	}

	inline const std::string& SceneObject::name() const noexcept {
		return _name;
	}

	inline bool SceneObject::isRoot() const noexcept {
		return !_parent;
	}

	inline bool SceneObject::hasParent() const noexcept {
		return !!_parent;
	}

	inline SceneObject* SceneObject::parent() noexcept {
		return _parent;
	}

	inline const SceneObject* SceneObject::parent() const noexcept {
		return _parent;
	}

	inline util::random_access_view_of<SceneObject *> auto SceneObject::childPointersView() noexcept {
		return 
			_children 
		  | std::views::transform([](auto &ptr) -> SceneObject* { return ptr.get(); });
	}

	inline util::random_access_view_of<const SceneObject *> auto SceneObject::childPointersView() const noexcept {
		return
			_children
		  | std::views::transform([](auto &ptr) -> const SceneObject* { return ptr.get(); });
	}

	inline std::vector<SceneObject *> SceneObject::childPointers() noexcept {
		auto view = childPointersView();
		return { view.begin(), view.end() };
	}

	inline std::vector<const SceneObject *> SceneObject::childPointers() const noexcept {
		auto view = childPointersView();
		return { view.begin(), view.end() };
	}

	inline util::random_access_move_only_range_of<SceneObject &> auto SceneObject::children() noexcept {
		return
			std::ranges::owning_view(childPointers())
		  | std::views::transform([](auto *ptr) -> SceneObject& { return *ptr; });
	}

	inline util::random_access_move_only_range_of<const SceneObject &> auto SceneObject::children() const noexcept {
		return
			std::ranges::owning_view(childPointers())
		  | std::views::transform([](auto *ptr) -> const SceneObject& { return *ptr; });
	}

	inline util::random_access_move_only_range_of<SceneObject &> auto SceneObject::allChildren() noexcept {
		return
			std::ranges::owning_view(allChildPointers())
		  | std::views::transform([](auto *ptr) -> SceneObject& { return *ptr; });
	}

	inline util::random_access_move_only_range_of<const SceneObject &> auto SceneObject::allChildren() const noexcept {
		return
			std::ranges::owning_view(allChildPointers())
		  | std::views::transform([](auto *ptr) -> const SceneObject& { return *ptr; });
	}

	inline bool SceneObject::hasChildren() const noexcept {
		return !_children.empty();
	}

	inline std::size_t SceneObject::numChildren() const noexcept {
		return _children.size();
	}

	inline bool SceneObject::hasScene() const noexcept {
		return !!_scene;
	}

	inline Scene& SceneObject::scene() {
		if (!_scene)
			throw util::InvalidStateException(
				std::format("[SceneObject(\"{}\")::scene()]: object is not attached to a scene.", name())
			);
		return *_scene;
	}

	inline const Scene& SceneObject::scene() const {
		if (!_scene)
			throw util::InvalidStateException(
				std::format("[SceneObject(\"{}\")::scene()]: object is not attached to a scene.", name())
			);
		return *_scene;
	}

	inline Transform& SceneObject::transform() noexcept {
		return *_transform;
	}

	inline const Transform& SceneObject::transform() const noexcept {
		return *_transform;
	}

	template <std::derived_from<Component> T>
	T& SceneObject::addComponent(std::unique_ptr<T> component) {
		T *comp_ptr = component.get();
		if (!comp_ptr)
			throw util::NullPointerException(
				std::format("[SceneObject(\"{}\")::addComponent(std::unique_ptr<T> component)]: component is null.", name())
			);
		_components.push_back(std::move(component));
		internalAttachComponentRoutine(*comp_ptr);
		return *comp_ptr;
	}

	template <std::derived_from<Component> T, typename ...Args>
	inline T& SceneObject::emplaceComponent(Args &&...args) {
		T *comp_ptr = new T(std::forward<Args>(args)...);
		_components.push_back(std::unique_ptr<T>(comp_ptr));
		internalAttachComponentRoutine(*comp_ptr);
		return *comp_ptr;
	}

	template <typename T>
	T* SceneObject::getComponent() noexcept {
		for (const auto &comp_ptr : _components) {
			if (auto ptr = dynamic_cast<T*>(comp_ptr.get()); ptr)
				return ptr;
		}

		return nullptr;
	}

	template <typename T>
	const T* SceneObject::getComponent() const noexcept {
		for (const auto &comp_ptr : _components) {
			if (auto ptr = dynamic_cast<const T*>(comp_ptr.get()); ptr)
				return ptr;
		}

		return nullptr;
	}

	inline void SceneObject::internalSetScene(Scene *scene) noexcept {
		_scene = scene;
		for (SceneObject &child : children())
			child._scene = scene;
	}

	inline void SceneObject::internalUncheckedSetParent(SceneObject *parent) noexcept {
		_parent = parent;
		if (parent) 
			internalSetScene(parent->_scene);
		else
			internalSetScene(nullptr);
	}

} // namespace infd::scene

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
#include <infd/util/Event.hpp>
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

	inline util::PublicEvent<void (SceneObject &self)>& SceneObject::onSceneAssigned() noexcept {
		return _on_scene_assigned;
	}

	inline util::PublicEvent<void (SceneObject &self)>& SceneObject::onSceneUnassigned() noexcept {
		return _on_scene_unassigned;
	}

	inline util::PublicEvent<void (SceneObject &self)>& SceneObject::onParentAssigned() noexcept {
		return _on_parent_assigned;
	}

	inline util::PublicEvent<void (SceneObject &self)>& SceneObject::onParentUnassigned() noexcept {
		return _on_parent_unassigned;
	}

	inline util::PublicEvent<void (SceneObject &self, SceneObject &child)>& SceneObject::onChildAdded() noexcept {
		return _on_child_added;
	}

	inline util::PublicEvent<void (SceneObject &self, SceneObject &child)>& SceneObject::onChildRemoved() noexcept {
		return _on_child_removed;
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

	inline bool SceneObject::hasChildren() const noexcept {
		return !_children.empty();
	}

	inline std::size_t SceneObject::numChildren() const noexcept {
		return _children.size();
	}

	inline util::random_access_view_of<SceneObject &> auto SceneObject::childrenView() noexcept {
		return 
			_children 
		  | std::views::transform([](std::unique_ptr<SceneObject> &ptr) -> SceneObject& { 
				return *ptr; 
			});
	}

	inline util::random_access_view_of<const SceneObject &> auto SceneObject::childrenView() const noexcept {
		return
			_children
		  | std::views::transform([](const std::unique_ptr<SceneObject> &ptr) -> const SceneObject& { 
				return *ptr; 
			});
	}

	inline std::vector<SceneObject *> SceneObject::childPointers() noexcept {
		auto view = 
			_children
		  | std::views::transform([](std::unique_ptr<SceneObject> &ptr) -> SceneObject* {
				return ptr.get();
			});
		return { view.begin(), view.end() };
	}

	inline std::vector<const SceneObject *> SceneObject::childPointers() const noexcept {
		auto view = 
			_children
		  | std::views::transform([](const std::unique_ptr<SceneObject> &ptr) -> const SceneObject* {
				return ptr.get();
			});
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

	template <util::consumer_of<SceneObject &> V>
	void SceneObject::visitChildren(V &&visitor) {
		for (SceneObject &child : childrenView())
			std::forward<V>(visitor)(child);
	}

	template <util::consumer_of<const SceneObject &> V>
	void SceneObject::visitChildren(V &&visitor) const {
		for (const SceneObject &child : childrenView())
			std::forward<V>(visitor)(child);
	}

	template <util::consumer_of<SceneObject &> V>
	void SceneObject::visitAllChildren(V &&visitor) {
		const auto recurse = [&visitor] (auto self, auto children_view) -> void {
			for (SceneObject &child : children_view) {
				std::forward<V>(visitor)(child);
				self(self, child.childrenView());
			}
		};
		recurse(recurse, childrenView());
	}

	template <util::consumer_of<const SceneObject &> V>
	void SceneObject::visitAllChildren(V &&visitor) const {
		const auto recurse = [&visitor] (auto self, auto children_view) -> void {
			for (const SceneObject &child : children_view) {
				std::forward<V>(visitor)(child);
				self(self, child.childrenView());
			}
		};
		recurse(recurse, childrenView());
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
			if (auto ptr = dynamic_cast<T *>(comp_ptr.get()); ptr)
				return ptr;
		}

		return nullptr;
	}

	template <typename T>
	const T* SceneObject::getComponent() const noexcept {
		for (const auto &comp_ptr : _components) {
			if (auto ptr = dynamic_cast<const T *>(comp_ptr.get()); ptr)
				return ptr;
		}

		return nullptr;
	}

	template <typename T>
	util::view_of<T &> auto SceneObject::getComponentsView() noexcept {
		return 
			_components
		  | std::views::filter([](std::unique_ptr<Component> &comp) -> bool {
				return !!dynamic_cast<T *>(comp.get());
			})
		  | std::views::transform([](std::unique_ptr<Component> &comp) -> T& {
				return *static_cast<T *>(comp.get());
			});
	}

	template <typename T>
	util::view_of<const T &> auto SceneObject::getComponentsView() const noexcept {
		return
			_components
		  | std::views::filter([](const std::unique_ptr<Component> &comp) -> bool {
				return !!dynamic_cast<const T *>(comp.get());
			})
		  | std::views::transform([](const std::unique_ptr<Component> &comp) -> const T& {
				return *static_cast<const T *>(comp.get());
			});
	}

	template <typename T>
	std::vector<T *> SceneObject::getComponentPointers() noexcept {
		auto view = 
			_components
		  | std::views::filter([](std::unique_ptr<Component> &comp) -> bool {
				return !!dynamic_cast<T *>(comp.get());
			})
		  | std::views::transform([](std::unique_ptr<Component> &comp) -> T* {
			  return static_cast<T *>(comp.get());
			});

		return { view.begin(), view.end() };
	}

	template <typename T>
	std::vector<const T *> SceneObject::getComponentPointers() const noexcept {
		auto view = 
			_components
		  | std::views::filter([](const std::unique_ptr<Component> &comp) -> bool {
				return !!dynamic_cast<const T *>(comp.get());
			})
		  | std::views::transform([](const std::unique_ptr<Component> &comp) -> const T* {
				return static_cast<const T *>(comp.get());
			});

		return { view.begin(), view.end() };
	}

	template <typename T>
	util::random_access_move_only_range_of<T &> auto SceneObject::getComponents() noexcept {
		return 
			std::ranges::owning_view(getComponentPointers<T>())
		  | std::views::transform([](T *ptr) -> T& { return *ptr; });
	}

	template <typename T>
	util::random_access_move_only_range_of<const T &> auto SceneObject::getComponents() const noexcept {
		return
			std::ranges::owning_view(getComponentPointers<T>())
		  | std::views::transform([](const T *ptr) -> const T& { return *ptr; });
	}

	template <typename T>
	T* SceneObject::getComponentInChildren(bool include_self) noexcept {
		if (include_self) {
			if (T *comp = getComponent<T>(); comp)
				return comp;
		}

		for (SceneObject &so : childrenView()) {
			if (T *ptr = so.getComponentInChildren<T>(true); ptr)
				return ptr;
		}

		return nullptr;
	}

	template <typename T>
	const T* SceneObject::getComponentInChildren(bool include_self) const noexcept {
		if (include_self) {
			if (T *comp = getComponent<T>(); comp)
				return comp;
		}

		for (const SceneObject &so : childrenView()) {
			if (const T *ptr = so.getComponentInChildren<T>(true); ptr)
				return ptr;
		}

		return nullptr;
	}

	template <typename T>
	std::vector<T *> SceneObject::getComponentPointersInChildren(bool include_self) noexcept {
		namespace vw = std::views;

		std::vector<T *> result;

		if (include_self) {
			auto view = getComponentsView<T>() 
				| vw::transform([](T &comp) -> T* { return &comp; })
				| vw::common;
			result.insert(result.end(), view.begin(), view.end());
		}

		const auto recurse = [&result] (auto self, auto children_view) -> void {
			auto children_comp = children_view
				| vw::transform([](SceneObject &child) { return child.getComponentsView<T>(); })
				| vw::join
				| vw::transform([](T &comp) -> T* { return &comp; })
				| vw::common;
			result.insert(result.end(), children_comp.begin(), children_comp.end());

			for (SceneObject &child : children_view)
				self(self, child.childrenView());
		};

		recurse(recurse, childrenView());

		return result;
	}

	template <typename T>
	std::vector<const T *> SceneObject::getComponentPointersInChildren(bool include_self) const noexcept {
		namespace vw = std::views;

		std::vector<const T *> result;

		if (include_self) {
			auto view = getComponentsView<T>()
				| vw::transform([](const T &comp) -> const T* { return &comp; })
				| vw::common;
			result.insert(result.end(), view.begin(), view.end());
		}

		const auto recurse = [&result] (auto self, auto children_view) -> void {
			auto children_comp = children_view
				| vw::transform([](const SceneObject &child) { return child.getComponentsView<T>(); })
				| vw::join
				| vw::transform([](const T &comp) -> const T* { return &comp; })
				| vw::common;
			result.insert(result.end(), children_comp.begin(), children_comp.end());

			for (const SceneObject &child : children_view)
				self(self, child.childrenView());
		};

		recurse(recurse, childrenView());
		
		return result;
	}
	
	template <typename T>
	util::random_access_move_only_range_of<T &> auto SceneObject::getComponentsInChildren(bool include_self) noexcept {
		return std::ranges::owning_view(getComponentPointersInChildren<T>(include_self))
			| std::views::transform([](T *ptr) -> T& { return *ptr; });
	}

	template <typename T>
	util::random_access_move_only_range_of<const T &> auto SceneObject::getComponentsInChildren(bool include_self) const noexcept {
		return std::ranges::owning_view(getComponentPointersInChildren<T>(include_self))
			| std::views::transform([](const T *ptr) -> const T& { return *ptr; });
	}

} // namespace infd::scene

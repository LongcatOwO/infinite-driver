// Created by Phuwasate Lutchanont

#pragma once

// std
#include <concepts>
#include <memory>
#include <string>
#include <vector>

// project - util
#include <infd/util/concepts.hpp>
#include <infd/util/Event.hpp>
#include <infd/util/exceptions.hpp>

// forward declarations
#include <infd/scene/fwd/Component.hpp>
#include <infd/scene/fwd/Scene.hpp>
#include <infd/scene/fwd/SceneObject.hpp>
#include <infd/scene/fwd/Transform.hpp>

namespace infd::scene {
	class SceneObject final {
		friend class Scene;
		friend class Component;

		std::string _name;

		Scene *_scene = nullptr;

		SceneObject *_parent = nullptr;
		std::vector<std::unique_ptr<SceneObject>> _children;

		std::vector<std::unique_ptr<Component>> _components;
		Transform *_transform;

		// called after scene assigned, not called when detached from scene after the change
		util::Event<void (SceneObject &self)> _on_scene_assigned;
		
		// called before scene unassigned, not called when detached from scene before the change
		util::Event<void (SceneObject &self)> _on_scene_unassigned;

		// called after parent assigned, not called when detached from parent after the change
		util::Event<void (SceneObject &self)> _on_parent_assigned;

		// called before parent unassigned, not called when detached from parent before the change
		util::Event<void (SceneObject &self)> _on_parent_unassigned;

		// called after child added
		util::Event<void (SceneObject &self, SceneObject &child)> _on_child_added;

		// called before child removed
		util::Event<void (SceneObject &self, SceneObject &child)> _on_child_removed;

		// called after component added
		util::Event<void (SceneObject &self, Component &comp)> _on_component_added;

		// called before component removed
		util::Event<void (SceneObject &self, Component &comp)> _on_component_removed;

	public:
		[[nodiscard]] SceneObject(std::string name) noexcept;
		~SceneObject();

		 SceneObject(const SceneObject &) = delete;
		 SceneObject& operator=(const SceneObject &) = delete;
		 SceneObject(SceneObject &&) = delete;
		 SceneObject& operator=(SceneObject &&) = delete;

		[[nodiscard]] std::string& name() noexcept;
		[[nodiscard]] const std::string& name() const noexcept;

		// called after scene is changed, not called if detached from scene after scene changed
		[[nodiscard]] util::PublicEvent<void (SceneObject &self)>& onSceneAssigned() noexcept;

		// called before scene is changed, not called if detached from scene before scene changed
		[[nodiscard]] util::PublicEvent<void (SceneObject &self)>& onSceneUnassigned() noexcept;

		// called after parent is changed, not called if detached from parent after the change
		[[nodiscard]] util::PublicEvent<void (SceneObject &self)>& onParentAssigned() noexcept;
		
		// called before parent is changed, not called if detached from parent before the change
		[[nodiscard]] util::PublicEvent<void (SceneObject &self)>& onParentUnassigned() noexcept;

		// called after the child is added
		[[nodiscard]] util::PublicEvent<void (SceneObject &self, SceneObject &child)>& onChildAdded() noexcept;

		// called before the child is removed
		[[nodiscard]] util::PublicEvent<void (SceneObject &self, SceneObject &child)>& onChildRemoved() noexcept;

		// called after the component is added
		[[nodiscard]] util::PublicEvent<void (SceneObject &self, Component &component)>& onComponentAdded() noexcept;

		// called before the component is removed
		[[nodiscard]] util::PublicEvent<void (SceneObject &self, Component &component)>& onComponentRemoved() noexcept;

		[[nodiscard]] bool isRoot() const noexcept;
		[[nodiscard]] bool hasParent() const noexcept;

		[[nodiscard]] SceneObject* parent() noexcept;
		[[nodiscard]] const SceneObject* parent() const noexcept;

		[[nodiscard]] bool hasChildren() const noexcept;
		[[nodiscard]] std::size_t numChildren() const noexcept;

		[[nodiscard]] util::random_access_view_of<SceneObject &> auto childrenView() noexcept;
		[[nodiscard]] util::random_access_view_of<const SceneObject &> auto childrenView() const noexcept;

		[[nodiscard]] std::vector<SceneObject *> childPointers() noexcept;
		[[nodiscard]] std::vector<const SceneObject *> childPointers() const noexcept;
		[[nodiscard]] std::vector<SceneObject *> allChildPointers() noexcept;
		[[nodiscard]] std::vector<const SceneObject *> allChildPointers() const noexcept;

		[[nodiscard]] util::random_access_move_only_range_of<SceneObject &> auto children() noexcept;
		[[nodiscard]] util::random_access_move_only_range_of<const SceneObject &> auto children() const noexcept;
		[[nodiscard]] util::random_access_move_only_range_of<SceneObject &> auto allChildren() noexcept;
		[[nodiscard]] util::random_access_move_only_range_of<const SceneObject &> auto allChildren() const noexcept;

		template <util::consumer_of<SceneObject &> V>
		void visitChildren(V &&visitor);

		template <util::consumer_of<const SceneObject &> V>
		void visitChildren(V &&visitor) const;

		template <util::consumer_of<SceneObject &> V>
		void visitAllChildren(V &&visitor);

		template <util::consumer_of<const SceneObject &> V>
		void visitAllChildren(V &&visitor) const;

		SceneObject& addChild(std::string name) noexcept;

		/*
		 * @throws infd::util::CyclicReferenceException if child is parent of this object.
		 * @throws infd::util::NullPointerException if child is null
		 */
		SceneObject& addChild(std::unique_ptr<SceneObject> child);
		
		/*
		 * @throws infd::util::InvalidStateException if this object has no parent.
		 */
		[[nodiscard]] std::unique_ptr<SceneObject> removeFromParent();

		/*
		 * @throws infd::util::CyclicReferenceException if new_parent is child of this object.
		 * @throws infd::util::InvalidStateException if this object has no parent.
		 */
		void removeFromParent(SceneObject &new_parent);

		/*
		 * Preserves the global transform at default.
		 */
		SceneObject& attachChild(std::string name) noexcept;

		/*
		 * Preserves the global transform.
		 *
		 * @throws infd::util::CyclicReferenceException if child is parent of this object.
		 * @throws infd::util::NullPointerException if child is null
		 */
		SceneObject& attachChild(std::unique_ptr<SceneObject> child);

		/*
		 * Preserves the global transform.
		 *
		 * @throws infd::util::InvalidStateException if parent is null
		 */
		[[nodiscard]] std::unique_ptr<SceneObject> detachFromParent();

		/*
		 * Preserves the global transform.
		 *
		 * @throws infd::util::CyclicReferenceException if new_parent is child of this object.
		 * @throws infd::util::InvalidStateException if parent is null
		 */
		void detachFromParent(SceneObject &new_parent);

		[[nodiscard]] bool hasScene() const noexcept;

		/*
		 * @throws infd::util::InvalidStateException if this object is not attached to a scene.
		 */
		[[nodiscard]] Scene& scene();
		/*
		 * @throws infd::util::InvalidStateException if this object is not attached to a scene.
		 */
		[[nodiscard]] const Scene& scene() const;

		/*
		 * @throws infd::util::InvalidStateException if this object is not attached to a scene, 
		 * or if the object is not a root in the scene.
		 */
		[[nodiscard]] std::unique_ptr<SceneObject> removeFromScene();

		[[nodiscard]] Transform& transform() noexcept;
		[[nodiscard]] const Transform& transform() const noexcept;

		/*
		 * @param component cannot be null
		 * @throws infd::util::NullPointerException
		 */
		template <std::derived_from<Component> T>
		T& addComponent(std::unique_ptr<T> component);

		template <std::derived_from<Component> T, typename ...Args>
		T& emplaceComponent(Args &&...args);


		template <typename T>
		[[nodiscard]] T* getComponent() noexcept;

		template <typename T>
		[[nodiscard]] const T* getComponent() const noexcept;

		template <typename T>
		[[nodiscard]] util::view_of<T &> auto getComponentsView() noexcept;

		template <typename T>
		[[nodiscard]] util::view_of<const T &> auto getComponentsView() const noexcept;

		template <typename T>
		[[nodiscard]] std::vector<T *> getComponentPointers() noexcept;

		template <typename T>
		[[nodiscard]] std::vector<const T *> getComponentPointers() const noexcept;

		template <typename T>
		[[nodiscard]] util::random_access_move_only_range_of<T &> auto getComponents() noexcept;

		template <typename T>
		[[nodiscard]] util::random_access_move_only_range_of<const T &> auto getComponents() const noexcept;

		template <typename T>
		[[nodiscard]] T* getComponentInChildren(bool include_self = true) noexcept;

		template <typename T>
		[[nodiscard]] const T* getComponentInChildren(bool include_self = true) const noexcept;

		template <typename T>
		[[nodiscard]] std::vector<T *> getComponentPointersInChildren(bool include_self = true) noexcept;

		template <typename T>
		[[nodiscard]] std::vector<const T *> getComponentPointersInChildren(bool include_self = true) const noexcept;

		template <typename T>
		[[nodiscard]] util::random_access_move_only_range_of<T &> auto getComponentsInChildren(bool include_self = true) noexcept;

		template <typename T>
		[[nodiscard]] util::random_access_move_only_range_of<const T &> auto getComponentsInChildren(bool include_self = true) const noexcept;

	private:
		void internalUncheckedNotifySceneUnassigned() noexcept;
		void internalUncheckedNotifySceneAssigned() noexcept;
		void internalUncheckedUnnotifiedSetScene(Scene *scene) noexcept;
		void internalSetScene(Scene *scene) noexcept;
		void internalUncheckedNotifyParentUnassigned() noexcept;
		void internalUncheckedNotifyParentAssigned() noexcept;
		void internalUncheckedUnnotifiedSetParent(SceneObject *parent) noexcept;
		void internalUncheckedAddChild(std::unique_ptr<SceneObject> child) noexcept;
		[[nodiscard]] std::unique_ptr<SceneObject> internalUncheckedRemoveChild(SceneObject &child) noexcept;
		void internalUncheckedRemoveChild(SceneObject &child, SceneObject &new_parent) noexcept;
		void internalUncheckedAddComponent(std::unique_ptr<Component> component) noexcept;
		[[nodiscard]] std::unique_ptr<Component> internalUncheckedRemoveComponent(Component &component) noexcept;
		void internalUncheckedRemoveComponent(Component &component, SceneObject &new_owner) noexcept;

		/*
		 * calls Component.onAwake() on all components in this object and all its children 
		 * recursively.
		 */
		void internalAwake();

		/* 
		 * calls Component.onFrameUpdate() on all components in this object and all its children
		 * recursively.
		 */
		void internalFrameUpdate();

		/*
		 * calls Component.onPhysicsUpdate() on all components in this object and all its children
		 * recursively.
		 */
		void internalPhysicsUpdate();

	}; // class SceneObject
} // namespace infd::scene

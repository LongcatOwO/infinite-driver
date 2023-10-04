#pragma once

// std
#include <memory>
#include <string>
#include <vector>

// project - util
#include <infd/util/concepts.hpp>
#include <infd/util/exceptions.hpp>

// forward declarations
#include <infd/scene/fwd/Component.hpp>
#include <infd/scene/fwd/Scene.hpp>
#include <infd/scene/fwd/SceneObject.hpp>
#include <infd/scene/fwd/Transform.hpp>

namespace infd::scene {
	class SceneObject final {
		friend class Scene;

		std::string _name;

		Scene *_scene = nullptr;

		SceneObject *_parent = nullptr;
		std::vector<std::unique_ptr<SceneObject>> _children;

		std::vector<std::unique_ptr<Component>> _components;
		Transform *_transform;

		void internalAttachComponentRoutine(Component &component);
	public:
		[[nodiscard]] SceneObject(std::string name) noexcept;

		[[nodiscard]] std::string& name() noexcept;
		[[nodiscard]] const std::string& name() const noexcept;

		[[nodiscard]] bool isRoot() const noexcept;
		[[nodiscard]] bool hasParent() const noexcept;

		[[nodiscard]] SceneObject* parent() noexcept;
		[[nodiscard]] const SceneObject* parent() const noexcept;

		[[nodiscard]] bool hasChildren() const noexcept;
		[[nodiscard]] std::size_t numChildren() const noexcept;

		[[nodiscard]] util::random_access_view_of<SceneObject *> auto childPointersView() noexcept;
		[[nodiscard]] util::random_access_view_of<const SceneObject *> auto childPointersView() const noexcept;

		[[nodiscard]] std::vector<SceneObject *> childPointers() noexcept;
		[[nodiscard]] std::vector<const SceneObject *> childPointers() const noexcept;
		[[nodiscard]] std::vector<SceneObject *> allChildPointers() noexcept;
		[[nodiscard]] std::vector<const SceneObject *> allChildPointers() const noexcept;

		[[nodiscard]] util::random_access_move_only_range_of<SceneObject &> auto children() noexcept;
		[[nodiscard]] util::random_access_move_only_range_of<const SceneObject &> auto children() const noexcept;
		[[nodiscard]] util::random_access_move_only_range_of<SceneObject &> auto allChildren() noexcept;
		[[nodiscard]] util::random_access_move_only_range_of<const SceneObject &> auto allChildren() const noexcept;

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

	private:
		void internalSetScene(Scene *scene) noexcept;
		void internalUncheckedSetParent(SceneObject *parent) noexcept;

	}; // class SceneObject
} // namespace infd::scene

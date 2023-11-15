// Created by Phuwasate Lutchanont

#pragma once

// std
#include <concepts>
#include <memory>

// project - scene
#include <infd/scene/definitions.hpp>

// project - util
#include <infd/util/concepts.hpp>
#include <infd/util/Timer.hpp>

// forward declarations
#include <infd/scene/fwd/Component.hpp>
#include <infd/scene/fwd/Scene.hpp>
#include <infd/scene/fwd/SceneObject.hpp>
#include <infd/scene/fwd/Transform.hpp>


namespace infd::scene {
	class Component {
		friend class SceneObject;
		SceneObject *_scene_object = nullptr;

	protected:
		// TODO call these functions properly

		// called after attached
		virtual void onAttach() {}

		// called before detached
		virtual void onDetach() {}

		// called once the scene is awaken
		virtual void onAwake() {}


		virtual void onFrameUpdate() {}
		virtual void onPhysicsUpdate() {}
		// virtual void onEnable() {}
		// virtual void onDisable() {}

	public:
		Component() noexcept = default;
		virtual ~Component() {}

		Component(const Component &) = delete;
		Component(Component &&) = delete;
		Component& operator=(const Component &) = delete;
		Component& operator=(Component &&) = delete;

		/*
		 * @throws InvalidStateException if this component is not attached to a SceneObject, 
		 * or if the SceneObject this component is attached to is not in a scene.
		 */
		[[nodiscard]] Scene& scene();

		/*
		 * @throws InvalidStateException if this component is not attached to a SceneObject, 
		 * or if the SceneObject this component is attached to is not in a scene.
		 */
		[[nodiscard]] const Scene& scene() const;

		/*
		 * @throws InvalidStateException if this component is not attached to a SceneObject.
		 */
		[[nodiscard]] SceneObject& sceneObject();

		/*
		 * @throws InvalidStateException if this component is not attached to a SceneObject.
		 */
		[[nodiscard]] const SceneObject& sceneObject() const;

		/*
		 * @throws InvalidStateException if this component is not attahced to a SceneObject.
		 */
		[[nodiscard]] Transform& transform();

		/*
		 * @throws InvalidStateException if this component is not attahced to a SceneObject.
		 */
		[[nodiscard]] const Transform& transform() const;

		/*
		 * @throws InvalidStateException if this component is not attached to a SceneObject, 
		 * or if the SceneObject this component is attached to is not in a scene.
		 */
		[[nodiscard]] util::Timer::Duration frameDeltaTime() const;

		/*
		 * @throws InvalidStateException if this component is not attached to a SceneObject, 
		 * or if the SceneObject this component is attached to is not in a scene.
		 */
		[[nodiscard]] util::Timer::Duration physicsDeltaTime() const;

		/*
		 * @throws InvalidStateException if this component is not attahced to a SceneObject.
		 */
		template <std::derived_from<Component> T>
		T& addComponent(std::unique_ptr<T> component);

		/*
		 * @throws InvalidStateException if this component is not attahced to a SceneObject.
		 */
		template <std::derived_from<Component> T, typename ...Args>
		T& emplaceComponent(Args &&...args);

		/*
		 * @throws InvalidStateException if this component is not attahced to a SceneObject.
		 */
		template <typename T>
		[[nodiscard]] T* getComponent();

		/*
		 * @throws InvalidStateException if this component is not attahced to a SceneObject.
		 */
		template <typename T>
		[[nodiscard]] const T* getComponent() const;

		/*
		 * @throws InvalidStateException if this component is not attahced to a SceneObject.
		 */
		template <typename T>
		[[nodiscard]] util::view_of<T &> auto getComponentsView();

		/*
		 * @throws InvalidStateException if this component is not attahced to a SceneObject.
		 */
		template <typename T>
		[[nodiscard]] util::view_of<const T &> auto getComponentsView() const;

		/*
		 * @throws InvalidStateException if this component is not attahced to a SceneObject.
		 */
		template <typename T>
		[[nodiscard]] std::vector<T *> getComponentPointers();

		/*
		 * @throws InvalidStateException if this component is not attahced to a SceneObject.
		 */
		template <typename T>
		[[nodiscard]] std::vector<const T *> getComponentPointers() const;

		/*
		 * @throws InvalidStateException if this component is not attahced to a SceneObject.
		 */
		template <typename T>
		[[nodiscard]] util::random_access_move_only_range_of<T &> auto getComponents();

		/*
		 * @throws InvalidStateException if this component is not attahced to a SceneObject.
		 */
		template <typename T>
		[[nodiscard]] util::random_access_move_only_range_of<const T &> auto getComponents() const;

		/*
		 * @throws InvalidStateException if this component is not attahced to a SceneObject.
		 */
		template <typename T>
		[[nodiscard]] T* getComponentInChildren(bool include_self = true);
		
		/*
		 * @throws InvalidStateException if this component is not attahced to a SceneObject.
		 */
		template <typename T>
		[[nodiscard]] const T* getComponentInChildren(bool include_self = true) const;

		/*
		 * @throws InvalidStateException if this component is not attahced to a SceneObject.
		 */
		template <typename T>
		[[nodiscard]] std::vector<T *> getComponentPointersInChildren(bool include_self = true);

		/*
		 * @throws InvalidStateException if this component is not attahced to a SceneObject.
		 */
		template <typename T>
		[[nodiscard]] std::vector<const T *> getComponentPointersInChildren(bool include_self = true) const;

		/*
		 * @throws InvalidStateException if this component is not attahced to a SceneObject.
		 */
		template <typename T>
		[[nodiscard]] util::random_access_move_only_range_of<T &> auto getComponentsInChildren(bool include_self = true);

		/*
		 * @throws InvalidStateException if this component is not attahced to a SceneObject.
		 */
		template <typename T>
		[[nodiscard]] util::random_access_move_only_range_of<const T &> auto getComponentsInChildren(bool include_self = true) const;

		/*
		 * @throws InvalidStateException if the component is not attached to a SceneObject.
		 */
		[[nodiscard]] std::unique_ptr<Component> removeFromSceneObject();

	}; // class Component
} // namespace infd::scene

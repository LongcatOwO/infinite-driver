#pragma once

// std
#include <concepts>
#include <memory>

// project - scene
#include <infd/scene/definitions.hpp>

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
		virtual void onAttach() {}
		virtual void onDetach() {}
		virtual void onAwake() {}
		virtual void onFrameUpdate() {}
		virtual void onPhysicsUpdate() {}
		// virtual void onEnable() {}
		// virtual void onDisable() {}

	public:
		 virtual ~Component() {}

		[[nodiscard]] Scene& scene() noexcept;
		[[nodiscard]] const Scene& scene() const noexcept;

		[[nodiscard]] SceneObject& sceneObject() noexcept;
		[[nodiscard]] const SceneObject& sceneObject() const noexcept;

		[[nodiscard]] Transform& transform() noexcept;
		[[nodiscard]] const Transform& transform() const noexcept;

		[[nodiscard]] Float frameDeltaTime() const noexcept;
		[[nodiscard]] Float physicsDeltaTime() const noexcept;

		template <std::derived_from<Component> T>
		T& addComponent(std::unique_ptr<T> component) noexcept;

		template <std::derived_from<Component> T, typename ...Args>
		T& emplaceComponent(Args &&...args);

		template <typename T>
		[[nodiscard]] T* getComponent() noexcept;

		template <typename T>
		[[nodiscard]] const T* getComponent() const noexcept;
	}; // class Component
} // namespace infd::scene

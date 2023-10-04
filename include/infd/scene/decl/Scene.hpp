#pragma once

// std
#include <cstddef>
#include <memory>
#include <string>
// #include <unordered_map>
#include <vector>

// project - math
#include <infd/math/DynamicRatio.hpp>

// project - util
#include <infd/util/exceptions.hpp>

// project - scene
#include <infd/scene/definitions.hpp>
#include <infd/util/Event.hpp>
#include <infd/scene/PhysicsContext.hpp>
#include <infd/scene/Timer.hpp>

// forward declarations
#include <infd/scene/fwd/SceneObject.hpp>

namespace infd::scene {
	class Scene final {
	public:
		friend class SceneObject;

		static constexpr math::DynamicRatio<Float> default_frame_rate{60};
		static constexpr math::DynamicRatio<Float> default_physics_rate{300};

	private:
		std::string _name;

		Timer _frame_timer{default_frame_rate.inverse()};
		Timer _physics_timer{default_physics_rate.inverse()};

		util::Event<void (Scene &)> _on_frame_update;
		util::Event<void (Scene &)> _on_physics_update;

		PhysicsContext _physics_context;
		std::vector<std::unique_ptr<SceneObject>> _scene_objects;
		
		// TODO faster object identity lookup
		// std::unordered_map<SceneObject *, std::size_t> _scene_objects_index;

		void frameUpdate();
		void physicsUpdate();
	public:
		Scene(std::string name) noexcept;

		[[nodiscard]] Float frameRate() const noexcept;
		void frameRate(Float value) noexcept;

		[[nodiscard]] Float physicsRate() const noexcept;
		void physicsRate(Float value) noexcept;

		void resetTime() noexcept;
		void enableTime() noexcept;
		void disableTime() noexcept;
		[[nodiscard]] bool isTimeEnabled() const noexcept;
		void isTimeEnabled(bool value) noexcept;
		void updateTime();

		[[nodiscard]] Float frameDeltaTime() const noexcept;
		[[nodiscard]] Float physicsDeltaTime() const noexcept;

		[[nodiscard]] PhysicsContext& physicsContext() noexcept;
		[[nodiscard]] const PhysicsContext& physicsContext() const noexcept;

		[[nodiscard]] util::PublicEvent<void (Scene &)>& onFrameUpdate() noexcept;
		[[nodiscard]] util::PublicEvent<void (Scene &)>& onPhysicsUpdate() noexcept;

		/*
		 * @throws infd::util::NullPointerException if so is null.
		 */
		SceneObject& addSceneObject(std::unique_ptr<SceneObject> so);

	private:
		std::unique_ptr<SceneObject> internalUncheckedRemoveSceneObject(SceneObject &so) noexcept;
	}; // class Scene
} // namespace infd::scene

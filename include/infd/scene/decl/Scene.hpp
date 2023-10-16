#pragma once

// std
#include <chrono>
#include <concepts>
#include <cstddef>
#include <memory>
#include <string>
// #include <unordered_map>
#include <vector>

// project - util
#include <infd/util/concepts.hpp>
#include <infd/util/exceptions.hpp>

// project - scene
#include <infd/scene/definitions.hpp>
#include <infd/util/Event.hpp>
#include <infd/util/Timer.hpp>

// forward declarations
#include <infd/scene/fwd/Component.hpp>
#include <infd/scene/fwd/SceneObject.hpp>


namespace infd::scene {
	class Scene final {
	public:
		friend class SceneObject;

		static constexpr double default_frame_per_sec = 60;
		static constexpr double default_physics_per_sec = 300;

	private:
		std::string _name;

		util::Timer _frame_timer = util::Timer::fromRate(default_frame_per_sec);
		util::Timer _physics_timer = util::Timer::fromRate(default_physics_per_sec);

		util::Event<void (Scene &)> _on_frame_render;

		util::Event<void (double x_pos, double y_pos)> _cursor_pos_event;
		util::Event<void (int button, int action, int mods)> _mouse_button_event;
		util::Event<void (double x_offset, double y_offset)> _scroll_event;
		util::Event<void (int key, int scancode, int action, int mods)> _key_event;
		util::Event<void (unsigned int c)> _char_event;

		std::vector<std::unique_ptr<SceneObject>> _scene_objects;

		bool _is_awaken = false;
		
		// TODO faster object identity lookup
		// std::unordered_map<SceneObject *, std::size_t> _scene_objects_index;

		void internalDoFrame(util::Timer &);
		void internalDoPhysics(util::Timer &);

	public:
		Scene(std::string name) noexcept;

		Scene(const Scene &) = delete;
		Scene& operator=(const Scene &) = delete;

		void awake();
		bool isAwaken() const noexcept;

		template <typename Period = std::ratio<1>, std::floating_point Rep = double>
		[[nodiscard]] Rep frameRate() const noexcept;

		template <typename Period = std::ratio<1>, std::floating_point Rep>
		void frameRate(const Rep &value) noexcept;

		[[nodiscard]] const util::Timer::Duration& frameInterval() const noexcept;

		template <typename Rep, typename Period>
		void frameInterval(const std::chrono::duration<Rep, Period> &value) noexcept;


		template <typename Period = std::ratio<1>, std::floating_point Rep = double>
		[[nodiscard]] Rep physicsRate() const noexcept;

		template <typename Period = std::ratio<1>, std::floating_point Rep>
		void physicsRate(const Rep &value) noexcept;

		[[nodiscard]] const util::Timer::Duration& physicsInterval() const noexcept;

		template <typename Rep, typename Period>
		void physicsInterval(const std::chrono::duration<Rep, Period> &value) noexcept;


		void resetTime() noexcept;
		void enableTime() noexcept;
		void disableTime() noexcept;
		[[nodiscard]] bool isTimeEnabled() const noexcept;
		void isTimeEnabled(bool value) noexcept;
		void updateTime();

		[[nodiscard]] util::Timer::Duration frameDeltaTime() const noexcept;
		[[nodiscard]] util::Timer::Duration physicsDeltaTime() const noexcept;

		[[nodiscard]] util::PublicEvent<void (Scene &)>& onFrameRender() noexcept;

		[[nodiscard]] util::PublicEvent<void (double x_pos, double y_pos)>& cursorPosEvent() noexcept;
		[[nodiscard]] util::PublicEvent<void (int button, int action, int mods)>& mouseButtonEvent() noexcept;
		[[nodiscard]] util::PublicEvent<void (double x_offset, double y_offset)>& scrollEvent() noexcept;
		[[nodiscard]] util::PublicEvent<void (int key, int scancode, int action, int mods)>& keyEvent() noexcept;
		[[nodiscard]] util::PublicEvent<void (unsigned int c)>& charEvent() noexcept;

		void cursorPosCallback(double x_pos, double y_pos);
		void mouseButtonCallback(int button, int action, int mods);
		void scrollCallback(double x_offset, double y_offset);
		void keyCallback(int key, int scancode, int action, int mods);
		void charCallback(unsigned int c);

		/*
		 * @throws infd::util::NullPointerException if so is null.
		 */
		SceneObject& addSceneObject(std::unique_ptr<SceneObject> so);

		[[nodiscard]] util::random_access_view_of<SceneObject &> auto rootSceneObjectsView() noexcept;

		[[nodiscard]] std::vector<SceneObject *> rootSceneObjectPointers() noexcept;
		[[nodiscard]] std::vector<SceneObject *> sceneObjectPointers() noexcept;

		[[nodiscard]] util::random_access_move_only_range_of<SceneObject &> auto rootSceneObjects() noexcept;
		[[nodiscard]] util::random_access_move_only_range_of<SceneObject &> auto sceneObjects() noexcept;

		template <util::consumer_of<SceneObject &> V>
		void visitRootSceneObjects(V &&v);

		template <util::consumer_of<SceneObject &> V>
		void visitSceneObjects(V &&v);

		template <typename T>
		[[nodiscard]] T* findComponentInRoots() noexcept;

		template <typename T>
		[[nodiscard]] T* findComponent() noexcept;

		template <typename T>
		[[nodiscard]] std::vector<T *> findComponentPointersInRoots() noexcept;

		template <typename T>
		[[nodiscard]] std::vector<T *> findComponentPointers() noexcept;

		template <typename T>
		[[nodiscard]] util::random_access_move_only_range_of<T &> auto findComponentsInRoots() noexcept;

		template <typename T>
		[[nodiscard]] util::random_access_move_only_range_of<T &> auto findComponents() noexcept;

	private:
		std::unique_ptr<SceneObject> internalUncheckedRemoveSceneObject(SceneObject &so) noexcept;
	}; // class Scene
} // namespace infd::scene

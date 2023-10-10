#pragma once

// std
#include <cassert>
#include <chrono>
#include <concepts>
#include <format>
#include <memory>
#include <ranges>
#include <utility>
#include <vector>

// project - util
#include <infd/util/concepts.hpp>
#include <infd/util/exceptions.hpp>

// project - scene
#include <infd/scene/definitions.hpp>
#include <infd/util/Event.hpp>
#include <infd/util/Function.hpp>
#include <infd/util/Timer.hpp>

// declarations
#include <infd/scene/decl/Scene.hpp>
#include <infd/scene/decl/SceneObject.hpp>

#include <iostream>

namespace infd::scene {

	inline void Scene::internalDoFrame(util::Timer &) {
		visitRootSceneObjects(util::MemberFunc(&SceneObject::internalFrameUpdate));
		_on_frame_render(*this);
	}

	inline void Scene::internalDoPhysics(util::Timer &) {
		visitRootSceneObjects(util::MemberFunc(&SceneObject::internalPhysicsUpdate));
	}

	inline Scene::Scene(std::string name) noexcept :
		_name(std::move(name))
	{
		_frame_timer.onIntervalCompleted() 	 += util::BindedMemberFunc(&Scene::internalDoFrame,   *this);
		_physics_timer.onIntervalCompleted() += util::BindedMemberFunc(&Scene::internalDoPhysics, *this);
	}

	inline void Scene::awake() {
		visitRootSceneObjects(util::MemberFunc(&SceneObject::internalAwake));
	}

	template <typename Period, std::floating_point Rep>
	Rep Scene::frameRate() const noexcept {
		return _frame_timer.rate<Period, Rep>();
	}

	template <typename Period, std::floating_point Rep>
	void Scene::frameRate(const Rep &value) noexcept {
		_frame_timer.rate<Period, Rep>(value);
	}

	inline const util::Timer::Duration& Scene::frameInterval() const noexcept {
		return _frame_timer.interval();
	}

	template <typename Rep, typename Period>
	void Scene::frameInterval(const std::chrono::duration<Rep, Period> &value) noexcept {
		_frame_timer.interval(value);
	}

	template <typename Period, std::floating_point Rep>
	Rep Scene::physicsRate() const noexcept {
		return _physics_timer.rate<Period, Rep>();
	}

	template <typename Period, std::floating_point Rep>
	void Scene::physicsRate(const Rep &value) noexcept {
		_physics_timer.rate<Period, Rep>(value);
	}

	inline const util::Timer::Duration& Scene::physicsInterval() const noexcept {
		return _physics_timer.interval();
	}

	template <typename Rep, typename Period>
	void Scene::physicsInterval(const std::chrono::duration<Rep, Period> &value) noexcept {
		_physics_timer.interval(value);
	}

	inline void Scene::resetTime() noexcept {
		_frame_timer.reset();
		_physics_timer.reset();
	}

	inline void Scene::enableTime() noexcept {
		_frame_timer.enable();
		_physics_timer.enable();
	}

	inline void Scene::disableTime() noexcept {
		_frame_timer.disable();
		_physics_timer.disable();
	}

	inline bool Scene::isTimeEnabled() const noexcept {
		assert(_frame_timer.isEnabled() == _physics_timer.isEnabled());
		return _frame_timer.isEnabled();
	}

	inline void Scene::isTimeEnabled(bool value) noexcept {
		_frame_timer.isEnabled(value);
		_physics_timer.isEnabled(value);
	}

	inline void Scene::updateTime() {
		_frame_timer.update();
		_physics_timer.update();
	}

	inline util::Timer::Duration Scene::frameDeltaTime() const noexcept {
		return _frame_timer.deltaTime();
	}

	inline util::Timer::Duration Scene::physicsDeltaTime() const noexcept {
		return _physics_timer.deltaTime();
	}

	inline util::PublicEvent<void (Scene&)>& Scene::onFrameRender() noexcept {
		return _on_frame_render;
	}

	inline util::PublicEvent<void (double x_pos, double y_pos)>& Scene::cursorPosEvent() noexcept {
		return _cursor_pos_event;
	}

	inline util::PublicEvent<void (int button, int action, int mods)>& Scene::mouseButtonEvent() noexcept {
		return _mouse_button_event;
	}

	inline util::PublicEvent<void (double x_offset, double y_offset)>& Scene::scrollEvent() noexcept {
		return _scroll_event;
	}

	inline util::PublicEvent<void (int key, int scancode, int action, int mods)>& Scene::keyEvent() noexcept {
		return _key_event;
	}

	inline util::PublicEvent<void (unsigned int c)>& Scene::charEvent() noexcept {
		return _char_event;
	}

	inline void Scene::cursorPosCallback(double x_pos, double y_pos) {
		_cursor_pos_event(x_pos, y_pos);
	}

	inline void Scene::mouseButtonCallback(int button, int action, int mods) {
		_mouse_button_event(button, action, mods);
	}

	inline void Scene::scrollCallback(double x_offset, double y_offset) {
		_scroll_event(x_offset, y_offset);
	}

	inline void Scene::keyCallback(int key, int scancode, int action, int mods) {
		_key_event(key, scancode, action, mods);
	}

	inline void Scene::charCallback(unsigned int c) {
		_char_event(c);
	}

	inline util::random_access_view_of<SceneObject &> auto Scene::rootSceneObjectsView() noexcept {
		return _scene_objects 
			| std::views::transform([](std::unique_ptr<SceneObject> &ptr) -> SceneObject& { return *ptr; });
	}

	inline std::vector<SceneObject *> Scene::rootSceneObjectPointers() noexcept {
		auto view = _scene_objects
			| std::views::transform([](std::unique_ptr<SceneObject> &ptr) -> SceneObject* { return ptr.get(); });
		return { view.begin(), view.end() };
	}

	inline util::random_access_move_only_range_of<SceneObject &> auto Scene::rootSceneObjects() noexcept {
		return std::ranges::owning_view(rootSceneObjectPointers())
			| std::views::transform([](SceneObject *ptr) -> SceneObject& { return *ptr; });
	}

	inline util::random_access_move_only_range_of<SceneObject &> auto Scene::sceneObjects() noexcept {
		return std::ranges::owning_view(sceneObjectPointers())
			| std::views::transform([](SceneObject *ptr) -> SceneObject& { return *ptr; });
	}

	template <util::consumer_of<SceneObject &> V>
	void Scene::visitRootSceneObjects(V &&v) {
		for (SceneObject &so : rootSceneObjectsView())
			std::forward<V>(v)(so);
	}

	template <util::consumer_of<SceneObject &> V>
	void Scene::visitSceneObjects(V &&v) {
		for (SceneObject &so : rootSceneObjectsView()) {
			std::forward<V>(v)(so);
			so.visitAllChildren(std::forward<V>(v));
		}
	}

	template <typename T>
	T* Scene::findComponentInRoots() noexcept {
		for (SceneObject &so : rootSceneObjectsView()) {
			if (T *comp = so.getComponent<T>(); comp)
				return comp;
		}

		return nullptr;
	}

	template <typename T>
	T* Scene::findComponent() noexcept {
		// search in root objects first
		if (T *comp = findComponentInRoots<T>(); comp)
			return comp;

		// search down the heirarchy
		for (SceneObject &so : rootSceneObjectsView()) {
			if (T *comp = so.getComponentInChildren<T>(false); comp)
				return comp;
		}

		return nullptr;
	}

	template <typename T>
	std::vector<T *> Scene::findComponentPointersInRoots() noexcept {
		auto view = rootSceneObjectsView() 
			| std::views::transform([](SceneObject &so) { return so.getComponentsView<T>(); })
			| std::views::join
			| std::views::transform([](T &comp) -> T* { return &comp; })
			| std::views::common;

		return { view.begin(), view.end() };
	}

	template <typename T>
	std::vector<T *> Scene::findComponentPointers() noexcept {
		std::vector<T *> result;
		visitSceneObjects([&result](SceneObject &so) {
			auto comp_view = so.getComponentsView<T>()
				| std::views::transform([](T &comp) -> T* { return &comp; })
				| std::views::common;
			result.insert(result.end(), comp_view.begin(), comp_view.end());
		});

		return result;
	}

	template <typename T>
	util::random_access_move_only_range_of<T &> auto Scene::findComponentsInRoots() noexcept {
		return std::ranges::owning_view(findComponentPointersInRoots<T>())
			| std::views::transform([](T *ptr) -> T& { return *ptr; });
	}

	template <typename T>
	util::random_access_move_only_range_of<T &> auto Scene::findComponents() noexcept {
		return std::ranges::owning_view(findComponentPointers<T>())
			| std::views::transform([](T *ptr) -> T& { return *ptr; });
	}

} // namespace infd::scene

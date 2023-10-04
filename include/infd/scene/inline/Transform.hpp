#pragma once

// std
#include <cstddef>
#include <ranges>
#include <string>
#include <vector>

// project - util
#include <infd/util/concepts.hpp>

// declarations
#include <infd/scene/decl/Component.hpp>
#include <infd/scene/decl/Transform.hpp>

// inline definitions
#include <infd/scene/inline/SceneObject.hpp>

namespace infd::scene {
	inline Transform::Transform() noexcept:
		_local_position(0),
		_local_rotation(1, 0, 0, 0),
		_local_scale(1)
	{}

	inline std::string& Transform::name() noexcept {
		return sceneObject().name();
	}

	inline const std::string& Transform::name() const noexcept {
		return sceneObject().name();
	}

	inline bool Transform::isRoot() const noexcept {
		return sceneObject().isRoot();
	}

	inline bool Transform::hasParent() const noexcept {
		return sceneObject().hasParent();
	}

	inline Transform* Transform::parent() noexcept {
		return &sceneObject().transform();
	}

	inline const Transform* Transform::parent() const noexcept {
		return &sceneObject().transform();
	}

	inline bool Transform::hasChildren() const noexcept {
		return sceneObject().hasChildren();
	}

	inline std::size_t Transform::numChildren() const noexcept {
		return sceneObject().numChildren();
	}

	inline util::random_access_view_of<Transform *> auto Transform::childPointersView() noexcept {
		return 
			sceneObject().childPointersView()
		  | std::views::transform([](SceneObject *so) -> Transform* { return &so->transform(); });
	}

	inline util::random_access_view_of<const Transform *> auto Transform::childPointersView() const noexcept {
		return
			sceneObject().childPointersView()
		  | std::views::transform([](const SceneObject *so) -> const Transform* { return &so->transform(); });
	}

	inline std::vector<Transform *> Transform::childPointers() noexcept {
		auto view = childPointersView();
		return { view.begin(), view.end() };
	}

	inline std::vector<const Transform *> Transform::childPointers() const noexcept {
		auto view = childPointersView();
		return { view.begin(), view.end() };
	}

	inline util::random_access_move_only_range_of<Transform &> auto Transform::children() noexcept {
		return 
			sceneObject().children()
		  | std::views::transform([](SceneObject &so) -> Transform& { return so.transform(); });
	}

	inline util::random_access_move_only_range_of<const Transform &> auto Transform::children() const noexcept {
		return
			sceneObject().children()
		  | std::views::transform([](const SceneObject &so) -> const Transform& { return so.transform(); });
	}

	inline util::random_access_move_only_range_of<Transform &> auto Transform::allChildren() noexcept {
		return
			sceneObject().allChildren()
		  | std::views::transform([](SceneObject &so) -> Transform& { return so.transform(); });
	}

	inline util::random_access_move_only_range_of<const Transform &> auto Transform::allChildren() const noexcept {
		return
			sceneObject().allChildren()
		  | std::views::transform([](const SceneObject &so) -> const Transform& { return so.transform(); });
	}

	inline Transform& Transform::addChild(std::string name) noexcept {
		return sceneObject().addChild(std::move(name)).transform();
	}

	inline void Transform::removeFromParent(Transform &new_parent) {
		sceneObject().removeFromParent(new_parent.sceneObject());
	}

	inline Transform& Transform::attachChild(std::string name) noexcept {
		return sceneObject().attachChild(std::move(name)).transform();
	}

	inline void Transform::detachFromParent(Transform &new_parent) {
		sceneObject().detachFromParent(new_parent.sceneObject());
	}

} // namespace infd::scene

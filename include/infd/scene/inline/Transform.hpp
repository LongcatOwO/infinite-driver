// Created by Phuwasate Lutchanont

#pragma once

// std
#include <cstddef>
#include <ranges>
#include <string>
#include <vector>

// glm
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

// project - util
#include <infd/util/concepts.hpp>
#include <infd/util/Function.hpp>
#include <infd/util/OwnableProperty.hpp>

// project - scene
#include <infd/scene/definitions.hpp>

// declarations
#include <infd/scene/decl/Component.hpp>
#include <infd/scene/decl/Transform.hpp>

// inline definitions
#include <infd/scene/inline/SceneObject.hpp>

#include <iostream>

namespace infd::scene {

	inline void Transform::onAttach() noexcept {
		_parent_global_transform_updated = false;
		visitAllChildren([](Transform &child) { child._parent_global_transform_updated = false; });
		sceneObject().onParentAssigned() += util::BindedMemberFunc(&Transform::internalOnParentAssigned, *this);
	}

	inline void Transform::onDetach() noexcept {
		_parent_global_transform_updated = false;
		visitAllChildren([](Transform &child) { child._parent_global_transform_updated = false; });
		sceneObject().onParentAssigned() -= util::BindedMemberFunc(&Transform::internalOnParentAssigned, *this);
	}

	inline void Transform::internalOnParentAssigned(SceneObject &) noexcept {
		_parent_global_transform_updated = false;
		visitAllChildren([](Transform &child) { child._parent_global_transform_updated = false; });
	}

	inline glm::mat<4, 4, Float> Transform::internalParentGlobalTransform() const noexcept {
		if (_ignore_parent)
			return glm::mat<4, 4, Float>{1};

		if (_parent_global_transform_updated) 
			return _parent_global_transform_cache;

		if (const Transform *parent_ptr = parent(); parent_ptr)
			_parent_global_transform_cache = parent_ptr->globalTransform();
		else
			_parent_global_transform_cache = glm::mat<4, 4, Float>{1};

		_parent_global_transform_updated = true;

		return _parent_global_transform_cache;
	}

	inline Transform::Transform() noexcept:
		_local_position(0),
		_local_rotation(1, 0, 0, 0),
		_local_scale(1),
		_parent_global_transform_cache(1)
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
		if (SceneObject* parent_so = sceneObject().parent(); parent_so) {
			return &parent_so->transform();
		}
		return nullptr;
	}

	inline const Transform* Transform::parent() const noexcept {
		if (const SceneObject* parent_so = sceneObject().parent(); parent_so) {
			return &parent_so->transform();
		}
		return nullptr;
	}

	inline util::OwnableProperty<bool>& Transform::ignoreParent() noexcept {
		return _ignore_parent;
	}

	inline const util::OwnableProperty<bool>& Transform::ignoreParent() const noexcept {
		return _ignore_parent;
	}

	inline bool Transform::hasChildren() const noexcept {
		return sceneObject().hasChildren();
	}

	inline std::size_t Transform::numChildren() const noexcept {
		return sceneObject().numChildren();
	}

	inline util::random_access_view_of<Transform &> auto Transform::childrenView() noexcept {
		return 
			sceneObject().childrenView()
		  | std::views::transform([](SceneObject &so) -> Transform& { return so.transform(); });
	}

	inline util::random_access_view_of<const Transform &> auto Transform::childrenView() const noexcept {
		return
			sceneObject().childrenView()
		  | std::views::transform([](const SceneObject &so) -> const Transform& { return so.transform(); });
	}

	inline std::vector<Transform *> Transform::childPointers() noexcept {
		auto view = 
			sceneObject().childrenView()
		  | std::views::transform([](SceneObject &so) -> Transform* { return &so.transform(); });
		return { view.begin(), view.end() };
	}

	inline std::vector<const Transform *> Transform::childPointers() const noexcept {
		auto view = 
			sceneObject().childrenView()
		  | std::views::transform([](const SceneObject &so) -> const Transform* { return &so.transform(); });
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

	template <util::consumer_of<Transform &> V>
	void Transform::visitChildren(V &&visitor) {
		sceneObject().visitChildren([&visitor](SceneObject &so) { 
			std::forward<V>(visitor)(so); 
		});
	}

	template <util::consumer_of<const Transform &> V>
	void Transform::visitChildren(V &&visitor) const {
		sceneObject().visitChildren([&visitor](const SceneObject &so) {
			std::forward<V>(visitor)(so);
		});
	}

	template <util::consumer_of<Transform &> V>
	void Transform::visitAllChildren(V &&visitor) {
		sceneObject().visitAllChildren([&visitor](SceneObject &so) {
			std::forward<V>(visitor)(so.transform());
		});
	}

	template <util::consumer_of<const Transform &> V>
	void Transform::visitAllChildren(V &&visitor) const {
		sceneObject().visitAllChildren([&visitor](const SceneObject &so) {
			std::forward<V>(visitor)(so.transform());
		});
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

	inline glm::vec<3, Float> Transform::localPosition() const noexcept {
		return _local_position;
	}

	inline glm::qua<Float> Transform::localRotation() const noexcept {
		return _local_rotation;
	}

	inline glm::vec<3, Float> Transform::localScale() const noexcept {
		return _local_scale;
	}

	inline glm::mat<4, 4, Float> Transform::localTransform() const noexcept {
		return glm::scale(glm::translate(glm::mat<4, 4, Float>{1}, 
				_local_position), 
				_local_scale) 
		  * glm::mat4_cast(_local_rotation);
	}

	inline glm::vec<3, Float> Transform::localFront() const noexcept {
		return localRotation() * glm::vec<3, Float>{0, 0, -1};
	}

	inline glm::vec<3, Float> Transform::localUp() const noexcept {
		return localRotation() * glm::vec<3, Float>{0, 1, 0};
	}

	inline glm::vec<3, Float> Transform::localRight() const noexcept {
		return localRotation() * glm::vec<3, Float>{1, 0, 0};
	}

	inline void Transform::localPosition(const glm::vec<3, Float> &value) noexcept {
		_local_position = value;
		visitAllChildren([](Transform &child) { child._parent_global_transform_updated = false; });
	}

	inline void Transform::localRotation(const glm::qua<Float> &value) noexcept {
		_local_rotation = value;
		visitAllChildren([](Transform &child) { child._parent_global_transform_updated = false; });
	}

	inline void Transform::localScale(const glm::vec<3, Float> &value) noexcept {
		_local_scale = value;
		visitAllChildren([](Transform &child) { child._parent_global_transform_updated = false; });
	}

	inline void Transform::localTransform(
		const glm::vec<3, Float> &position,
		const glm::qua<Float> &rotation,
		const glm::vec<3, Float> &scale
	) noexcept {
		_local_position = position;
		_local_rotation = rotation;
		_local_scale = scale;
		visitAllChildren([](Transform &child) { child._parent_global_transform_updated = false; });
	}

	inline glm::vec<3, Float> Transform::globalPosition() const noexcept {
		glm::vec<3, Float> position;
		glm::qua<Float> rotation;
		glm::vec<3, Float> scale;
		decomposeGlobalTransform(position, rotation, scale);
		return position;
	}

	inline glm::qua<Float> Transform::globalRotation() const noexcept {
		glm::vec<3, Float> position;
		glm::qua<Float> rotation;
		glm::vec<3, Float> scale;
		decomposeGlobalTransform(position, rotation, scale);
		return rotation;
	}

	inline glm::vec<3, Float> Transform::globalScale() const noexcept {
		glm::vec<3, Float> position;
		glm::qua<Float> rotation;
		glm::vec<3, Float> scale;
		decomposeGlobalTransform(position, rotation, scale);
		return scale;
	}

	inline glm::mat<4, 4, Float> Transform::globalTransform() const noexcept {
		return internalParentGlobalTransform() * localTransform();
	}

	inline void Transform::globalTransform(
		const glm::vec<3, Float> &position,
		const glm::qua<Float> &rotation,
		const glm::vec<3, Float> &scale
	) noexcept {
		// globalTransform = parentTransform * localTransform
		// localTransform = parentTransform^-1 * globalTransform
		glm::mat<4, 4, Float> target_global_transform = 
			glm::scale(glm::translate(glm::mat<4, 4, Float>{1},
				position),
				scale)
		  * glm::mat4_cast(rotation);
		
		glm::mat<4, 4, Float> target_local_transform = 
			glm::inverse(internalParentGlobalTransform()) * target_global_transform;

		glm::vec<3, Float> skew;
		glm::vec<4, Float> perspective;
		glm::decompose(target_local_transform, _local_scale, _local_rotation, _local_position, skew, perspective);
		visitAllChildren([](Transform& child) { child._parent_global_transform_updated = false; });
	}

	inline void Transform::decomposeGlobalTransform(
		glm::vec<3, Float> &out_position,
		glm::qua<Float> &out_rotation,
		glm::vec<3, Float> &out_scale
	) const noexcept {
		glm::vec<3, Float> skew;
		glm::vec<4, Float> perspective;
		glm::decompose(globalTransform(), out_scale, out_rotation, out_position, skew, perspective);
	}

} // namespace infd::scene

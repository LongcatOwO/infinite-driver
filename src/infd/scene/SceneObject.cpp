// Created by Phuwasate Lutchanont

// std
#include <format>
#include <memory>
#include <ranges>
#include <string>
#include <utility>
#include <vector>

// glm
#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

// project - util
#include <infd/util/exceptions.hpp>

// project - scene
#include <infd/scene/Scene.hpp>

namespace infd::scene {

	SceneObject::~SceneObject() {
		// remove all children, calling their callbacks
		while (!_children.empty()) {
			_on_child_removed(*this, *_children.back());
			_children.back()->internalUncheckedNotifyParentUnassigned();
			_children.back()->internalUncheckedUnnotifiedSetParent(nullptr);
			_children.pop_back();
		}

		while (!_components.empty()) {
			_on_component_removed(*this, *_components.back());
			_components.back()->onDetach();
			_components.back()->_scene_object = nullptr;
			_components.pop_back();
		}
	}

	std::vector<SceneObject*> SceneObject::allChildPointers() noexcept {
		namespace rng = std::ranges;
		namespace vw = std::views;
		std::vector<SceneObject *> result = childPointers();
		auto begin = result.begin();
		auto end = result.end();
		while (begin != end) {
			auto next_gen = rng::subrange(begin, end)
				| vw::transform([](SceneObject *so) { return so->childrenView(); })
				| vw::join
				| vw::transform([](SceneObject &so) -> SceneObject* { return &so; })
				| vw::common;
			result.insert(end, next_gen.begin(), next_gen.end());
			begin = end;
			end = result.end();
		}
		return result;
	}

	std::vector<const SceneObject*> SceneObject::allChildPointers() const noexcept {
		namespace rng = std::ranges;
		namespace vw = std::views;
		std::vector<const SceneObject *> result = childPointers();
		auto begin = result.begin();
		auto end = result.end();
		while (begin != end) {
			auto next_gen = rng::subrange(begin, end)
				| vw::transform([](const SceneObject *so) { return so->childrenView(); })
				| vw::join
				| vw::transform([](const SceneObject &so) -> const SceneObject* { return &so; })
				| vw::common;
			result.insert(end, next_gen.begin(), next_gen.end());
			begin = end;
			end = result.end();
		}
		return result;
	}

	SceneObject& SceneObject::addChild(std::string name) noexcept {
		SceneObject *new_child = new SceneObject(std::move(name));
		internalUncheckedAddChild(std::unique_ptr<SceneObject>{new_child});
		return *new_child;
	}

	SceneObject& SceneObject::addChild(std::unique_ptr<SceneObject> child) {
		if (!child) 
			throw util::NullPointerException(
					std::format("[SceneObject(\"{}\")::addChild(std::unique_ptr<SceneObject> child)]: child is null.", _name)
				);
		
		SceneObject *parent = this;
		SceneObject *child_ptr = child.get();
		while (parent && parent != child_ptr)
			parent = parent->_parent;

		if (parent == child_ptr)
			throw util::CyclicReferenceException(
					std::format("[SceneObject(\"{}\")::addChild(std::unique_ptr<SceneObject> child)]: child(\"{}\") is parent in the heirarchy.", _name, child_ptr->_name)
				);

		internalUncheckedAddChild(std::move(child));
		return *child_ptr;
	}

	std::unique_ptr<SceneObject> SceneObject::removeFromParent() {
		if (!_parent) 
			throw util::InvalidStateException(
					std::format("[SceneObject(\"{}\")::removeFromParent()]: parent is null.", _name)
				);

		return _parent->internalUncheckedRemoveChild(*this);
	}

	void SceneObject::removeFromParent(SceneObject &new_parent) {
		if (!_parent)
			throw util::InvalidStateException(
					std::format("[SceneObject(\"{}\")::removeFromParent(SceneObject &new_parent)]: parent is null.", _name)
				);

		SceneObject *new_parent_parent = &new_parent;
		while (new_parent_parent && new_parent_parent != this)
			new_parent_parent = new_parent_parent->_parent;

		if (new_parent_parent == this) {
			throw util::CyclicReferenceException(
					std::format("[SceneObject(\"{}\")::removeFromParent(SceneObject &new_parent)]: new_parent(\"{}\") is child in the heirarchy.", _name, new_parent._name)
				);
		}

		_parent->internalUncheckedRemoveChild(*this, new_parent);
	}

	std::unique_ptr<SceneObject> SceneObject::removeFromScene() {
		if (!_scene)
			throw util::InvalidStateException(
					std::format("[SceneObject(\"{}\")::removeFromScene()]: object is not attached to scene.", _name)
				);
		if (!isRoot())
			throw util::InvalidStateException(
					std::format("[SceneObject(\"{}\")::removeFromScene()]: object is not a root in the scene.", _name)
				);

		return _scene->internalUncheckedRemoveSceneObject(*this);
	}

	SceneObject& SceneObject::attachChild(std::string name) noexcept {
		SceneObject &child = addChild(std::move(name));
		child.transform().globalTransform(glm::vec<3, Float>{0});
		return child;
	}

	SceneObject& SceneObject::attachChild(std::unique_ptr<SceneObject> child) {
		if (!child)
			throw util::NullPointerException(
					std::format("[SceneObject(\"{}\")::attachChild(std::unique_ptr<SceneObject> child)]: child is null.", _name)
				);

		glm::vec<3, Float> position;
		glm::qua<Float> rotation;
		glm::vec<3, Float> scale;
		child->transform().decomposeGlobalTransform(position, rotation, scale);
		SceneObject &child_ref = addChild(std::move(child));
		child_ref.transform().globalTransform(position, rotation, scale);
		return child_ref;
	}

	std::unique_ptr<SceneObject> SceneObject::detachFromParent() {
		glm::vec<3, Float> position;
		glm::qua<Float> rotation;
		glm::vec<3, Float> scale;
		transform().decomposeGlobalTransform(position, rotation, scale);
		std::unique_ptr<SceneObject> owning_self = removeFromParent();
		transform().globalTransform(position, rotation, scale);
		return owning_self;
	}

	void SceneObject::detachFromParent(SceneObject &new_parent) {
		glm::vec<3, Float> position;
		glm::qua<Float> rotation;
		glm::vec<3, Float> scale;
		transform().decomposeGlobalTransform(position, rotation, scale);
		removeFromParent(new_parent);
		transform().globalTransform(position, rotation, scale);
	}

	void SceneObject::internalUncheckedNotifySceneUnassigned() noexcept {
		if (this->hasScene()) {
			_on_scene_unassigned(*this);
			visitAllChildren([](SceneObject &child) { child._on_scene_unassigned(child); });
		}
	}

	void SceneObject::internalUncheckedNotifySceneAssigned() noexcept {
		if (this->hasScene()) {
			_on_scene_assigned(*this);
			visitAllChildren([](SceneObject &child) { child._on_scene_assigned(child); });
		}
	}

	void SceneObject::internalUncheckedUnnotifiedSetScene(Scene *scene) noexcept {
		_scene = scene;
		visitAllChildren([scene](SceneObject &child) { child._scene = scene; });
	}

	void SceneObject::internalSetScene(Scene *scene) noexcept {
		internalUncheckedNotifySceneUnassigned();
		internalUncheckedUnnotifiedSetScene(scene);
		internalUncheckedNotifySceneAssigned();
	}

	void SceneObject::internalUncheckedNotifyParentUnassigned() noexcept {
		if (this->hasParent()) {
			_on_parent_unassigned(*this);
			internalUncheckedNotifySceneUnassigned();
		}
	}

	void SceneObject::internalUncheckedNotifyParentAssigned() noexcept {
		if (this->hasParent()) {
			_on_parent_assigned(*this);
			internalUncheckedNotifySceneAssigned();
		}
	}

	void SceneObject::internalUncheckedUnnotifiedSetParent(SceneObject *parent) noexcept {
		_parent = parent;
		if (_parent)
			internalUncheckedUnnotifiedSetScene(_parent->_scene);
		else
			internalUncheckedUnnotifiedSetScene(nullptr);
	}

	void SceneObject::internalUncheckedAddChild(std::unique_ptr<SceneObject> child) noexcept {
		SceneObject &child_ref = *child;

		_children.push_back(std::move(child));
		child_ref.internalUncheckedUnnotifiedSetParent(this);
		child_ref.internalUncheckedNotifyParentAssigned();
		_on_child_added(*this, child_ref);
		if (hasScene() && scene().isAwaken())
			child_ref.internalAwake();
	}

	std::unique_ptr<SceneObject> SceneObject::internalUncheckedRemoveChild(SceneObject &child) noexcept {
		_on_child_removed(*this, child);
		child.internalUncheckedNotifyParentUnassigned();

		auto it = std::find_if(
				_children.begin(), _children.end(), 
				[&child](std::unique_ptr<SceneObject> &ptr) {
					return &child == ptr.get();
				}
			);
		std::unique_ptr<SceneObject> owning_child{std::move(*it)};
		_children.erase(it);
		child.internalUncheckedUnnotifiedSetParent(nullptr);

		return owning_child;
	}

	void SceneObject::internalUncheckedRemoveChild(SceneObject &child, SceneObject &new_parent) noexcept {
		_on_child_removed(*this, child);
		child.internalUncheckedNotifyParentUnassigned();

		auto it = std::find_if(
				_children.begin(), _children.end(),
				[&child](std::unique_ptr<SceneObject> &ptr) {
					return &child == ptr.get();
				}
			);
		std::unique_ptr<SceneObject> owning_child{std::move(*it)};
		_children.erase(it);
		new_parent._children.push_back(std::move(owning_child));
		child.internalUncheckedUnnotifiedSetParent(&new_parent);

		child.internalUncheckedNotifyParentAssigned();
		new_parent._on_child_added(new_parent, child);
	}

	void SceneObject::internalUncheckedAddComponent(std::unique_ptr<Component> component) noexcept {
		Component *comp_ptr = component.get();
		_components.push_back(std::move(component));
		comp_ptr->_scene_object = this;
		comp_ptr->onAttach();
		_on_component_added(*this, *comp_ptr);
		if (hasScene() && scene().isAwaken())
			comp_ptr->onAwake();
	}

	std::unique_ptr<Component> SceneObject::internalUncheckedRemoveComponent(Component &component) noexcept {
		_on_component_removed(*this, component);
		component.onDetach();
		auto it = std::find_if(
				_components.begin(), _components.end(), 
				[&component](std::unique_ptr<Component> &comp_ptr) {
					return &component == comp_ptr.get();
				}
			);

		std::unique_ptr<Component> owning_comp{std::move(*it)};
		_components.erase(it);
		owning_comp->_scene_object = nullptr;
		return owning_comp;
	}

	void SceneObject::internalUncheckedRemoveComponent(Component &component, SceneObject &new_owner) noexcept {
		_on_component_removed(*this, component);
		component.onDetach();
		auto it = std::find_if(
				_components.begin(), _components.end(),
				[&component](std::unique_ptr<Component> &comp_ptr) {
					return &component == comp_ptr.get();
				}
			);

		std::unique_ptr<Component> owning_comp{std::move(*it)};
		_components.erase(it);
		new_owner.internalUncheckedAddComponent(std::move(owning_comp));
	}
}

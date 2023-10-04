// std
#include <format>
#include <ranges>
#include <vector>

// project - util
#include <infd/util/exceptions.hpp>

// project - scene
#include <infd/scene/SceneObject.hpp>

namespace infd::scene {
	std::vector<SceneObject*> SceneObject::allChildPointers() noexcept {
		namespace rng = std::ranges;
		namespace vw = std::views;
		std::vector<SceneObject *> result = childPointers();
		auto begin = result.begin();
		auto end = result.end();
		while (begin != end) {
			auto next_gen = rng::subrange(begin, end)
				| vw::transform([](SceneObject *so) { return so->childPointersView(); })
				| vw::join
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
				| vw::transform([](const SceneObject *so) { return so->childPointersView(); })
				| vw::join
				| vw::common;
			result.insert(end, next_gen.begin(), next_gen.end());
			begin = end;
			end = result.end();
		}
		return result;
	}

	SceneObject& SceneObject::addChild(std::string name) noexcept {
		SceneObject *new_child = new SceneObject(std::move(name));
		_children.push_back(std::unique_ptr<SceneObject>{new_child});
		new_child->internalUncheckedSetParent(this);
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

		_children.push_back(std::move(child));
		child_ptr->internalUncheckedSetParent(this);
		return *child_ptr;
	}

	std::unique_ptr<SceneObject> SceneObject::removeFromParent() {
		if (!_parent) 
			throw util::InvalidStateException(
					std::format("[SceneObject(\"{}\")::removeFromParent()]: parent is null.", _name)
				);

		auto it = std::find_if(_parent->_children.begin(), _parent->_children.end(),
				[this] (std::unique_ptr<SceneObject> &ptr) {
					return this == ptr.get();
				}
			);

		std::unique_ptr<SceneObject> owning_self{std::move(*it)};
		_parent->_children.erase(it);

		internalUncheckedSetParent(nullptr);
		return owning_self;
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

		auto it = std::find_if(_parent->_children.begin(), _parent->_children.end(),
				[this] (std::unique_ptr<SceneObject> &ptr) {
					return this == ptr.get();
				}
			);

		std::unique_ptr<SceneObject> owning_self{std::move(*it)};
		_parent->_children.erase(it);

		new_parent._children.push_back(std::move(owning_self));
		internalUncheckedSetParent(&new_parent);
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
}

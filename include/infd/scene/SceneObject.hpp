#pragma once

// std
#include <cassert>
#include <concepts>
#include <memory>
#include <ranges>
#include <utility>
#include <vector>

// bullet
#include <btBulletCollisionCommon.h>

// project
#include <infd/util/concepts.hpp>


namespace infd::scene {
	class SceneObject;
	class Component;
	class Transform;


	class SceneObject {
		SceneObject *_parent;
		std::vector<std::unique_ptr<SceneObject>> _children;

		std::vector<std::unique_ptr<Component>> _components;
		Transform *_transform;

		void attachComponentRoutine(Component &component);

	protected:
		virtual void onAddChild(SceneObject& child) { (void)child; }
		virtual void onRemoveChild(SceneObject& child) { (void)child; }
	public:
		SceneObject() noexcept;

		SceneObject* parent() noexcept;
		const SceneObject* parent() const noexcept;
		util::random_access_range_of<SceneObject> auto children() noexcept;
		util::random_access_range_of<const SceneObject> auto children() const noexcept;
		std::unique_ptr<SceneObject> removeChild(SceneObject &child);
		SceneObject& addChild(std::unique_ptr<SceneObject> child);

		template <std::derived_from<Component> T>
		T& addComponent(std::unique_ptr<T> component);

		template <std::derived_from<Component> T, typename ...Args>
		T& emplaceComponent(Args &&...args);

		template <typename T>
		T* getComponent();

		template <typename T>
		const T* getComponent() const;

		Transform& transform() noexcept;
		const Transform& transform() const noexcept;

		virtual ~SceneObject() {}
	};

	class SceneRoot : public SceneObject {

	};

	class Component {
		friend class SceneObject;
		SceneObject *_scene_object;

	public:
		Component() noexcept;
		SceneObject& sceneObject() noexcept;
		const SceneObject& sceneObject() const noexcept;

		template <std::derived_from<Component> T>
		T& addComponent(std::unique_ptr<T> component);

		template <std::derived_from<Component> T, typename ...Args>
		T& emplaceComponent(Args &&...args);

		template <typename T>
		T* getComponent();

		template <typename T>
		const T* getComponent() const;

		Transform& transform() noexcept;
		const Transform& transform() const noexcept;

		virtual void onAttach() {}
		virtual void onDetach() {}
		virtual void onUpdate() {}
		virtual void onFixedUpdate() {}
		virtual ~Component() {}
	};

	class Transform : public Component {
		friend class SceneObject;

		btVector3 _local_position;
		btQuaternion _local_rotation;
		btVector3 _local_scale;

		Transform() noexcept;
	public:
		
		Transform* parent() noexcept;
		const Transform* parent() const noexcept;

		util::random_access_range_of<Transform> auto children() noexcept;
		util::random_access_range_of<const Transform> auto children() const noexcept;

		const btVector3& localPosition() const noexcept;
		const btVector3& localPosition(const btVector3 &v) noexcept;
		const btQuaternion& localRotation() const noexcept;
		const btQuaternion& localRotation(const btQuaternion &q) noexcept;
		const btVector3& localScale() const noexcept;
		const btVector3& localScale(const btVector3 &v) noexcept;
	};

	// ==========inline implementations==========

	// ----------SceneObject----------

	inline void SceneObject::attachComponentRoutine(Component &component) {
		component._scene_object = this;
		component.onAttach();
	}

	inline std::unique_ptr<SceneObject> SceneObject::removeChild(SceneObject &child) {
		for (auto it = _children.begin(); it != _children.end(); ++it) {
			if (it->get() == &child) {
				std::unique_ptr<SceneObject> child_ptr{std::move(*it)};
				_children.erase(it);
				onRemoveChild(*child_ptr);
				return child_ptr;
			}
		}

		return nullptr;
	}

	inline SceneObject& SceneObject::addChild(std::unique_ptr<SceneObject> child) {
		SceneObject *ptr = child.get();
		_children.push_back(std::move(child));
		onAddChild(*ptr);
		return *ptr;
	}

	inline SceneObject::SceneObject() noexcept :
		_parent(nullptr)
	{
		_transform = &emplaceComponent<Transform>();
		assert(!!_transform);
	}

	inline SceneObject* SceneObject::parent() noexcept {
		return _parent;
	}

	inline const SceneObject* SceneObject::parent() const noexcept {
		return _parent;
	}

	inline util::random_access_range_of<SceneObject> auto SceneObject::children() noexcept {
		return _children | std::views::transform([](auto &ptr) -> SceneObject& { return *ptr; });
	}

	inline util::random_access_range_of<const SceneObject> auto SceneObject::children() const noexcept  {
		return _children | std::views::transform([](const auto &ptr) -> const SceneObject& { return *ptr; });
	}

	template <std::derived_from<Component> T>
	T& SceneObject::addComponent(std::unique_ptr<T> component) {
		T *ptr = component.get();
		_components.push_back(std::move(component));
		attachComponentRoutine(*ptr);
		return *ptr;
	}

	template <std::derived_from<Component> T, typename ...Args>
	T& SceneObject::emplaceComponent(Args &&...args) {
		T *ptr = new T(std::forward<Args>(args)...);
		_components.push_back(std::unique_ptr<T>(ptr));
		attachComponentRoutine(*ptr);
		return *ptr;
	}

	template <typename T>
	T* SceneObject::getComponent() {
		for (const auto &comp_ptr : _components) {
			if (auto ptr = dynamic_cast<T*>(comp_ptr.get()); ptr)
				return ptr;
		}
		return nullptr;
	}

	template <typename T>
	const T* SceneObject::getComponent() const {
		for (const auto &comp_ptr : _components) {
			if (auto ptr = dynamic_cast<const T*>(comp_ptr.get()); ptr)
				return ptr;
		}
		return nullptr;
	}

	inline Transform& SceneObject::transform() noexcept {
		return *_transform;
	}

	inline const Transform& SceneObject::transform() const noexcept {
		return *_transform;
	}

	// ----------Component----------

	inline Component::Component() noexcept : _scene_object(nullptr) {}

	inline SceneObject& Component::sceneObject() noexcept { 
		return *_scene_object; 
	}

	inline const SceneObject& Component::sceneObject() const noexcept { 
		return *_scene_object; 
	}

	template <std::derived_from<Component> T>
	T& Component::addComponent(std::unique_ptr<T> component) {
		return sceneObject().addComponent(std::move(component));
	}

	template <std::derived_from<Component> T, typename ...Args>
	T& Component::emplaceComponent(Args &&...args) {
		return sceneObject().emplaceComponent<T>(std::forward<Args>(args)...);
	}

	template <typename T>
	T* Component::getComponent() {
		return sceneObject().getComponent<T>();
	}

	template <typename T>
	const T* Component::getComponent() const {
		return sceneObject().getComponent<T>();
	}

	inline Transform& Component::transform() noexcept {
		return sceneObject().transform();
	}

	inline const Transform& Component::transform() const noexcept {
		return sceneObject().transform();
	}


	// ----------Transform----------

	inline Transform::Transform() noexcept : 
		_local_position{0, 0, 0},
		_local_rotation{0, 0, 0},
		_local_scale   {1, 1, 1}
	{}

	inline Transform* Transform::parent() noexcept {
		SceneObject *parent_obj = sceneObject().parent();
		if (parent_obj)
			return &parent_obj->transform();
		else
			return nullptr;
	}

	inline const Transform* Transform::parent() const noexcept {
		const SceneObject *parent_obj = sceneObject().parent();
		if (parent_obj)
			return &parent_obj->transform();
		else
			return nullptr;
	}

	inline util::random_access_range_of<Transform> auto Transform::children() noexcept {
		return 
			sceneObject().children() 
		  | std::views::transform([](auto &obj) -> Transform& { return obj.transform(); });
	}

	inline util::random_access_range_of<const Transform> auto Transform::children() const noexcept {
		return
			sceneObject().children()
		  | std::views::transform([](const auto &obj) -> const Transform& { return obj.transform(); });
	}

	inline const btVector3& Transform::localPosition() const noexcept { 
		return _local_position; 
	}

	inline const btVector3& Transform::localPosition(const btVector3 &v) noexcept { 
		return _local_position = v;
	}

	inline const btQuaternion& Transform::localRotation() const noexcept {
		return _local_rotation;
	}

	inline const btQuaternion& Transform::localRotation(const btQuaternion &q) noexcept {
		return _local_rotation = q;
	}

	inline const btVector3& Transform::localScale() const noexcept {
		return _local_scale;
	}

	inline const btVector3& Transform::localScale(const btVector3 &v) noexcept {
		return _local_scale = v;
	}
}

// std
#include <algorithm>
#include <memory>

// project - util
#include <infd/scene/Timer.hpp>

// project - scene
#include <infd/scene/Scene.hpp>
#include <infd/scene/SceneObject.hpp>

namespace infd::scene {

	void Scene::frameUpdate(Timer &) {
		for (std::unique_ptr<SceneObject> &so : _scene_objects)
			so->internalFrameUpdate();

		_on_frame_update(*this);
	}

	void Scene::physicsUpdate(Timer &) {
		for (std::unique_ptr<SceneObject> &so : _scene_objects)
			so->internalPhysicsUpdate();

		_on_physics_update(*this);
	}

	SceneObject& Scene::addSceneObject(std::unique_ptr<SceneObject> so) {
		if (!so)
			throw util::NullPointerException(
					std::format("[Scene(\"{}\")::addSceneObject(std::unique_ptr<SceneObject> so)]: so is null.", _name)
				);

		SceneObject *so_ptr = so.get();
		_scene_objects.push_back(std::move(so));
		so_ptr->internalSetScene(this);
		return *so_ptr;
	}

	std::unique_ptr<SceneObject> Scene::internalUncheckedRemoveSceneObject(SceneObject &so) noexcept {
		auto it = std::find_if(_scene_objects.begin(), _scene_objects.end(),
				[&so](std::unique_ptr<SceneObject> &ptr) { return &so == ptr.get(); }
			);

		std::unique_ptr<SceneObject> owning_so{std::move(*it)};
		_scene_objects.erase(it);

		so.internalSetScene(nullptr);
		return owning_so;
	}

} // namespace infd::scene

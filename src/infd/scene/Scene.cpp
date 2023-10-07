// std
#include <algorithm>
#include <memory>
#include <ranges>
#include <vector>

// project - util
#include <infd/scene/Timer.hpp>

// project - scene
#include <infd/scene/Scene.hpp>


namespace infd::scene {

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

	std::vector<SceneObject *> Scene::sceneObjectPointers() noexcept {
		namespace rng = std::ranges;
		namespace vw = std::views;

		std::vector<SceneObject *> result = rootSceneObjectPointers();
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

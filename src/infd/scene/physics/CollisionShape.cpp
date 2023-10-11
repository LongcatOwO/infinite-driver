// std
#include <format>

// project - scene
#include <infd/scene/exceptions.hpp>

// project - scene::physics
#include <infd/scene/physics/physics.hpp>

namespace infd::scene::physics {

	void CollisionShape::onAttach() {
		auto collision_bounds = getComponents<CollisionShape>();
		if (collision_bounds.size() > 1) {
			throw DuplicateComponentException(std::format(
					"[CollisionBound(\"{}\")::onAttach()]: Cannot have more than 1 CollisionBound on the same SceneObject!",
					sceneObject().name()
				));
		}
	}

} // namespace infd::scene::physics

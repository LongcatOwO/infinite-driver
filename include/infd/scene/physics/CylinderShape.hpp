#pragma once

// bullet
#include <BulletCollision/CollisionShapes/btCylinderShape.h>

// project - scene::physics
#include <infd/scene/physics/physics.hpp>


namespace infd::scene::physics {

	class CylinderShape : public CollisionShape {
		btCylinderShape _cylinder_shape;
	}; // class CylinderShape

} // namespace infd::scene::physics

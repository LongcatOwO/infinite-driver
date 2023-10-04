// std
#include <format>
#include <iostream>
#include <memory>
#include <vector>

// bullet
#include <btBulletDynamicsCommon.h>

// project
#include <infd/scene/SceneObject.hpp>

int main() {
	std::unique_ptr<btCollisionConfiguration> collision_configuration = 
		std::make_unique<btDefaultCollisionConfiguration>();

	std::unique_ptr<btDispatcher> dispatcher = 
		std::make_unique<btCollisionDispatcher>(collision_configuration.get());

	std::unique_ptr<btBroadphaseInterface> overlapping_pair_cache = 
		std::make_unique<btDbvtBroadphase>();

	std::unique_ptr<btConstraintSolver> solver = 
		std::make_unique<btSequentialImpulseConstraintSolver>();

	std::unique_ptr<btDynamicsWorld> dynamics_world = 
		std::make_unique<btDiscreteDynamicsWorld>(
			dispatcher.get(), 
			overlapping_pair_cache.get(),
			solver.get(),
			collision_configuration.get()
		);


	dynamics_world->setGravity({0, -10, 0});

	btAlignedObjectArray<btCollisionShape*> collision_shapes;
	
	{
		btCollisionShape *ground_shape = new btBoxShape({50, 50, 50});
		collision_shapes.push_back(ground_shape);

		btTransform ground_transform;
		ground_transform.setIdentity();
		ground_transform.setOrigin({0, -56, 0});

		btScalar mass(0);

		bool is_dynamic = (mass != 0);
		btVector3 local_inertia{0, 0, 0};
		if (is_dynamic)
			ground_shape->calculateLocalInertia(mass, local_inertia);

		btDefaultMotionState *motion_state = new btDefaultMotionState(ground_transform);
		btRigidBody::btRigidBodyConstructionInfo rb_info(mass, motion_state, ground_shape, local_inertia);
		btRigidBody *body = new btRigidBody(rb_info);

		dynamics_world->addRigidBody(body);
	}

	{
		btCollisionShape *collision_shape = new btSphereShape(btScalar(1));
		collision_shapes.push_back(collision_shape);

		btTransform start_transform;
		start_transform.setIdentity();

		btScalar mass(1.f);

		bool is_dynamic = (mass != 0);

		btVector3 local_inertia{0, 0, 0};
		if (is_dynamic) 
			collision_shape->calculateLocalInertia(mass, local_inertia);

		start_transform.setOrigin({2, 10, 0});

		auto motion_state = new btDefaultMotionState(start_transform);
		btRigidBody::btRigidBodyConstructionInfo rb_info(mass, motion_state, collision_shape, local_inertia);
		auto body = new btRigidBody(rb_info);
		dynamics_world->addRigidBody(body);
	}

	for (int i = 0; i < 100; ++i) {
		dynamics_world->stepSimulation(1.f/60.f, 10);

		for (int j = dynamics_world->getNumCollisionObjects() - 1; j >= 0; --j) {
			btCollisionObject *obj = dynamics_world->getCollisionObjectArray()[j];
			btRigidBody *body = btRigidBody::upcast(obj);
			btTransform trans;
			if (body && body->getMotionState()) {
				body->getMotionState()->getWorldTransform(trans);
			} else {
				trans = obj->getWorldTransform();
			}
			std::cout << 
				std::format(
					"world pos object {} = {},{},{}\n",
					j,
					static_cast<float>(trans.getOrigin().getX()),
					static_cast<float>(trans.getOrigin().getY()),
					static_cast<float>(trans.getOrigin().getZ())
				);
		}
	}


	for (int i = dynamics_world->getNumCollisionObjects() - 1; i >= 0; --i) {
		btCollisionObject *obj = dynamics_world->getCollisionObjectArray()[i];
		btRigidBody *body = btRigidBody::upcast(obj);
		if (body && body->getMotionState()) {
			delete body->getMotionState();
		}
		dynamics_world->removeCollisionObject(obj);
		delete obj;
	}

	collision_shapes.clear();


	return 0;
}

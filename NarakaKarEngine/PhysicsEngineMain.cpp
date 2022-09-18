#include "pch.h"
#include "PhysicsEngineMain.h"

#include "pch.h"
#include "PhysicsEngineMain.h"

PhysicsEngineMain::PhysicsEngineMain()
	: collisionConfig{ std::make_unique<btDefaultCollisionConfiguration>() }
	, dispatcher{ std::make_unique<btCollisionDispatcher>(collisionConfig.get()) }
	, broadPhase{ std::make_unique<btDbvtBroadphase>() }
	, solver{ std::make_unique<btSequentialImpulseConstraintSolver>() }
	, world{ std::make_unique<btDiscreteDynamicsWorld>(dispatcher.get(), broadPhase.get(), solver.get(), collisionConfig.get()) }
	, bodies{ std::vector<std::tuple<glm::mat4*, btRigidBody*>>() }
{
	world->setGravity(btVector3(0, -9.8f, 0));
}

void PhysicsEngineMain::Update(float deltaTime)
{
	world->stepSimulation(deltaTime);
	for (int i = 0; i < bodies.size(); ++i)
	{
		if (std::get<1>(bodies[i])->getCollisionShape()->getShapeType() == STATIC_PLANE_PROXYTYPE)
			ApplyPlanePhysics(bodies[i]);
		else if (std::get<1>(bodies[i])->getCollisionShape()->getShapeType() == SPHERE_SHAPE_PROXYTYPE)
			ApplySpherePhysics(bodies[i]);
	}
}

void PhysicsEngineMain::ApplyPlanePhysics(std::tuple<glm::mat4*, btRigidBody*> rigidBody)
{
	if (std::get<1>(rigidBody)->getCollisionShape()->getShapeType() != STATIC_PLANE_PROXYTYPE)
		return;

	btTransform t;
	std::get<1>(rigidBody)->getMotionState()->getWorldTransform(t);
	float mat[16];
	t.getOpenGLMatrix(mat);
	*(std::get<0>(rigidBody)) = glm::make_mat4(mat);
}

void PhysicsEngineMain::ApplySpherePhysics(std::tuple<glm::mat4*, btRigidBody*> rigidBody)
{
	if (std::get<1>(rigidBody)->getCollisionShape()->getShapeType() != SPHERE_SHAPE_PROXYTYPE)
		return;

	btTransform t;
	std::get<1>(rigidBody)->getMotionState()->getWorldTransform(t);
	float mat[16];
	t.getOpenGLMatrix(mat);
	*(std::get<0>(rigidBody)) = glm::make_mat4(mat);
}

void PhysicsEngineMain::AddStaticPlane(float rad, float x, float y, float z, float mass, glm::mat4* model)
{
	btTransform t;
	t.setIdentity();
	t.setOrigin(btVector3(0, 0, 0));
	btStaticPlaneShape* plane = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
	btMotionState* motion = new btDefaultMotionState(t);
	btRigidBody::btRigidBodyConstructionInfo info(0.0f, motion, plane);
	btRigidBody* body = new btRigidBody(info);
	world->addRigidBody(body);
	bodies.push_back(std::make_tuple(model, body));
}

void PhysicsEngineMain::AddSphere(float rad, float x, float y, float z, float mass, glm::mat4* model)
{
	btTransform t;
	t.setIdentity();
	t.setOrigin(btVector3(x, y, z));
	btSphereShape* sphere = new btSphereShape(rad);
	btVector3 inertia(0.0f, 0.0f, 0.0f);
	if (mass != 0.0) { sphere->calculateLocalInertia(mass, inertia); }
	btMotionState* motion = new btDefaultMotionState(t);
	btRigidBody::btRigidBodyConstructionInfo info(mass, motion, sphere, inertia);
	btRigidBody* body = new btRigidBody(info);
	world->addRigidBody(body);
	bodies.push_back(std::make_tuple(model, body));
}

PhysicsEngineMain::~PhysicsEngineMain()
{
	for (int i = 0; i < bodies.size(); ++i)
	{
		world->removeCollisionObject(std::get<1>(bodies[i]));
		btMotionState* motionState = std::get<1>(bodies[i])->getMotionState();
		btCollisionShape* shape = std::get<1>(bodies[i])->getCollisionShape();
		delete std::get<1>(bodies[i]);
		delete shape;
		delete motionState;
	}
}

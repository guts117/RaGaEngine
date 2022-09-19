#include "physics_pch.h"
#include "PhysicsEngineMain.h"
#include <btBulletDynamicsCommon.h>

struct PhysicsEngineMain::Impl 
{
public:
	Impl()
		: collisionConfig{ new btDefaultCollisionConfiguration() }
		, dispatcher{ new btCollisionDispatcher(collisionConfig) }
		, broadPhase{ new btDbvtBroadphase() }
		, solver{ new btSequentialImpulseConstraintSolver() }
		, world{ new btDiscreteDynamicsWorld(dispatcher, broadPhase, solver, collisionConfig) }
		, bodies{ new std::vector<std::tuple<glm::mat4*, btRigidBody*>>() }
	{
		world->setGravity(btVector3(0, -9.8f, 0));
	}

	void Update(float deltaTime)
	{
		world->stepSimulation(deltaTime);
		for (int i = 0; i < bodies->size(); ++i)
		{
			if (std::get<1>(bodies->at(i))->getCollisionShape()->getShapeType() == STATIC_PLANE_PROXYTYPE)
				ApplyPlanePhysics(bodies->at(i));
			else if (std::get<1>(bodies->at(i))->getCollisionShape()->getShapeType() == SPHERE_SHAPE_PROXYTYPE)
				ApplySpherePhysics(bodies->at(i));
		}
	}

	void ApplyPlanePhysics(std::tuple<glm::mat4*, btRigidBody*> rigidBody)
	{
		if (std::get<1>(rigidBody)->getCollisionShape()->getShapeType() != STATIC_PLANE_PROXYTYPE)
			return;

		btTransform t;
		std::get<1>(rigidBody)->getMotionState()->getWorldTransform(t);
		float mat[16];
		t.getOpenGLMatrix(mat);
		*(std::get<0>(rigidBody)) = glm::make_mat4(mat);
	}

	void ApplySpherePhysics(std::tuple<glm::mat4*, btRigidBody*> rigidBody)
	{
		if (std::get<1>(rigidBody)->getCollisionShape()->getShapeType() != SPHERE_SHAPE_PROXYTYPE)
			return;

		btTransform t;
		std::get<1>(rigidBody)->getMotionState()->getWorldTransform(t);
		float mat[16];
		t.getOpenGLMatrix(mat);
		*(std::get<0>(rigidBody)) = glm::make_mat4(mat);
	}

	void AddStaticPlane(float rad, float x, float y, float z, float mass, glm::mat4* model)
	{
		btTransform t;
		t.setIdentity();
		t.setOrigin(btVector3(0, 0, 0));
		btStaticPlaneShape* plane = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
		btMotionState* motion = new btDefaultMotionState(t);
		btRigidBody::btRigidBodyConstructionInfo info(0.0f, motion, plane);
		btRigidBody* body = new btRigidBody(info);
		world->addRigidBody(body);
		bodies->push_back(std::make_tuple(model, body));
	}

	void AddSphere(float rad, float x, float y, float z, float mass, glm::mat4* model)
	{
		btTransform t;
		t.setIdentity();
		t.setIdentity();
		t.setOrigin(btVector3(x, y, z));
		btSphereShape* sphere = new btSphereShape(rad);
		btVector3 inertia(0.0f, 0.0f, 0.0f);
		if (mass != 0.0) { sphere->calculateLocalInertia(mass, inertia); }
		btMotionState* motion = new btDefaultMotionState(t);
		btRigidBody::btRigidBodyConstructionInfo info(mass, motion, sphere, inertia);
		btRigidBody* body = new btRigidBody(info);
		world->addRigidBody(body);
		bodies->push_back(std::make_tuple(model, body));
	}

	~Impl()
	{
		for (int i = 0; i < bodies->size(); ++i)
		{
			world->removeCollisionObject(std::get<1>(bodies->at(i)));
			btMotionState* motionState = std::get<1>(bodies->at(i))->getMotionState();
			btCollisionShape* shape = std::get<1>(bodies->at(i))->getCollisionShape();
			delete std::get<1>(bodies->at(i));
			delete shape;
			delete motionState;
		}

		delete dispatcher;
		delete collisionConfig;
		delete solver;
		delete broadPhase;
		delete world;
		delete bodies;
	}

private:
	btCollisionConfiguration* collisionConfig;
	btDispatcher* dispatcher;
	btBroadphaseInterface* broadPhase;
	btConstraintSolver* solver;
	btDynamicsWorld* world;
	std::vector<std::tuple<glm::mat4*, btRigidBody*>>* bodies;
};

PhysicsEngineMain::PhysicsEngineMain() : m_pImpl{ new Impl() } {};

void PhysicsEngineMain::Update(float deltaTime)
{
	Pimpl()->Update(deltaTime);
}

void PhysicsEngineMain::AddStaticPlane(float rad, float x, float y, float z, float mass, glm::mat4* model)
{
	Pimpl()->AddStaticPlane(rad, x, y, z, mass, model);
}

void PhysicsEngineMain::AddSphere(float rad, float x, float y, float z, float mass, glm::mat4* model)
{
	Pimpl()->AddSphere(rad, x, y, z, mass, model);
}

PhysicsEngineMain::~PhysicsEngineMain()
{
	delete m_pImpl;
}
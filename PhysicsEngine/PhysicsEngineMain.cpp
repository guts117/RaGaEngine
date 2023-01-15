#include "physics_pch.h"
#include "PhysicsEngineMain.h"
#include <bullet/btBulletDynamicsCommon.h>

using namespace NarakaKarEngine;
using namespace PhysicsEngine;

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
			btTransform t;
			std::get<1>(bodies->at(i))->getMotionState()->getWorldTransform(t);
			auto ptr_mat = std::get<0>(bodies->at(i));
			if (ptr_mat != nullptr) 
			{
				t.getOpenGLMatrix(glm::value_ptr(*ptr_mat));
			}
		}
	}

	void AddStaticPlane(float x, float y, float z, float mass, glm::vec3 normal, glm::mat4* model)
	{
		btTransform t;
		t.setIdentity();
		t.setOrigin(btVector3(x, y, z));
		btStaticPlaneShape* plane = new btStaticPlaneShape(btVector3(normal.x, normal.y, normal.z), 0);
		btMotionState* motion = new btDefaultMotionState(t);
		btRigidBody::btRigidBodyConstructionInfo info(mass, motion, plane);
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

PhysicsEngineMain::PhysicsEngineMain() : m_pImpl{ std::make_unique<Impl>()} {};

void PhysicsEngineMain::Update(float deltaTime)
{
	Pimpl()->Update(deltaTime);
}

void PhysicsEngineMain::AddStaticPlane(float x, float y, float z, float mass, glm::vec3 normal, glm::mat4* model)
{
	Pimpl()->AddStaticPlane(x, y, z, mass, normal, model);
}

void PhysicsEngineMain::AddSphere(float rad, float x, float y, float z, float mass, glm::mat4* model)
{
	Pimpl()->AddSphere(rad, x, y, z, mass, model);
}

PhysicsEngineMain::~PhysicsEngineMain() = default;
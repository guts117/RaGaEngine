#ifndef PHYSICS_ENGINE
#define PHYSICS_ENGINE

#ifdef PHYSICS_ENGINE_EXPORTS
#define PHYSICS_ENGINE_API __declspec(dllexport)
#else
#define PHYSICS_ENGINE_API __declspec(dllimport)
#endif

#include "physics_pch.h"

class btDynamicsWorld;
class btDispatcher;
class btCollisionConfiguration;
class btBroadphaseInterface;
class btConstraintSolver;
class btRigidBody;

class PHYSICS_ENGINE_API PhysicsEngineMain
{
public:
	explicit PhysicsEngineMain();
	PhysicsEngineMain(PhysicsEngineMain&& rhs) noexcept = delete;
	PhysicsEngineMain& operator=(PhysicsEngineMain&& rhs) noexcept = delete;

	PhysicsEngineMain(const PhysicsEngineMain& rhs) noexcept = delete;
	PhysicsEngineMain& operator=(const PhysicsEngineMain& rhs) noexcept = delete;

	void Update(float deltaTime);

	void AddStaticPlane(float rad, float x, float y, float z, float mass, glm::mat4* model);
	void AddSphere(float rad, float x, float y, float z, float mass, glm::mat4* model);

	~PhysicsEngineMain();

private:
	btDynamicsWorld* world;
	btDispatcher* dispatcher;
	btCollisionConfiguration* collisionConfig;
	btBroadphaseInterface* broadPhase;
	btConstraintSolver* solver;
	std::vector<std::tuple<glm::mat4*, btRigidBody*>>* bodies;

	void ApplyPlanePhysics(std::tuple<glm::mat4*, btRigidBody*> rigidBody);
	void ApplySpherePhysics(std::tuple<glm::mat4*, btRigidBody*> rigidBody);
};
#endif //PHYSICS_ENGINE



#ifndef PHYSICS_ENGINE
#define PHYSICS_ENGINE

#include <btBulletDynamicsCommon.h>
#include "pch.h"

class PhysicsEngineMain
{
public:
	explicit PhysicsEngineMain();
	PhysicsEngineMain(PhysicsEngineMain&& rhs) noexcept = default;
	PhysicsEngineMain& operator=(PhysicsEngineMain&& rhs) noexcept = default;

	PhysicsEngineMain(const PhysicsEngineMain& rhs) noexcept = delete;
	PhysicsEngineMain& operator=(const PhysicsEngineMain& rhs) noexcept = delete;

	void Update(float deltaTime);

	void AddStaticPlane(float rad, float x, float y, float z, float mass, glm::mat4* model);
	void AddSphere(float rad, float x, float y, float z, float mass, glm::mat4* model);

	~PhysicsEngineMain();
private:
	std::unique_ptr <btDynamicsWorld> world;
	std::unique_ptr <btDispatcher> dispatcher;
	std::unique_ptr <btCollisionConfiguration> collisionConfig;
	std::unique_ptr <btBroadphaseInterface> broadPhase;
	std::unique_ptr <btConstraintSolver> solver;
	std::vector<std::tuple<glm::mat4*, btRigidBody*>> bodies;

	void ApplyPlanePhysics(std::tuple<glm::mat4*, btRigidBody*> rigidBody);
	void ApplySpherePhysics(std::tuple<glm::mat4*, btRigidBody*> rigidBody);
};
#endif //PHYSICS_ENGINE



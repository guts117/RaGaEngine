#ifndef PHYSICS_ENGINE
#define PHYSICS_ENGINE

#ifdef PHYSICS_ENGINE_EXPORTS
#define PHYSICS_ENGINE_API __declspec(dllexport)
#else
#define PHYSICS_ENGINE_API __declspec(dllimport)
#endif

#include "physics_pch.h"

namespace NarakaPhysicsEngine
{
	namespace PhysicsEngine
	{
		class PHYSICS_ENGINE_API PhysicsEngineMain
		{
		public:
			explicit PhysicsEngineMain();
			PhysicsEngineMain(PhysicsEngineMain&& rhs) noexcept = delete;
			PhysicsEngineMain& operator=(PhysicsEngineMain&& rhs) noexcept = delete;

			PhysicsEngineMain(const PhysicsEngineMain& rhs) noexcept = delete;
			PhysicsEngineMain& operator=(const PhysicsEngineMain& rhs) noexcept = delete;

			void Update(float deltaTime);

			void AddStaticPlane(float x, float y, float z, float mass, glm::vec3 normal, glm::mat4* model);
			void AddSphere(float rad, float x, float y, float z, float mass, glm::mat4* model);

			~PhysicsEngineMain();

		private:
			struct Impl;
			const Impl* Pimpl() const { return m_pImpl.get(); }
			Impl* Pimpl() { return m_pImpl.get(); }

			std::unique_ptr<Impl> m_pImpl;
		};
	}
}
#endif //PHYSICS_ENGINE



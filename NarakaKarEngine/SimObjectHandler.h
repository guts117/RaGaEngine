#ifndef SIM_OBJECT_HANDLER
#define SIM_OBJECT_HANDLER

#include "pch.h"

namespace NarakaKarEngine
{
	struct SimObject;
	class SimObjectHandler
	{
	public:
		explicit SimObjectHandler(std::shared_ptr<SimObject> simObj);

		//virtual void OnCreation();
		//virtual void OnFixedUpdate(); //This will run independent of the other function. the timing can be changed per function probably. Unity uses the physics deltatime.
		//virtual void OnPreUpdate();
		//virtual void OnUpdate();
		//virtual void OnPostUpdate();
		//virtual void OnDestruction();

		virtual ~SimObjectHandler() = 0;
	private:
		std::weak_ptr<SimObject> m_simObject;
	};
}

#endif //SIM_OBJECT_HANDLER
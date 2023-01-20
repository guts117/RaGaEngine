#include "pch.h"
#include "EngineSimulator.h"
#include "SimObject.h"
#include "Transform.h"

using namespace NarakaKarEngine;


struct EngineSimulatorMain::Impl
{
	std::vector<Transform> transformPool = std::vector<Transform>();
	std::vector<SimObject> simObjectpool = std::vector<SimObject>();

	Impl() = delete;

	Impl(Impl&& rhs) = delete;
	Impl& operator=(Impl&& rhs) = delete;

	Impl(const Impl& rhs) = delete;
	Impl& operator=(const Impl& rhs) = delete;

	void UpdateObjectTransforms()
	{
		for (auto& t : transformPool)
		{
			t.UpdateModelMatrix();
		}
	}

	void Update(const glm::ivec2& screenDims) 
	{
		UpdateObjectTransforms();
	}
	void EndUpdate() 
	{
	}

	~Impl() = default;
};


void EngineSimulatorMain::Update(const glm::ivec2& screenDims)
{
}

void EngineSimulatorMain::EndUpdate()
{
}

EngineSimulatorMain::~EngineSimulatorMain()
{
}

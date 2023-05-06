#include "pch.h"
#include "EngineSimulatorMain.h"
#include "SimObject.h"
#include "Transform.h"

using namespace NarakaKarEngine;


struct EngineSimulatorMainMain::Impl
{
	std::vector<Transform> transformPool = std::vector<Transform>();
	std::vector<SimObject> simObjectpool = std::vector<SimObject>();

	//where game loops happen;
	std::vector<std::function<void()>> onCreation;
	std::vector<std::function<void()>> startFuncPool;
	std::vector<std::function<void()>> preUpdateFuncPool;
	std::vector<std::function<void()>> updateFuncPool;
	std::vector<std::function<void()>> endUpdateFuncPool;
	std::vector<std::function<void()>> endFuncPool;

	Impl() noexcept = delete;

	Impl(Impl&& rhs) noexcept = delete;
	Impl& operator=(Impl&& rhs) noexcept = delete;

	Impl(const Impl& rhs) noexcept = delete;
	Impl& operator=(const Impl& rhs) noexcept = delete;

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

	~Impl() noexcept = default;
};


void EngineSimulatorMainMain::Update(const glm::ivec2& screenDims)
{
}

void EngineSimulatorMainMain::EndUpdate()
{
}

void EngineSimulatorMainMain::AddToUpdateStack(std::function<void(glm::ivec2)>)
{
}

EngineSimulatorMainMain::~EngineSimulatorMainMain() noexcept = default;

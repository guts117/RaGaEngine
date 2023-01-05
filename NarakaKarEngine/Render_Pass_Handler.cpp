#include "pch.h"
#include "Render_Pass_Handler.h"
#include "Fbo_Handler.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

Render_Pass_Handler::Render_Pass_Handler(std::shared_ptr<Fbo_Handler> fboHandlr
													, const std::vector<std::shared_ptr<Shader_Object>>& shaderVec
													, std::shared_ptr<std::vector<std::shared_ptr<std::any>>> inputs)
													: m_fboHandler{ fboHandlr }
													, m_shaderVec{ std::make_unique<std::vector<std::shared_ptr<Shader_Object>>>(shaderVec) }
													, m_inputs{ inputs }
													, m_outputs{ std::make_shared<std::vector<std::shared_ptr<GLint>>>() }
{
}

void Render_Pass_Handler::Init()
{
}

template <typename T>
std::shared_ptr<T> Render_Pass_Handler::CheckInputDataType(const std::any& data)
{
	try
	{
		auto dat = std::any_cast<T>(data);
		return std::make_shared<T>(dat);
	}
	catch (const std::bad_any_cast& e)
	{
		std::cout << e.what() << std::endl;
		return nullptr;
	}
}

Render_Pass_Handler::~Render_Pass_Handler() = default;

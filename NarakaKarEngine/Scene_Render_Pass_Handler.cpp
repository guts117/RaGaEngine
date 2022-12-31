#include "pch.h"
#include "Scene_Render_Pass_Handler.h"
#include "Fbo_Handler.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

Scene_Render_Pass_Handler::Scene_Render_Pass_Handler(std::shared_ptr<Fbo_Handler> fboHandlr
													, const std::vector<std::shared_ptr<Shader_Object>>& shaderVec
													, std::shared_ptr<std::vector<std::shared_ptr<std::any>>> inputs)
													: m_fboHandler{ fboHandlr }
													, m_shaderVec{ std::make_unique<std::vector<std::shared_ptr<Shader_Object>>>(shaderVec) }
													, m_inputs{ inputs }
													, m_outputs{ std::make_shared<std::vector<std::shared_ptr<GLint>>>() }
{
}

void Scene_Render_Pass_Handler::Init()
{
}

Scene_Render_Pass_Handler::~Scene_Render_Pass_Handler() = default;

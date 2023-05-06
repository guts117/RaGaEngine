#include "render_pch.h"
#include "Render_Pass_Handler.h"
#include "Fbo_Handler.h"

using namespace NarakaRenderEngine;
using namespace RenderEngine;

Render_Pass_Handler::Render_Pass_Handler(Fbo_Handler* fboHandlr
													, std::vector<Shader_Object*>&& shaderVec
													, std::shared_ptr<std::vector<std::shared_ptr<std::any>>> inputs)
													: m_fboHandler{ fboHandlr }
													, m_shaderVec{ std::move(shaderVec) }
													, m_inputs{ inputs }
													, m_outputs{ std::make_shared<std::vector<std::shared_ptr<GLint>>>() }
{
}

void Render_Pass_Handler::Init()
{
}

Render_Pass_Handler::~Render_Pass_Handler() = default;
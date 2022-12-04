#include "pch.h"
#include "PreZ_Render_Pass_Handler.h"
#include "Fbo_Handler.h"
#include "Shader_Object.h"
#include "Render_Object.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

PreZ_Render_Pass_Handler::PreZ_Render_Pass_Handler(std::shared_ptr<Fbo_Handler> fboHandlr
	, const std::vector<std::shared_ptr<Shader_Object>>& shaderVec
	, std::shared_ptr<std::vector<std::shared_ptr<std::any>>> inputs)
	: Scene_Render_Pass_Handler(fboHandlr, shaderVec, inputs)
{
}

void PreZ_Render_Pass_Handler::Update(std::shared_ptr<std::vector<std::shared_ptr<Render_Object>>> renderObj, const CamParam& camParam)
{
	glViewport(0, 0, m_fboHandler->GetFBOWidth(), m_fboHandler->GetFBOHeight());

	m_fboHandler->BindFBO();
	//clear depth buffer
	glClear(GL_DEPTH_BUFFER_BIT);

	for(auto shaderIndex = 0; shaderIndex < m_shaderVec->size(); ++shaderIndex)
	{
		m_shaderVec->at(shaderIndex)->ResetTextureUnit(0);
		m_shaderVec->at(shaderIndex)->UseShaderObject();

		m_shaderVec->at(shaderIndex)->SetVariable("Projection", camParam.Projection);
		m_shaderVec->at(shaderIndex)->SetVariable("View", camParam.View);
		m_shaderVec->at(shaderIndex)->SetVariable("eyePosition", camParam.Position);

		for (auto roIndex = 0; roIndex < renderObj->size(); ++roIndex) 
		{
			renderObj->at(roIndex)->RenderObject(m_shaderVec->at(shaderIndex), camParam.PrevProjView);
		}
		m_shaderVec->at(shaderIndex)->ValidateShaderObject();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

PreZ_Render_Pass_Handler::~PreZ_Render_Pass_Handler() = default;
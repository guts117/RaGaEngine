#include "pch.h"
#include "Brdf_Render_Pass_Handler.h"
#include "Fbo_Handler.h"
#include "Shader_Object.h"
#include "Render_Object.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

Brdf_Render_Pass_Handler::Brdf_Render_Pass_Handler(std::shared_ptr<Fbo_Handler> fboHandlr
	, const std::vector<std::shared_ptr<Shader_Object>>& shaderVec
	, std::shared_ptr<std::vector<std::shared_ptr<std::any>>> inputs)
	: Render_Pass_Handler(fboHandlr, shaderVec, inputs)
{
}

void Brdf_Render_Pass_Handler::Update(const std::vector<std::vector<std::shared_ptr<Render_Object>>>& renderObj, const CamParam* camParam, const LightParam* lightParam)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, m_fboHandler->GetFBOWidth(), m_fboHandler->GetFBOHeight());

	m_fboHandler->BindFBO();
	//clear depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (auto shaderIndex = 0; shaderIndex < m_shaderVec->size(); ++shaderIndex)
	{
		m_shaderVec->at(shaderIndex)->ResetTextureUnit(0);
		m_shaderVec->at(shaderIndex)->UseShaderObject();

		for (auto roIndex = 0; roIndex < renderObj[shaderIndex].size(); ++roIndex)
		{
			renderObj[shaderIndex][roIndex]->RenderObject(*m_shaderVec->at(shaderIndex), std::move(RenderObjectParams{}));
		}
		m_shaderVec->at(shaderIndex)->ValidateShaderObject();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Brdf_Render_Pass_Handler::~Brdf_Render_Pass_Handler() = default;
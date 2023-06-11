#include "render_pch.h"
#include "Brdf_Render_Pass_Handler.h"
#include "Fbo_Handler.h"
#include "Shader_Object.h"
#include "Render_Object.h"

using namespace NarakaRenderEngine;
using namespace RenderEngine;

Brdf_Render_Pass_Handler::Brdf_Render_Pass_Handler(Fbo_Handler* fboHandlr
	, std::vector<rw_clustering_ptr<Shader_Object>>&& shaderVec
	, std::shared_ptr<std::vector<std::shared_ptr<std::any>>> inputs)
	: Render_Pass_Handler(fboHandlr, std::move(shaderVec), inputs)
{
}

void Brdf_Render_Pass_Handler::Update(std::vector<std::vector<Render_Object>>& renderObj, const CamParam* camParam, const LightParam* lightParam)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, m_fboHandler->GetFBOWidth(), m_fboHandler->GetFBOHeight());

	m_fboHandler->BindFBO();
	//clear depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (auto shaderIndex = 0; shaderIndex < m_shaderVec.size(); ++shaderIndex)
	{
		if (shaderIndex >= renderObj.size()) { break; }

		auto& shader = m_shaderVec[shaderIndex];
		//shader->ResetTextureUnit(0);
		shader.oneTimeWrite(&Shader_Object::ResetTextureUnit, 0);
		shader->UseShaderObject();

		for (auto roIndex = 0; roIndex < renderObj[shaderIndex].size(); ++roIndex)
		{
			renderObj[shaderIndex][roIndex].RenderObject(shader, std::move(RenderObjectParams{}));
		}
		shader->ValidateShaderObject();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Brdf_Render_Pass_Handler::~Brdf_Render_Pass_Handler() = default;
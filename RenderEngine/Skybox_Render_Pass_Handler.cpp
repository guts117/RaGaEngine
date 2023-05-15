#include "render_pch.h"
#include "Skybox_Render_Pass_Handler.h"
#include "Fbo_Handler.h"
#include "Shader_Object.h"
#include "Render_Object.h"
#include "RenderingCommonValues.h"

using namespace NarakaRenderEngine;
using namespace RenderEngine;

Skybox_Render_Pass_Handler::Skybox_Render_Pass_Handler(Fbo_Handler* fboHandlr
	, std::vector<rw_clustering_ptr<Shader_Object>>&& shaderVec
	, std::shared_ptr<std::vector<std::shared_ptr<std::any>>> inputs)
	: Render_Pass_Handler(fboHandlr, std::move(shaderVec), inputs)
{
}

void Skybox_Render_Pass_Handler::Update(std::vector<std::vector<Render_Object>>& renderObj, const CamParam* camParam, const LightParam* lightParam)
{
	glViewport(0, 0, m_fboHandler->GetFBOWidth(), m_fboHandler->GetFBOHeight());

	m_fboHandler->BindFBO();

	for (auto shaderIndex = 0; shaderIndex < m_shaderVec.size(); ++shaderIndex)
	{
		if (shaderIndex >= renderObj.size()) { break; }

		auto& shader = m_shaderVec[shaderIndex];
		auto viewMatrix = glm::mat4(glm::mat3(camParam->View));
		auto prevPV = camParam->PrevProj * glm::mat4(glm::mat3(camParam->PrevView));

		shader.write(std::mem_fn(&Shader_Object::ResetTextureUnit), 0);
		glDepthFunc(GL_LEQUAL); //so that skybox doesn't render on top
		shader->UseShaderObject();

		shader->SetVariable("Projection", camParam->Projection);
		shader->SetVariable("View", viewMatrix);
		shader->SetVariable("prevPV", prevPV);

		if (auto val = CheckInputDataType<Fbo_Handler*>(*m_inputs->at(0)))
		{
			shader.write(std::mem_fn(&Shader_Object::SetTextureUnit), "skybox");
			val->AttachFBOToTextureUnit(0, shader->GetTextureUnit(), 0, 0);
		}

		for (auto roIndex = 0; roIndex < renderObj[shaderIndex].size(); ++roIndex)
		{
			renderObj[shaderIndex][roIndex].RenderObject(shader, std::move(RenderObjectParams{ }));
		}
		shader->ValidateShaderObject();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Skybox_Render_Pass_Handler::~Skybox_Render_Pass_Handler() = default;


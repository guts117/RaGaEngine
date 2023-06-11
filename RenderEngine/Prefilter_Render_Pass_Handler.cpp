#include "render_pch.h"
#include "Prefilter_Render_Pass_Handler.h"
#include "Fbo_Handler.h"
#include "Shader_Object.h"
#include "Render_Object.h"

using namespace NarakaRenderEngine;
using namespace RenderEngine;

Prefilter_Render_Pass_Handler::Prefilter_Render_Pass_Handler(Fbo_Handler* fboHandlr
	, std::vector<rw_clustering_ptr<Shader_Object>>&& shaderVec
	, std::shared_ptr<std::vector<std::shared_ptr<std::any>>> inputs)
	: Render_Pass_Handler(fboHandlr, std::move(shaderVec), inputs)
{
}

void Prefilter_Render_Pass_Handler::Update(std::vector<std::vector<Render_Object>>& renderObj, const CamParam* camParam, const LightParam* lightParam)
{
	auto captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);	

	glm::mat4 captureViews[6] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
	};

	m_fboHandler->BindFBO();
	auto maxMipLevels = 5;

	for (auto shaderIndex = 0; shaderIndex < m_shaderVec.size(); ++shaderIndex)
	{
		if (shaderIndex >= renderObj.size()) { break; }

		auto& shader = m_shaderVec.at(shaderIndex);
		shader->UseShaderObject();
		shader->SetVariable("skybox", 0);
		shader->SetVariable("Projection", captureProjection);
		//shader->ResetTextureUnit(0);
		shader.invoke(&Shader_Object::ResetTextureUnit, 0);

		for (auto mip = 0; mip < maxMipLevels; ++mip)
		{
			//resize framebuffer according to mip-level size
			auto mipWidth = m_fboHandler->GetFBOWidth() * std::pow(0.5, mip);
			auto mipHeight = m_fboHandler->GetFBOHeight() * std::pow(0.5, mip);
			glViewport(0, 0, mipWidth, mipHeight);

			float roughness = (float)mip / (float)(maxMipLevels - 1);
			shader->SetVariable("roughness", roughness);
			for (auto faceId = 0; faceId < 6; ++faceId)
			{
				shader->SetVariable("View", captureViews[faceId]);
				m_fboHandler->WriteToFBOBuffer(0, 0, 0, faceId, mip);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				shader->ValidateShaderObject();

				if (auto val = CheckInputDataType<Fbo_Handler*>(*m_inputs->at(0))) 
				{
					//shader->SetTextureUnit("skybox");
					shader.invoke(&Shader_Object::SetTextureUnit, "skybox");
					val->AttachFBOToTextureUnit(0, shader->GetTextureUnit(), 0, 0);
				}

				for (auto roIndex = 0; roIndex < renderObj[shaderIndex].size(); ++roIndex)
				{
					renderObj[shaderIndex][roIndex].RenderObject(shader, std::move(RenderObjectParams{}));
				}
			}
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Prefilter_Render_Pass_Handler::~Prefilter_Render_Pass_Handler() = default;

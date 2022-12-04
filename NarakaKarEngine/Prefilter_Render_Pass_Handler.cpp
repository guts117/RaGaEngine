#include "pch.h"
#include "Prefilter_Render_Pass_Handler.h"
#include "Fbo_Handler.h"
#include "Shader_Object.h"
#include "Render_Object.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

Prefilter_Render_Pass_Handler::Prefilter_Render_Pass_Handler(std::shared_ptr<Fbo_Handler> fboHandlr
	, const std::vector<std::shared_ptr<Shader_Object>>& shaderVec
	, std::shared_ptr<std::vector<std::shared_ptr<std::any>>> inputs)
	: Scene_Render_Pass_Handler(fboHandlr, shaderVec, inputs)
{
}

void Prefilter_Render_Pass_Handler::Update(std::shared_ptr<std::vector<std::shared_ptr<Render_Object>>> renderObj, const CamParam& camParam)
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

	for (auto shaderIndex = 0; shaderIndex < m_shaderVec->size(); ++shaderIndex)
	{
		auto shader = m_shaderVec->at(shaderIndex);
		shader->UseShaderObject();
		shader->SetVariable("skybox", 0);
		shader->SetVariable("Projection", captureProjection);

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
				shader->ResetTextureUnit(0);

				try
				{
					auto input = std::any_cast<std::shared_ptr<Fbo_Handler>>(*m_inputs->at(0));
					input->AttachFBOToTextureUnit(0, GL_TEXTURE0, 0, 0);
				}
				catch (const std::bad_any_cast& e)
				{
					std::cout << e.what() << std::endl;
				}

				for (auto roIndex = 0; roIndex < renderObj->size(); ++roIndex)
				{
					renderObj->at(roIndex)->RenderObject(shader, camParam.PrevProjView, false);
				}
			}
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Prefilter_Render_Pass_Handler::~Prefilter_Render_Pass_Handler() = default;

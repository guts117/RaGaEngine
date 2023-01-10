#include "pch.h"
#include "Irradiance_Convolution_Render_Pass_Handler.h"
#include "Fbo_Handler.h"
#include "Shader_Object.h"
#include "Render_Object.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

Irradiance_Convolution_Render_Pass_Handler::Irradiance_Convolution_Render_Pass_Handler(std::shared_ptr<Fbo_Handler> fboHandlr
	, const std::vector<std::shared_ptr<Shader_Object>>& shaderVec
	, std::shared_ptr<std::vector<std::shared_ptr<std::any>>> inputs)
	: Render_Pass_Handler(fboHandlr, shaderVec, inputs)
{
}

void Irradiance_Convolution_Render_Pass_Handler::Update(const std::vector<std::vector<std::shared_ptr<Render_Object>>>& renderObj, const CamParam* camParam, const LightParam* lightParam)
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

	glViewport(0, 0, m_fboHandler->GetFBOWidth(), m_fboHandler->GetFBOHeight());
	m_fboHandler->BindFBO();

	for (auto shaderIndex = 0; shaderIndex < m_shaderVec->size(); ++shaderIndex)
	{
		if (shaderIndex >= renderObj.size()) { return; }

		auto& shader = m_shaderVec->at(shaderIndex);

		shader->ResetTextureUnit(0);
		shader->UseShaderObject();
		shader->SetVariable("skybox", 0);
		shader->SetVariable("Projection", captureProjection);

		for (auto faceId = 0; faceId < 6; ++faceId)
		{
			shader->SetVariable("View", captureViews[faceId]);
			m_fboHandler->WriteToFBOBuffer(0, 0, 0, faceId);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			shader->ValidateShaderObject();

			if (auto val = CheckInputDataType<std::shared_ptr<Fbo_Handler>>(*m_inputs->at(0))) 
			{
				val->get()->AttachFBOToTextureUnit(0, shader->SetTextureUnit("skybox"), 0, 0);
			}
			
			shader->ResetTextureUnit(0);
			for (auto roIndex = 0; roIndex < renderObj[shaderIndex].size(); ++roIndex)
			{
				renderObj[shaderIndex][roIndex]->RenderObject(*shader, std::move(RenderObjectParams{}));
			}
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Irradiance_Convolution_Render_Pass_Handler::~Irradiance_Convolution_Render_Pass_Handler() = default;
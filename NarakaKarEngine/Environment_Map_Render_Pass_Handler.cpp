#include "pch.h"
#include "Environment_Map_Render_Pass_Handler.h"
#include "Fbo_Handler.h"
#include "Shader_Object.h"
#include "Render_Object.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

Environment_Map_Render_Pass_Handler::Environment_Map_Render_Pass_Handler(std::shared_ptr<Fbo_Handler> fboHandlr
	, const std::vector<std::shared_ptr<Shader_Object>>& shaderVec
	, std::shared_ptr<std::vector<std::shared_ptr<std::any>>> inputs)
	: Render_Pass_Handler(fboHandlr, shaderVec, inputs)
{
}

void Environment_Map_Render_Pass_Handler::Update(const std::vector<std::vector<std::shared_ptr<Render_Object>>>& renderObj, const CamParam* camParam, const LightParam* lightParam)
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
		auto shader = m_shaderVec->at(shaderIndex);

		shader->ResetTextureUnit(0);
		shader->UseShaderObject();
		shader->SetVariable("Projection", captureProjection);

		for (auto faceId = 0; faceId < 6; ++faceId)
		{
			shader->SetVariable("View", captureViews[faceId]);
			m_fboHandler->WriteToFBOBuffer(0, 0, 0, faceId);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			shader->ValidateShaderObject();

			shader->ResetTextureUnit(0);
			for (auto roIndex = 0; roIndex < renderObj[shaderIndex].size(); ++roIndex)
			{
				renderObj[shaderIndex][roIndex]->RenderObject(*shader, std::move(RenderObjectParams{false, true}));
			}
		}
	}

	m_fboHandler->CreateFBOMipMap(0, 0, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Environment_Map_Render_Pass_Handler::~Environment_Map_Render_Pass_Handler() = default;

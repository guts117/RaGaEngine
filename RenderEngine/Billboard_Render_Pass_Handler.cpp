#include "render_pch.h"
#include "Billboard_Render_Pass_Handler.h"
#include "Fbo_Handler.h"
#include "Shader_Object.h"
#include "Render_Object.h"
#include "RenderingCommonValues.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

using namespace NarakaRenderEngine;
using namespace RenderEngine;

Billboard_Render_Pass_Handler::Billboard_Render_Pass_Handler(std::shared_ptr<Fbo_Handler> fboHandlr
	, const std::vector<std::shared_ptr<Shader_Object>>& shaderVec
	, std::shared_ptr<std::vector<std::shared_ptr<std::any>>> inputs)
	: Render_Pass_Handler(fboHandlr, shaderVec, inputs)
{
}

void Billboard_Render_Pass_Handler::Update(const std::vector<std::vector<std::shared_ptr<Render_Object>>>& renderObj, const CamParam* camParam, const LightParam* lightParam)
{
	glViewport(0, 0, m_fboHandler->GetFBOWidth(), m_fboHandler->GetFBOHeight());

	m_fboHandler->BindFBO();

	for (auto shaderIndex = 0; shaderIndex < m_shaderVec->size(); ++shaderIndex)
	{
		if (shaderIndex >= renderObj.size()) { break; }

		auto& shader = m_shaderVec->at(shaderIndex);

		shader->ResetTextureUnit(0);
		shader->UseShaderObject();

		shader->SetVariable("Projection", camParam->Projection);
		shader->SetVariable("View", camParam->View);
		shader->SetVariable("CameraUp_worldspace", camParam->Up);
		shader->SetVariable("CameraRight_worldspace", camParam->Right);
		shader->SetVariable("prevPV", camParam->PrevProjView);

		for (auto roIndex = 0; roIndex < renderObj[shaderIndex].size(); ++roIndex)
		{
			auto& ro = renderObj[shaderIndex][roIndex];
			if(auto modelMatrix = ro->GetModelMatrix().lock())
			{
				glm::vec3 scale;
				glm::quat rotation;
				glm::vec3 translation;
				glm::vec3 skew;
				glm::vec4 perspective;
				glm::decompose(*modelMatrix, scale, rotation, translation, skew, perspective);
				//rotation = glm::conjugate(rotation);
				shader->SetVariable("BillboardPos", glm::vec3(0.0f, -2.0f, 0.0f));//translation);
				shader->SetVariable("BillboardSize", glm::vec2(2.0f, 2.0f));//scale.x, scale.y));
			}
			ro->RenderObject(*shader, std::move(RenderObjectParams{ false, true }));
		}
		shader->ValidateShaderObject();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Billboard_Render_Pass_Handler::~Billboard_Render_Pass_Handler() = default;


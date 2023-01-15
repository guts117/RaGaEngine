#include "render_pch.h"
#include "Omni_Directional_Shadow_Map_Render_Pass_Handler.h"
#include "Fbo_Handler.h"
#include "Shader_Object.h"
#include "Render_Object.h"
#include "RenderingCommonValues.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

Omni_Directional_Shadow_Map_Render_Pass_Handler::Omni_Directional_Shadow_Map_Render_Pass_Handler(std::shared_ptr<Fbo_Handler> fboHandlr
	, const std::vector<std::shared_ptr<Shader_Object>>& shaderVec
	, std::shared_ptr<std::vector<std::shared_ptr<std::any>>> inputs)
	: Render_Pass_Handler(fboHandlr, shaderVec, inputs)
{
}

void SetLightTransform(const Shader_Object* shader, const int& lightIndex, const LightParam* lightParam)
{
	auto& position = lightParam->Position[lightIndex];
	auto& projection = lightParam->Projection[lightIndex];

	std::vector<glm::mat4> lightMatrices = 
	{
		//+x, -x
		projection * glm::lookAt(position, position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
		projection * glm::lookAt(position, position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
		//+y, -y
		projection * glm::lookAt(position, position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
		projection * glm::lookAt(position, position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)),
		//+z,  -z
		projection * glm::lookAt(position, position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)),
		projection * glm::lookAt(position, position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)),
	};

	for (auto i = 0; i < lightMatrices.size(); ++i) 
	{
		shader->SetVariable("lightMatrices", lightMatrices[i], i);
	}
}

void Omni_Directional_Shadow_Map_Render_Pass_Handler::Update(const std::vector<std::vector<std::shared_ptr<Render_Object>>>& renderObj, const CamParam* camParam, const LightParam* lightParam)
{
	for (auto lightIndex = 0; lightIndex < lightParam->Count; ++lightIndex)
	{
		glViewport(0, 0, m_fboHandler->GetFBOWidth(lightIndex), m_fboHandler->GetFBOHeight(lightIndex));
		m_fboHandler->BindFBO(lightIndex);
		glClear(GL_DEPTH_BUFFER_BIT);

		for (auto shaderIndex = 0; shaderIndex < m_shaderVec->size(); ++shaderIndex)
		{
			if (shaderIndex >= renderObj.size()) { break; }

			auto& shader = m_shaderVec->at(shaderIndex);

			shader->ResetTextureUnit(0);
			shader->UseShaderObject();
			shader->SetVariable("lightPos", lightParam->Position[lightIndex]);
			shader->SetVariable("farPlane", lightParam->FarPlane[lightIndex]);
			SetLightTransform(shader.get(), lightIndex, lightParam);
			shader->ValidateShaderObject();

			for (auto roIndex = 0; roIndex < renderObj[shaderIndex].size(); ++roIndex)
			{
				auto& ro = renderObj[shaderIndex][roIndex];
				if (ro->IsTesselated())
				{
					shader->SetVariable("eyePosition", camParam->Position);
					ro->RenderObject(*shader, std::move(RenderObjectParams{ true, true }));
				}
				else
				{
					ro->RenderObject(*shader, std::move(RenderObjectParams{ true }));
				}
			}
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Omni_Directional_Shadow_Map_Render_Pass_Handler::~Omni_Directional_Shadow_Map_Render_Pass_Handler() = default;

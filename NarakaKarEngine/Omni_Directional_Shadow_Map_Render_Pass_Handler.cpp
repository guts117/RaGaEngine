#include "pch.h"
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
	std::vector<glm::mat4> lightMatrices;
	auto position = lightParam->Position[lightIndex];
	auto projection = lightParam->Projection[lightIndex];
	//+x, -x
	lightMatrices.push_back(projection * glm::lookAt(position, position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	lightMatrices.push_back(projection * glm::lookAt(position, position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	//+y, -y
	lightMatrices.push_back(projection * glm::lookAt(position, position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	lightMatrices.push_back(projection * glm::lookAt(position, position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
	//+z,  -z
	lightMatrices.push_back(projection * glm::lookAt(position, position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
	lightMatrices.push_back(projection * glm::lookAt(position, position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

	for (auto i = 0; i < lightMatrices.size(); ++i) 
	{
		shader->SetVariable("lightMatrices", lightMatrices[i]);
	}
}

void Omni_Directional_Shadow_Map_Render_Pass_Handler::Update(const std::vector<std::vector<std::shared_ptr<Render_Object>>>& renderObj, const CamParam* camParam, const LightParam* lightParam)
{
	for (auto lightIndex = 0; lightIndex < lightParam->Count; ++lightIndex)
	{
		m_fboHandler->BindFBO(lightIndex);

		glViewport(0, 0, m_fboHandler->GetFBOWidth(lightIndex), m_fboHandler->GetFBOHeight(lightIndex));
		glClear(GL_DEPTH_BUFFER_BIT);

		for (auto shaderIndex = 0; shaderIndex < m_shaderVec->size(); ++shaderIndex)
		{
			auto shader = m_shaderVec->at(shaderIndex);

			shader->ResetTextureUnit(0);
			shader->UseShaderObject();
			shader->SetVariable("lightPos", lightParam->Position[lightIndex]);
			shader->SetVariable("farPlane", lightParam->FarPlane[lightIndex]);
			SetLightTransform(shader.get(), lightIndex, lightParam);
			shader->ValidateShaderObject();

			for (auto roIndex = 0; roIndex < renderObj[shaderIndex].size(); ++roIndex)
			{
				auto ro = renderObj[shaderIndex][roIndex];
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

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

Omni_Directional_Shadow_Map_Render_Pass_Handler::~Omni_Directional_Shadow_Map_Render_Pass_Handler() = default;

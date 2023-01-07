#include "pch.h"
#include "Scene_Render_Pass_Handler.h"
#include "Fbo_Handler.h"
#include "Shader_Object.h"
#include "Render_Object.h"
#include "RenderingCommonValues.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

Scene_Render_Pass_Handler::Scene_Render_Pass_Handler(std::shared_ptr<Fbo_Handler> fboHandlr
	, const std::vector<std::shared_ptr<Shader_Object>>& shaderVec
	, std::shared_ptr<std::vector<std::shared_ptr<std::any>>> inputs)
	: Render_Pass_Handler(fboHandlr, shaderVec, inputs)
{
}

void Scene_Render_Pass_Handler::Update(const std::vector<std::vector<std::shared_ptr<Render_Object>>>& renderObj, const CamParam* camParam, const LightParam* lightParam)
{
	glViewport(0, 0, m_fboHandler->GetFBOWidth(), m_fboHandler->GetFBOHeight());

	m_fboHandler->BindFBO();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	std::shared_ptr<Fbo_Handler> val;
	auto inputTexBuffs = std::vector<std::string>{ "irradianceMap" , "prefilterMap", "brdfLUT", "AOMap", "depthMap" };

	for (auto shaderIndex = 0; shaderIndex < m_shaderVec->size(); ++shaderIndex)
	{
		auto& shader = m_shaderVec->at(shaderIndex);

		shader->ResetTextureUnit(0);
		shader->UseShaderObject();

		shader->SetVariable("Projection", camParam->Projection);
		shader->SetVariable("View", camParam->View);
		shader->SetVariable("SpotLightCount", 1);
		shader->SetVariable("directionalLight", *lightParam[0].Color, 0, "color");
		shader->SetVariable("directionalLight", *lightParam[0].Direction, 0, "direction");
		shader->SetVariable("eyePosition", camParam->Position);
		shader->SetVariable("height_scale", 0.02f);

		auto inputOffset = 0;

		for (auto cascId = 0; cascId < NUM_CASCADES; ++cascId)
		{
			shader->SetVariable("DirectionalLightTransforms", lightParam[0].Projection[cascId] * lightParam[0].View[cascId], cascId);
			shader->SetVariable("CascadeEndClipSpace", lightParam[0].Edge, cascId);

			if (CheckInputDataType<std::shared_ptr<Fbo_Handler>>(val, *m_inputs->at(inputOffset)))
			{
				val->AttachFBOToTextureUnit(0, shader->SetTextureUnit("directionalShadowMaps", cascId, "shadowMap"), 0, 0);
			}
		}
		++inputOffset;

		auto hasOmniShadowFbo = CheckInputDataType<std::shared_ptr<Fbo_Handler>>(val, *m_inputs->at(inputOffset));

		if (hasOmniShadowFbo) 
		{
			for (auto omniLightIndex = 0; omniLightIndex < lightParam[1].Count + lightParam[2].Count; ++omniLightIndex)
			{
				val->AttachFBOToTextureUnit(0, shader->SetTextureUnit("omniShadowMaps", omniLightIndex, "shadowMap"), 0, 0);

				if (omniLightIndex < lightParam[1].Count)
				{
					shader->SetVariable("omniShadowMaps", lightParam[1].FarPlane[omniLightIndex], omniLightIndex, "farPlane");
				}
				else
				{
					auto spotlightIndex = omniLightIndex - lightParam[1].Count;
					shader->SetVariable("omniShadowMaps", lightParam[2].FarPlane[spotlightIndex], omniLightIndex, "farPlane");
					shader->SetVariable("spotLights", lightParam[2].Position[spotlightIndex], spotlightIndex, "position");
					shader->SetVariable("spotLights", lightParam[2].Color[spotlightIndex], spotlightIndex, "color");
					shader->SetVariable("spotLights", lightParam[2].Direction[spotlightIndex], spotlightIndex, "direction");
					shader->SetVariable("spotLights", lightParam[2].Edge[spotlightIndex], spotlightIndex, "edge");
				}
			}
		}
		++inputOffset;

		for(auto inputIndex = inputOffset; inputIndex < m_inputs->size(); ++inputIndex)
		{
			if (CheckInputDataType<std::shared_ptr<Fbo_Handler>>(val, *m_inputs->at(inputOffset)))
			{
				val->AttachFBOToTextureUnit(0, shader->SetTextureUnit(std::move(inputTexBuffs[inputIndex - inputOffset])), 0, 0);
			}
		}

		for (auto roIndex = 0; roIndex < renderObj[shaderIndex].size(); ++roIndex)
		{
			auto& ro = renderObj[shaderIndex][roIndex];
			ro->RenderObject(*shader, std::move(RenderObjectParams{ true , true, &camParam->PrevProjView }));
		}
		shader->ValidateShaderObject();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Scene_Render_Pass_Handler::~Scene_Render_Pass_Handler() = default;
#include "render_pch.h"
#include "Scene_Render_Pass_Handler.h"
#include "Fbo_Handler.h"
#include "Shader_Object.h"
#include "Render_Object.h"
#include "RenderingCommonValues.h"

using namespace NarakaRenderEngine;
using namespace RenderEngine;

Scene_Render_Pass_Handler::Scene_Render_Pass_Handler(Fbo_Handler* fboHandlr
	, std::vector<rw_clustering_ptr<Shader_Object>>&& shaderVec
	, std::shared_ptr<std::vector<std::shared_ptr<std::any>>> inputs)
	: Render_Pass_Handler(fboHandlr, std::move(shaderVec), inputs)
{
}

void Scene_Render_Pass_Handler::Update(std::vector<std::vector<Render_Object>>& renderObj, const CamParam* camParam, const LightParam* lightParam)
{
	glViewport(0, 0, m_fboHandler->GetFBOWidth(), m_fboHandler->GetFBOHeight());

	m_fboHandler->BindFBO();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto inputTexBuffs = std::vector<std::string>{ "irradianceMap" , "prefilterMap", "brdfLUT", "AOMap", "depthMap" };

	for (auto shaderIndex = 0; shaderIndex < m_shaderVec.size(); ++shaderIndex)
	{
		if (shaderIndex >= renderObj.size()) { break; }

		auto& shader = m_shaderVec[shaderIndex];

		//shader->ResetTextureUnit(0);
		shader.oneTimeWrite(&Shader_Object::ResetTextureUnit, 0);
		shader->UseShaderObject();

		shader->SetVariable("Projection", camParam->Projection);
		shader->SetVariable("View", camParam->View);
		shader->SetVariable("eyePosition", camParam->Position);
		shader->SetVariable("height_scale", 0.02f);

		auto inputOffset = 0;

		if (auto val = CheckInputDataType<Fbo_Handler*>(*m_inputs->at(inputOffset)))
		{
			shader->SetVariable("directionalLight", *lightParam[0].Color, 0, "color");
			shader->SetVariable("directionalLight", *lightParam[0].Direction, 0, "direction");

			for (auto cascId = 0; cascId < NUM_CASCADES; ++cascId)
			{
				//shader->SetTextureUnitStructArr("directionalShadowMaps", cascId, "shadowMap");
				shader.oneTimeWrite(&Shader_Object::SetTextureUnitStructArr, "directionalShadowMaps", cascId, "shadowMap");
				val->AttachFBOToTextureUnit(0, shader->GetTextureUnit(), 0, cascId);
				shader->SetVariable("DirectionalLightTransforms", lightParam[0].Projection[cascId] * lightParam[0].View[cascId], cascId);
				shader->SetVariable("CascadeEndClipSpace", lightParam[0].Edge[cascId], cascId);
			}
		}
		++inputOffset;

		if (auto val = CheckInputDataType<Fbo_Handler*>(*m_inputs->at(inputOffset)))
		{
			shader->SetVariable("PointLightCount", lightParam[1].Count);
			shader->SetVariable("SpotLightCount", lightParam[2].Count);

			auto pointLightsCnt = lightParam[1].Count + lightParam[2].Count;

			for (auto omniLightIndex = 0; omniLightIndex < pointLightsCnt; ++omniLightIndex)
			{
				//shader->SetTextureUnitStructArr("omniShadowMaps", omniLightIndex, "shadowMap");
				shader.oneTimeWrite(&Shader_Object::SetTextureUnitStructArr, "omniShadowMaps", omniLightIndex, "shadowMap");
				val->AttachFBOToTextureUnit(omniLightIndex, shader->GetTextureUnit(), 0, 0);

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
			if (auto val = CheckInputDataType<Fbo_Handler*>(*m_inputs->at(inputIndex)))
			{
				//shader->SetTextureUnit(std::move(inputTexBuffs[inputIndex - inputOffset]));
				//ToDo: Why is causing this invoke error??? Is it deducing the wrong type (std::string&)?? Figure out why.....
				//shader.write(std::mem_fn(&Shader_Object::SetTextureUnit), std::move(inputTexBuffs[inputIndex - inputOffset]));
				
				//shader.write(std::mem_fn(&Shader_Object::SetTextureUnitRef), inputTexBuffs[inputIndex - inputOffset]);
				shader.oneTimeWrite(&Shader_Object::SetTextureUnitRef, inputTexBuffs[inputIndex - inputOffset]);

				val->AttachFBOToTextureUnit(0, shader->GetTextureUnit(), 0, 0);
			}
		}

		shader->ValidateShaderObject();

		for (auto roIndex = 0; roIndex < renderObj[shaderIndex].size(); ++roIndex)
		{
			renderObj[shaderIndex][roIndex].RenderObject(shader, std::move(RenderObjectParams{ true , true, &camParam->PrevProjView }));
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Scene_Render_Pass_Handler::~Scene_Render_Pass_Handler() = default;
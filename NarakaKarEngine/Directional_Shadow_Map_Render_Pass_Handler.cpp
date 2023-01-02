#include "pch.h"
#include "Directional_Shadow_Map_Render_Pass_Handler.h"
#include "Fbo_Handler.h"
#include "Shader_Object.h"
#include "Render_Object.h"
#include "RenderingCommonValues.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

Directional_Shadow_Map_Render_Pass_Handler::Directional_Shadow_Map_Render_Pass_Handler(std::shared_ptr<Fbo_Handler> fboHandlr
	, const std::vector<std::shared_ptr<Shader_Object>>& shaderVec
	, std::shared_ptr<std::vector<std::shared_ptr<std::any>>> inputs)
	: Render_Pass_Handler(fboHandlr, shaderVec, inputs)
{
}

void Directional_Shadow_Map_Render_Pass_Handler::Update(const std::vector<std::vector<std::shared_ptr<Render_Object>>>& renderObj, const CamParam* camParam, const LightParam* lightParam)
{
	m_fboHandler->BindFBO();

	for (size_t cascadeId = 0; cascadeId < NUM_CASCADES; ++cascadeId)
	{
		glViewport(0, 0, m_fboHandler->GetFBOWidth(), m_fboHandler->GetFBOHeight());
		m_fboHandler->WriteToFBOBuffer(0, 0, cascadeId, 0);
		glClear(GL_DEPTH_BUFFER_BIT);

		for (auto shaderIndex = 0; shaderIndex < m_shaderVec->size(); ++shaderIndex)
		{
			auto shader = m_shaderVec->at(shaderIndex);

			shader->ResetTextureUnit(0);
			shader->UseShaderObject();
			shader->SetVariable("DirectionalLightTransform", lightParam->Projection[cascadeId] * lightParam->View[cascadeId]);
			shader->ValidateShaderObject();

			for (auto roIndex = 0; roIndex < renderObj[shaderIndex].size(); ++roIndex)
			{
				auto ro = renderObj[shaderIndex][roIndex];
				if (ro->IsTesselated()) 
				{
					shader->SetVariable("eyePosition", camParam->Position); 
					ro->RenderObject(*shader, std::move(RenderObjectParams{ true, true}));
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

Directional_Shadow_Map_Render_Pass_Handler::~Directional_Shadow_Map_Render_Pass_Handler() = default;
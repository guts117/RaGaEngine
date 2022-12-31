#include "pch.h"
#include "PreZ_Render_Pass_Handler.h"
#include "Fbo_Handler.h"
#include "Shader_Object.h"
#include "Render_Object.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

PreZ_Render_Pass_Handler::PreZ_Render_Pass_Handler(std::shared_ptr<Fbo_Handler> fboHandlr
	, const std::vector<std::shared_ptr<Shader_Object>>& shaderVec
	, std::shared_ptr<std::vector<std::shared_ptr<std::any>>> inputs)
	: Scene_Render_Pass_Handler(fboHandlr, shaderVec, inputs)
{
}

void PreZ_Render_Pass_Handler::Update(const std::vector<std::vector<std::shared_ptr<Render_Object>>>& renderObj, const CamParam* camParam, const LightParam* lightParam)
{
	glViewport(0, 0, m_fboHandler->GetFBOWidth(), m_fboHandler->GetFBOHeight());

	m_fboHandler->BindFBO();
	//clear depth buffer
	glClear(GL_DEPTH_BUFFER_BIT);

	for(auto shaderIndex = 0; shaderIndex < m_shaderVec->size(); ++shaderIndex)
	{
		auto shader = m_shaderVec->at(shaderIndex);

		shader->ResetTextureUnit(0);
		shader->UseShaderObject();

		shader->SetVariable("Projection", camParam->Projection);
		shader->SetVariable("View", camParam->View);

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
		shader->ValidateShaderObject();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

PreZ_Render_Pass_Handler::~PreZ_Render_Pass_Handler() = default;
#include "render_pch.h"
#include "Bloom_Render_Pass_Handler.h"
#include "Fbo_Handler.h"
#include "Shader_Object.h"
#include "Render_Object.h"
#include "RenderingCommonValues.h"

using namespace NarakaRenderEngine;
using namespace RenderEngine;

Bloom_Render_Pass_Handler::Bloom_Render_Pass_Handler(Fbo_Handler* fboHandlr
	, std::vector<clustering_ptr<Shader_Object>>&& shaderVec
	, std::shared_ptr<std::vector<std::shared_ptr<std::any>>> inputs)
	: Render_Pass_Handler(fboHandlr, std::move(shaderVec), inputs)
{
}

void Bloom_Render_Pass_Handler::Update(std::vector<std::vector<Render_Object>>& renderObj, const CamParam* camParam, const LightParam* lightParam)
{
	int amount = 10;

	glViewport(0, 0, m_fboHandler->GetFBOWidth(), m_fboHandler->GetFBOHeight());

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (auto shaderIndex = 0; shaderIndex < m_shaderVec.size(); ++shaderIndex)
	{
		if (shaderIndex >= renderObj.size()) { break; }

		auto isHorizontalFbo = true;

		auto& shader = m_shaderVec[shaderIndex];
		shader->ResetTextureUnit(0);
		shader->UseShaderObject();

		if (auto val = CheckInputDataType<Fbo_Handler*>(*m_inputs->at(0)))
		{
			val->AttachFBOToTextureUnit(0, shader->SetTextureUnit("theTexture"), 0, 1);
		}

		for (int i = 0; i < amount; ++i)
		{
			m_fboHandler->BindFBO(isHorizontalFbo);
			shader->ResetTextureUnit(0);
			shader->SetVariable("horizontal", isHorizontalFbo);

			if (i > 0)	
			{
				m_fboHandler->AttachFBOToTextureUnit(!isHorizontalFbo, shader->SetTextureUnit("theTexture"), 0, 0);
			}

			shader->ValidateShaderObject();

			for (auto roIndex = 0; roIndex < renderObj[shaderIndex].size(); ++roIndex)
			{
				renderObj[shaderIndex][roIndex].RenderObject(shader, std::move(RenderObjectParams{ }));
			}

			isHorizontalFbo = !isHorizontalFbo;
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Bloom_Render_Pass_Handler::~Bloom_Render_Pass_Handler() = default;

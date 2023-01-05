#include "pch.h"
#include "Bloom_Render_Pass_Handler.h"
#include "Fbo_Handler.h"
#include "Shader_Object.h"
#include "Render_Object.h"
#include "RenderingCommonValues.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

Bloom_Render_Pass_Handler::Bloom_Render_Pass_Handler(std::shared_ptr<Fbo_Handler> fboHandlr
	, const std::vector<std::shared_ptr<Shader_Object>>& shaderVec
	, std::shared_ptr<std::vector<std::shared_ptr<std::any>>> inputs)
	: Render_Pass_Handler(fboHandlr, shaderVec, inputs)
{
}

void Bloom_Render_Pass_Handler::Update(const std::vector<std::vector<std::shared_ptr<Render_Object>>>& renderObj, const CamParam* camParam, const LightParam* lightParam)
{
	bool isHorizontalFbo = true;
	int amount = 10;

	for (auto shaderIndex = 0; shaderIndex < m_shaderVec->size(); ++shaderIndex)
	{
		auto shader = m_shaderVec->at(shaderIndex);
		shader->ResetTextureUnit(0);
		shader->UseShaderObject();

		if (auto val = CheckInputDataType<Fbo_Handler>(*m_inputs->at(0)))
		{
			val->AttachFBOToTextureUnit(0, shader->SetTextureUnit("theTexture"), 0, 1);
		}

		for (int i = 0; i < amount; i++)
		{
			m_fboHandler->BindFBO(isHorizontalFbo);
			shader->SetVariable("horizontal", isHorizontalFbo);
			shader->ValidateShaderObject();

			if (i > 0)
			{
				m_fboHandler->AttachFBOToTextureUnit(!isHorizontalFbo, GL_TEXTURE0, 0, 0);
			}

			for (auto roIndex = 0; roIndex < renderObj[shaderIndex].size(); ++roIndex)
			{
				renderObj[shaderIndex][roIndex]->RenderObject(*shader, std::move(RenderObjectParams{ }));
			}

			isHorizontalFbo = !isHorizontalFbo;
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Bloom_Render_Pass_Handler::~Bloom_Render_Pass_Handler() = default;

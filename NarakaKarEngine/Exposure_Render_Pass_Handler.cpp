#include "pch.h"
#include "Exposure_Render_Pass_Handler.h"
#include "Fbo_Handler.h"
#include "Shader_Object.h"
#include "Render_Object.h"
#include "RenderingCommonValues.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

Exposure_Render_Pass_Handler::Exposure_Render_Pass_Handler(std::shared_ptr<Fbo_Handler> fboHandlr
	, const std::vector<std::shared_ptr<Shader_Object>>& shaderVec
	, std::shared_ptr<std::vector<std::shared_ptr<std::any>>> inputs)
	: Render_Pass_Handler(fboHandlr, shaderVec, inputs)
{
}

void Exposure_Render_Pass_Handler::Update(const std::vector<std::vector<std::shared_ptr<Render_Object>>>& renderObj, const CamParam* camParam, const LightParam* lightParam)
{
	glViewport(0, 0, m_fboHandler->GetFBOWidth(), m_fboHandler->GetFBOHeight());

	m_fboHandler->BindFBO();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (auto shaderIndex = 0; shaderIndex < m_shaderVec->size(); ++shaderIndex)
	{
		if (shaderIndex >= renderObj.size()) { return; }

		auto& shader = m_shaderVec->at(shaderIndex);

		shader->ResetTextureUnit(0);
		shader->UseShaderObject();

		shader->SetVariable("hdr", 1);
		shader->SetVariable("exposure", 1.0f);

		if (auto val = CheckInputDataType<std::shared_ptr<Fbo_Handler>>(*m_inputs->at(0)))
		{
			val->get()->AttachFBOToTextureUnit(1, shader->SetTextureUnit("blurTexture"), 0, 0);
		}

		if (auto val = CheckInputDataType<std::shared_ptr<Fbo_Handler>>(*m_inputs->at(1)))
		{
			val->get()->AttachFBOToTextureUnit(0, shader->SetTextureUnit("theTexture"), 0, 0);
		}

		for (auto roIndex = 0; roIndex < renderObj[shaderIndex].size(); ++roIndex)
		{
			renderObj[shaderIndex][roIndex]->RenderObject(*shader, std::move(RenderObjectParams{ }));
		}
		shader->ValidateShaderObject();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Exposure_Render_Pass_Handler::~Exposure_Render_Pass_Handler() = default;

#include "render_pch.h"
#include "Ssao_Render_Pass_Handler.h"
#include "Fbo_Handler.h"
#include "Shader_Object.h"
#include "Render_Object.h"
#include "RenderingCommonValues.h"

using namespace NarakaRenderEngine;
using namespace RenderEngine;

Ssao_Render_Pass_Handler::Ssao_Render_Pass_Handler(Fbo_Handler* fboHandlr
	, std::vector<rw_clustering_ptr<Shader_Object>>&& shaderVec
	, std::shared_ptr<std::vector<std::shared_ptr<std::any>>> inputs)
	: Render_Pass_Handler(fboHandlr, std::move(shaderVec), inputs)
{
}

void Ssao_Render_Pass_Handler::Init()
{
	glm::vec3 kernel[KERNEL_SIZE];

	for (unsigned int i = 0; i < KERNEL_SIZE; i++) {
		float scale = (float)i / (float)(KERNEL_SIZE);
		glm::vec3 v;
		v.x = 2.0f * (float)rand() / RAND_MAX - 1.0f;
		v.y = 2.0f * (float)rand() / RAND_MAX - 1.0f;
		v.z = (float)rand() / RAND_MAX;
		// Use an acceleration function so more points are
		// located closer to the origin
		v *= (0.1f + 0.9f * scale * scale);

		kernel[i] = v;
	}

	for (auto shaderIndex = 0; shaderIndex < m_shaderVec.size(); ++shaderIndex)
	{
		auto shader = m_shaderVec[shaderIndex];

		shader->UseShaderObject();
		shader->SetVariable("Kernel", std::make_tuple(KERNEL_SIZE, kernel));
		shader->ValidateShaderObject();
	}
}

void Ssao_Render_Pass_Handler::Update(std::vector<std::vector<Render_Object>>& renderObj, const CamParam* camParam, const LightParam* lightParam)
{
	glViewport(0, 0, m_fboHandler->GetFBOWidth(), m_fboHandler->GetFBOHeight());

	m_fboHandler->BindFBO();
	//clear color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	for (auto shaderIndex = 0; shaderIndex < m_shaderVec.size(); ++shaderIndex)
	{
		if (shaderIndex >= renderObj.size()) { break; }

		auto& shader = m_shaderVec[shaderIndex];

		shader.write(std::mem_fn(&Shader_Object::ResetTextureUnit), 0);
		shader->UseShaderObject();

		if (auto val = CheckInputDataType<Fbo_Handler*>(*m_inputs->at(0)))
		{
			shader.write(std::mem_fn(&Shader_Object::SetTextureUnit), "theTexture");
			val->AttachFBOToTextureUnit(0, shader->GetTextureUnit(), 0, 0);
		}

		shader->SetVariable("SampleRad", 0.1f);
		shader->SetVariable("Projection", camParam->Projection);

		for (auto roIndex = 0; roIndex < renderObj[shaderIndex].size(); ++roIndex)
		{
			renderObj[shaderIndex][roIndex].RenderObject(shader, std::move(RenderObjectParams{ false, true}));
		}
		shader->ValidateShaderObject();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Ssao_Render_Pass_Handler::~Ssao_Render_Pass_Handler() = default;

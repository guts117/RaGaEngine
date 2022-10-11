#include "pch.h"
#include "Shading_Pass_Fbo_Handler.h"
#include "FrameBufferObject.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

struct Shading_Pass_Fbo_Handler::Impl
{
	std::unique_ptr<FrameBufferObject> m_Fbo;

	Impl() = delete;

	Impl(const GLuint& width, const GLuint& height)
	{
		auto colorFboTexParams = std::vector<FBOTexParams>();
		colorFboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MIN_FILTER,	GL_LINEAR });
		colorFboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MAG_FILTER,	GL_LINEAR });
		colorFboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_S,		GL_CLAMP_TO_EDGE });
		colorFboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_T,		GL_CLAMP_TO_EDGE });

		auto motionFboTexParams = std::vector<FBOTexParams>();
		motionFboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MIN_FILTER,	GL_NEAREST });
		motionFboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MAG_FILTER,	GL_NEAREST });

		FBOTexGenParams colorFboTexGenParams{ 2, GL_TEXTURE_2D, 0, GL_RGBA16F, 0, GL_RGBA, GL_FLOAT, NULL, colorFboTexParams };
		FBOTexGenParams motionfboTexGenParams{ 1, GL_TEXTURE_2D, 0, GL_RG16F, 0, GL_RG, GL_FLOAT, NULL, motionFboTexParams };
		FBORenderBufferParam fboRBParams{ GL_DEPTH_COMPONENT , GL_DEPTH_ATTACHMENT };
		auto fboParams = std::make_shared<FBOParams>(FBOParams{ false, width, height, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, std::vector<FBOTexGenParams>{colorFboTexGenParams, motionfboTexGenParams }, std::vector<FBORenderBufferParam>{ fboRBParams } });
		m_Fbo = std::make_unique<FrameBufferObject>(fboParams);
	}

	Impl(Impl&& rhs) noexcept = default;
	Impl& operator=(Impl&& rhs) noexcept = default;

	Impl(const Impl& rhs) noexcept = delete;
	Impl& operator=(const Impl& rhs) noexcept = delete;

	void BindFBO() const
	{
		m_Fbo->Bind();
	}

	void AttachFBOToTextureUnit(const GLenum& textureUnit, const GLuint& bufferIndex) const
	{
		auto texGenParamIndex = bufferIndex == ShadingPassBufferType::Motion ? 1 : 0;
		m_Fbo->AttachColorBufferToTexture(textureUnit, texGenParamIndex, bufferIndex);
	}

	void ResizeFBO(const GLuint& width, const GLuint& height)
	{
		m_Fbo->ResizeBuffers(width, height);
	}

	~Impl() = default;
};

Shading_Pass_Fbo_Handler::Shading_Pass_Fbo_Handler(const GLuint& width, const GLuint& height) : m_pImpl{ new Impl(width, height) } {}

void Shading_Pass_Fbo_Handler::BindFBO() const
{
	Pimpl()->BindFBO();
}

void Shading_Pass_Fbo_Handler::AttachFBOToTextureUnit(const GLenum& textureUnit, const GLuint& bufferIndex) const
{
	Pimpl()->AttachFBOToTextureUnit(textureUnit, bufferIndex);
}

void Shading_Pass_Fbo_Handler::ResizeFBO(const GLuint& width, const GLuint& height)
{
	Pimpl()->ResizeFBO(width, height);
}

const GLuint& Shading_Pass_Fbo_Handler::GetFBOWidth() const
{
	return Pimpl()->m_Fbo->GetWidth();
}

const GLuint& Shading_Pass_Fbo_Handler::GetFBOHeight() const
{
	return Pimpl()->m_Fbo->GetHeight();
}

const GLuint& Shading_Pass_Fbo_Handler::GetFBOBuffer(const GLuint& bufferIndex) const
{
	return Pimpl()->m_Fbo->GetBuffer(bufferIndex);
}

Shading_Pass_Fbo_Handler::~Shading_Pass_Fbo_Handler() = default;
#include "pch.h"
#include "Ssao_Pass_Fbo_Handler.h"
#include "FrameBufferObject.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

struct Ssao_Pass_Fbo_Handler::Impl
{
	std::unique_ptr<FrameBufferObject> m_Fbo;

	Impl() = delete;

	Impl(const GLuint& width, const GLuint& height)
	{
		auto fboTexParams = std::vector<FBOTexParams>();
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MIN_FILTER,		GL_LINEAR });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MAG_FILTER,		GL_LINEAR });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_S,			GL_CLAMP_TO_EDGE });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_T,			GL_CLAMP_TO_EDGE });

		FBOTexGenParams fboTexGenParams{ 1, GL_TEXTURE_2D, 0, GL_R32F, 0, GL_RED, GL_FLOAT, NULL, fboTexParams };
		auto fboParams = std::make_shared<FBOParams>(FBOParams{ false, width, height, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, std::vector<FBOTexGenParams>{fboTexGenParams} });
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

	void AttachFBOToTextureUnit(const GLenum& textureUnit) const
	{
		m_Fbo->AttachColorBufferToTexture(textureUnit, 0, 0);
	}

	void ResizeFBO(const GLuint& width, const GLuint& height)
	{
		m_Fbo->ResizeBuffers(width, height);
	}

	~Impl() = default;
};

Ssao_Pass_Fbo_Handler::Ssao_Pass_Fbo_Handler(const GLuint& width, const GLuint& height) : m_pImpl{ new Impl(width, height) } {}

void Ssao_Pass_Fbo_Handler::BindFBO() const
{
	Pimpl()->BindFBO();
}

void Ssao_Pass_Fbo_Handler::AttachFBOToTextureUnit(const GLenum& textureUnit) const
{
	Pimpl()->AttachFBOToTextureUnit(textureUnit);
}

void Ssao_Pass_Fbo_Handler::ResizeFBO(const GLuint& width, const GLuint& height)
{
	Pimpl()->ResizeFBO(width, height);
}

const GLuint& Ssao_Pass_Fbo_Handler::GetFBOWidth() const
{
	return Pimpl()->m_Fbo->GetWidth();
}

const GLuint& Ssao_Pass_Fbo_Handler::GetFBOHeight() const
{
	return Pimpl()->m_Fbo->GetHeight();
}

const GLuint& Ssao_Pass_Fbo_Handler::GetFBOBuffer(const GLuint& bufferIndex) const
{
	return Pimpl()->m_Fbo->GetBuffer(bufferIndex);
}

Ssao_Pass_Fbo_Handler::~Ssao_Pass_Fbo_Handler() = default;
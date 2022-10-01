#include "pch.h"
#include "Depth_Pass_Fbo_Handler.h"
#include "FrameBufferObject.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

struct Depth_Pass_Fbo_Handler::Impl
{
	std::unique_ptr<FrameBufferObject> m_FboVec;

	Impl() = delete;

	Impl(const GLuint& width, const GLuint& height)
	{
		for (unsigned int i = 0; i < 2; ++i)
		{
			auto fboTexParams = std::vector<FBOTexParams>();
			fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MIN_FILTER,		GL_LINEAR });
			fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MAG_FILTER,		GL_LINEAR });
			fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_S,			GL_CLAMP_TO_EDGE });
			fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_T,			GL_CLAMP_TO_EDGE });
			fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_COMPARE_MODE,	GL_NONE });

			FBOTexGenParams fboTexGenParams{ 1, GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL, fboTexParams };
			auto fboParams = std::make_shared<FBOParams>(FBOParams{ width, height, GL_DEPTH_ATTACHMENT, std::vector<FBOTexGenParams>{fboTexGenParams}});
			m_FboVec = std::make_unique<FrameBufferObject>(fboParams);
		}
	}

	Impl(Impl&& rhs) noexcept = default;
	Impl& operator=(Impl&& rhs) noexcept = default;

	Impl(const Impl& rhs) noexcept = delete;
	Impl& operator=(const Impl& rhs) noexcept = delete;

	void WriteToFBO() const
	{
		m_FboVec->WriteToBuffer();
	}

	void AttachFBOToTextureUnit(const GLenum& textureUnit) const
	{
		m_FboVec->AttachColorBufferToTexture(textureUnit, 0, 0);
	}

	void ResizeFBO(const GLuint& width, const GLuint& height)
	{
		m_FboVec->ResizeBuffers(width, height);
	}

	~Impl() = default;
};

Depth_Pass_Fbo_Handler::Depth_Pass_Fbo_Handler(const GLuint& width, const GLuint& height) : m_pImpl{ new Impl(width, height) } {}

void Depth_Pass_Fbo_Handler::WriteToFBO() const
{
	Pimpl()->WriteToFBO();
}

void Depth_Pass_Fbo_Handler::AttachFBOToTextureUnit(const GLenum& textureUnit) const
{
	Pimpl()->AttachFBOToTextureUnit(textureUnit);
}

void Depth_Pass_Fbo_Handler::ResizeFBO(const GLuint& width, const GLuint& height)
{
	Pimpl()->ResizeFBO(width, height);
}

const GLuint& Depth_Pass_Fbo_Handler::GetFBOWidth() const
{
	return Pimpl()->m_FboVec->GetWidth();
}

const GLuint& Depth_Pass_Fbo_Handler::GetFBOHeight() const
{
	return Pimpl()->m_FboVec->GetHeight();
}

Depth_Pass_Fbo_Handler::~Depth_Pass_Fbo_Handler() = default;
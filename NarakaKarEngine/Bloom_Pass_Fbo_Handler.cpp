#include "pch.h"
#include "Bloom_Pass_Fbo_Handler.h"
#include "FrameBufferObject.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

struct Bloom_Pass_Fbo_Handler::Impl
{
	std::unique_ptr<std::vector<std::shared_ptr<FrameBufferObject>>> m_FboVec;

	Impl() = delete;

	Impl(const GLuint& width, const GLuint& height)
		: m_FboVec{ std::make_unique<std::vector<std::shared_ptr<FrameBufferObject>>>() }
	{
		for (unsigned int i = 0; i < BloomFBOType::Max; ++i)
		{
			auto fboTexParams = std::vector<FBOTexParams>();
			fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MIN_FILTER,	GL_LINEAR });
			fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MAG_FILTER,	GL_LINEAR });
			fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_S,		GL_CLAMP_TO_EDGE });
			fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_T,		GL_CLAMP_TO_EDGE });

			FBOTexGenParams fboTexGenParams{ 1, GL_TEXTURE_2D, 0, GL_RGB16F, 0, GL_RGB, GL_FLOAT, NULL, fboTexParams };
			auto fboParams = std::make_shared<FBOParams>(FBOParams{ width, height, GL_COLOR_ATTACHMENT0, std::vector<FBOTexGenParams>{fboTexGenParams} });
			m_FboVec->push_back(std::make_shared<FrameBufferObject>(fboParams));
		}
	}

	Impl(Impl&& rhs) noexcept = default;
	Impl& operator=(Impl&& rhs) noexcept = default;

	Impl(const Impl& rhs) noexcept = delete;
	Impl& operator=(const Impl& rhs) noexcept = delete;

	void Bind(const GLuint& fboIndex) const
	{
		m_FboVec->at(fboIndex)->Bind();
	}

	void AttachFBOToTextureUnit(const GLenum& textureUnit, const GLuint& fboIndex) const
	{
		m_FboVec->at(fboIndex)->AttachColorBufferToTexture(textureUnit, 0, 0);
	}

	void ResizeFBO(const GLuint& width, const GLuint& height)
	{
		for (auto i = 0; i < m_FboVec->size(); ++i) 
		{
			m_FboVec->at(i)->ResizeBuffers(width, height);
		}
	}

	~Impl() = default;
};

Bloom_Pass_Fbo_Handler::Bloom_Pass_Fbo_Handler(const GLuint& width, const GLuint& height) : m_pImpl{ new Impl(width, height) } {}

void Bloom_Pass_Fbo_Handler::BindFBO(const GLuint& fboIndex) const
{
	Pimpl()->Bind(fboIndex);
}

void Bloom_Pass_Fbo_Handler::AttachFBOToTextureUnit(const GLenum& textureUnit, const GLuint& fboIndex) const
{
	Pimpl()->AttachFBOToTextureUnit(textureUnit, fboIndex);
}

void Bloom_Pass_Fbo_Handler::ResizeFBO(const GLuint& width, const GLuint& height)
{
	Pimpl()->ResizeFBO(width, height);
}

const GLuint& Bloom_Pass_Fbo_Handler::GetFBOWidth(const GLuint& fboIndex) const
{
	return Pimpl()->m_FboVec->at(fboIndex)->GetWidth();
}

const GLuint& Bloom_Pass_Fbo_Handler::GetFBOHeight(const GLuint& fboIndex) const
{
	return Pimpl()->m_FboVec->at(fboIndex)->GetHeight();
}

const GLuint& Bloom_Pass_Fbo_Handler::GetFBOBuffer(const GLuint& fboIndex) const
{
	return Pimpl()->m_FboVec->at(fboIndex)->GetBuffer(0);
}

Bloom_Pass_Fbo_Handler::~Bloom_Pass_Fbo_Handler() = default;

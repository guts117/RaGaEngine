#include "pch.h"
#include "BRDF_Pass_FBO_Handler.h"
#include "FrameBufferObject.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

struct BRDF_Pass_FBO_Handler::Impl
{
	std::unique_ptr<FBOParams> m_FboParams;
	std::unique_ptr<FrameBufferObject> m_Fbo;

	Impl() = delete;

	Impl(const GLuint& width, const GLuint& height)
	{
		auto fboTexParams = std::vector<FBOTexParams>();
		fboTexParams.push_back(FBOTexParams{ static_cast<GLenum>(GL_TEXTURE_MIN_FILTER),	static_cast<GLint>(GL_LINEAR) });
		fboTexParams.push_back(FBOTexParams{ static_cast<GLenum>(GL_TEXTURE_MAG_FILTER),	static_cast<GLint>(GL_LINEAR) });
		fboTexParams.push_back(FBOTexParams{ static_cast<GLenum>(GL_TEXTURE_WRAP_S),		static_cast<GLint>(GL_CLAMP_TO_EDGE) });
		fboTexParams.push_back(FBOTexParams{ static_cast<GLenum>(GL_TEXTURE_WRAP_T),		static_cast<GLint>(GL_CLAMP_TO_EDGE) });

		FBOTexGenParams fboTexGenParams{ 1, GL_TEXTURE_2D, 0, GL_RG16F, 0, GL_RG, GL_FLOAT, NULL, fboTexParams };

		m_FboParams = std::make_unique<FBOParams>(FBOParams{ width, height, std::vector<FBOTexGenParams>{fboTexGenParams}, 0, 0, 0 });
		m_Fbo = std::make_unique<FrameBufferObject>(m_FboParams);
	}

	Impl(Impl&& rhs) noexcept = default;
	Impl& operator=(Impl&& rhs) noexcept = default;

	Impl(const Impl& rhs) noexcept = delete;
	Impl& operator=(const Impl& rhs) noexcept = delete;

	void WriteToFBO() const
	{
		m_Fbo->WriteToBuffer();
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

BRDF_Pass_FBO_Handler::BRDF_Pass_FBO_Handler(const GLuint& width, const GLuint& height) : m_pImpl{ new Impl(width, height) } {}

void BRDF_Pass_FBO_Handler::WriteToFBO() const
{
	Pimpl()->WriteToFBO();
}

void BRDF_Pass_FBO_Handler::AttachFBOToTextureUnit(const GLenum& textureUnit) const
{
	Pimpl()->AttachFBOToTextureUnit(textureUnit);
}

void BRDF_Pass_FBO_Handler::ResizeFBO(const GLuint& width, const GLuint& height)
{
	Pimpl()->ResizeFBO(width, height);
}

const GLuint& BRDF_Pass_FBO_Handler::GetFBOWidth() const
{
	return Pimpl()->m_FboParams->Width;
}

const GLuint& BRDF_Pass_FBO_Handler::GetFBOHeight() const
{
	return Pimpl()->m_FboParams->Height;
}

BRDF_Pass_FBO_Handler::~BRDF_Pass_FBO_Handler() = default;

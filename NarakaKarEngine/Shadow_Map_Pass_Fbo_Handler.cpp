#include "pch.h"
#include "Shadow_Map_Pass_Fbo_Handler.h"
#include "FrameBufferObject.h"
#include "RenderingCommonValues.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

struct Shadow_Map_Pass_Fbo_Handler::Impl
{
	std::unique_ptr<FrameBufferObject> m_Fbo;

	Impl() = delete;

	Impl(const GLuint& width, const GLuint& height)
	{
		auto fboTexParams = std::vector<FBOTexParams>();
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MIN_FILTER,		GL_NEAREST });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MAG_FILTER,		GL_NEAREST });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_COMPARE_MODE,	GL_NONE });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_S,			GL_CLAMP_TO_BORDER });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_T,			GL_CLAMP_TO_BORDER });
		float bColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_BORDER_COLOR,	bColor });

		FBOTexGenParams fboTexGenParams{ NUM_CASCADES, GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL, fboTexParams };
		auto fboParams = std::make_shared<FBOParams>(FBOParams{ false, width, height, GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, std::vector<FBOTexGenParams>{fboTexGenParams} });
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

	void WriteToFBO(const GLuint& cascadeIndex) const
	{
		m_Fbo->WriteToBuffer(0, cascadeIndex);
	}

	void AttachFBOToTextureUnit(const GLenum& textureUnit, const GLenum& bufferIndex) const
	{
		m_Fbo->AttachColorBufferToTexture(textureUnit, 0, bufferIndex);
	}

	void CreateFBOMipMap() const
	{
		m_Fbo->CreateMipMap(0, 0);
	}

	void ResizeFBO(const GLuint& width, const GLuint& height)
	{
		m_Fbo->ResizeBuffers(width, height);
	}

	~Impl() = default;
};

Shadow_Map_Pass_Fbo_Handler::Shadow_Map_Pass_Fbo_Handler(const GLuint& width, const GLuint& height) : m_pImpl{ new Impl(width, height) } {}

void Shadow_Map_Pass_Fbo_Handler::BindFBO() const
{
	Pimpl()->BindFBO();
}

void Shadow_Map_Pass_Fbo_Handler::WriteToFBOBuffer(const GLuint& cascadeIndex) const
{
	Pimpl()->WriteToFBO(cascadeIndex);
}

void Shadow_Map_Pass_Fbo_Handler::AttachFBOToTextureUnit(const GLenum& textureUnit, const GLenum& bufferIndex) const
{
	Pimpl()->AttachFBOToTextureUnit(textureUnit, bufferIndex);
}

void Shadow_Map_Pass_Fbo_Handler::CreateFBOMipMap() const
{
	Pimpl()->CreateFBOMipMap();
}

void Shadow_Map_Pass_Fbo_Handler::ResizeFBO(const GLuint& width, const GLuint& height)
{
	Pimpl()->ResizeFBO(width, height);
}

const GLuint& Shadow_Map_Pass_Fbo_Handler::GetFBOWidth() const
{
	return Pimpl()->m_Fbo->GetWidth();
}

const GLuint& Shadow_Map_Pass_Fbo_Handler::GetFBOHeight() const
{
	return Pimpl()->m_Fbo->GetHeight();
}

const GLuint& Shadow_Map_Pass_Fbo_Handler::GetFBOBuffer(const GLuint& bufferIndex) const
{
	return Pimpl()->m_Fbo->GetBuffer(bufferIndex);
}

Shadow_Map_Pass_Fbo_Handler::~Shadow_Map_Pass_Fbo_Handler() = default;
#include "pch.h"
#include "Equirect_To_Cubemap_Pass_Fbo_Handler.h"
#include "FrameBufferObject.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

struct Equirect_To_Cubemap_Pass_Fbo_Handler::Impl
{
	std::unique_ptr<FrameBufferObject> m_Fbo;

	Impl() = delete;

	Impl(const GLuint& width, const GLuint& height, const bool& hasMipMap)
	{
		auto fboTexParams = std::vector<FBOTexParams>();

		if (hasMipMap) 
		{
			fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MIN_FILTER,		GL_LINEAR_MIPMAP_LINEAR });
		}
		else
		{
			fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MIN_FILTER,		GL_LINEAR });
		}
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MAG_FILTER,		GL_LINEAR });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_S,			GL_CLAMP_TO_EDGE });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_T,			GL_CLAMP_TO_EDGE });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_R,			GL_CLAMP_TO_EDGE });

		FBOTexGenParams fboTexGenParams{ 1, GL_TEXTURE_CUBE_MAP, 0, GL_RGB16F, 0, GL_RGB, GL_FLOAT, NULL, fboTexParams };
		FBORenderBufferParam fboRBParams{ GL_DEPTH_COMPONENT24 , GL_DEPTH_ATTACHMENT };
		auto fboParams = std::make_shared<FBOParams>(FBOParams{ width, height, GL_COLOR_ATTACHMENT0, std::vector<FBOTexGenParams>{fboTexGenParams}, std::vector<FBORenderBufferParam>{ fboRBParams } });
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

	void WriteToFBO(const GLuint& faceId) const
	{
		m_Fbo->WriteToBuffer(0, 0, faceId);
	}

	void AttachFBOToTextureUnit(const GLenum& textureUnit) const
	{
		m_Fbo->AttachColorBufferToTexture(textureUnit, 0, 0);
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

Equirect_To_Cubemap_Pass_Fbo_Handler::Equirect_To_Cubemap_Pass_Fbo_Handler(const GLuint& width, const GLuint& height, const bool& hasMipMap) : m_pImpl{ new Impl(width, height, hasMipMap) } {}

void Equirect_To_Cubemap_Pass_Fbo_Handler::BindFBO() const
{
	Pimpl()->BindFBO();
}

void Equirect_To_Cubemap_Pass_Fbo_Handler::WriteToFBOBuffer(const GLuint& faceId) const
{
	Pimpl()->WriteToFBO(faceId);
}

void Equirect_To_Cubemap_Pass_Fbo_Handler::AttachFBOToTextureUnit(const GLenum& textureUnit) const
{
	Pimpl()->AttachFBOToTextureUnit(textureUnit);
}

void Equirect_To_Cubemap_Pass_Fbo_Handler::CreateFBOMipMap() const 
{
	Pimpl()->CreateFBOMipMap();
}

void Equirect_To_Cubemap_Pass_Fbo_Handler::ResizeFBO(const GLuint& width, const GLuint& height)
{
	Pimpl()->ResizeFBO(width, height);
}

const GLuint& Equirect_To_Cubemap_Pass_Fbo_Handler::GetFBOWidth() const
{
	return Pimpl()->m_Fbo->GetWidth();
}

const GLuint& Equirect_To_Cubemap_Pass_Fbo_Handler::GetFBOHeight() const
{
	return Pimpl()->m_Fbo->GetHeight();
}

const GLuint& Equirect_To_Cubemap_Pass_Fbo_Handler::GetFBOBuffer(const GLuint& bufferIndex) const
{
	return Pimpl()->m_Fbo->GetBuffer(bufferIndex);
}

Equirect_To_Cubemap_Pass_Fbo_Handler::~Equirect_To_Cubemap_Pass_Fbo_Handler() = default;
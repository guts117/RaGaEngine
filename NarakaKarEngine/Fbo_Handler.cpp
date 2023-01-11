#include "pch.h"
#include "Fbo_Handler.h"
#include "FrameBufferObject.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

struct Fbo_Handler::Impl
{
	std::unique_ptr<std::vector<std::shared_ptr<FrameBufferObject>>> m_FboVec;
	std::string m_HandlerName;
	bool m_isWindowSized;
	GLuint m_Width;
	GLuint m_Height;
	Fbo_Handler* m_PrevHandler;
	Fbo_Handler* m_NextHandler;

	Impl() = delete;

	Impl(std::unique_ptr<std::vector<std::shared_ptr<FrameBufferObject>>>&& fboVectorPtr
		, const std::string& handlerName
		, const bool& isWindowSized
		, const GLuint& width
		, const GLuint& height
		, Fbo_Handler* prevHandler
		, Fbo_Handler* nextHandler)
		: m_FboVec{std::move(fboVectorPtr)}
		, m_HandlerName{ handlerName }
		, m_isWindowSized { isWindowSized }
		, m_Width { width }
		, m_Height{ height }
		, m_PrevHandler{ prevHandler }
		, m_NextHandler{ nextHandler }
	{
	}

	Impl(Impl&& rhs) noexcept = default;
	Impl& operator=(Impl&& rhs) noexcept = default;

	Impl(const Impl& rhs) noexcept = delete;
	Impl& operator=(const Impl& rhs) noexcept = delete;

	void Bind(const GLuint& fboIndex) const
	{
		m_FboVec->at(fboIndex)->Bind();
	}

	void WriteToFBO(const GLuint& fboIndex, const GLuint& texGenParamIndex, const GLuint& bufferIndex, const GLuint& faceId, const GLuint& mipLevel) const
	{
		m_FboVec->at(fboIndex)->WriteToBuffer(texGenParamIndex, bufferIndex, faceId, mipLevel);
	}
	void AttachFBOToTextureUnit(const GLuint& fboIndex, const GLuint& textureUnit, const GLuint& texGenParamIndex, const GLuint& bufferIndex) const
	{
		m_FboVec->at(fboIndex)->AttachColorBufferToTexture(GL_TEXTURE0 + textureUnit, texGenParamIndex, bufferIndex);
	}

	void CreateFBOMipMap(const GLuint& fboIndex, const GLuint& texGenParamIndex, const GLuint& bufferIndex) const
	{
		m_FboVec->at(fboIndex)->CreateMipMap(texGenParamIndex, bufferIndex);
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

Fbo_Handler::Fbo_Handler(std::unique_ptr<std::vector<std::shared_ptr<FrameBufferObject>>>&& fboVectorPtr
	, const std::string& handlerName
	, const bool& isWindowSized
	, const GLuint& width
	, const GLuint& height
	, Fbo_Handler* prevHandler
	, Fbo_Handler* nextHandler)
	: m_pImpl{ new Impl(std::move(fboVectorPtr), handlerName, isWindowSized, width, height, prevHandler, nextHandler) }
{
}

const std::string& Fbo_Handler::GetHandlerName() const
{
	return Pimpl()->m_HandlerName;
}

bool Fbo_Handler::CanResizeWithWindow() const
{
	return Pimpl()->m_isWindowSized;
}

void Fbo_Handler::BindFBO(const GLuint& fboIndex) const
{
	Pimpl()->Bind(fboIndex);
}

void Fbo_Handler::WriteToFBOBuffer(const GLuint& fboIndex, const GLuint& texGenParamIndex, const GLuint& bufferIndex, const GLuint& faceId, const GLuint& mipLevel) const
{
	Pimpl()->WriteToFBO(fboIndex, texGenParamIndex, bufferIndex, faceId, mipLevel);
}

void Fbo_Handler::AttachFBOToTextureUnit(const GLuint& fboIndex, const GLuint& textureUnit, const GLuint& texGenParamIndex, const GLuint& bufferIndex) const
{
	Pimpl()->AttachFBOToTextureUnit(fboIndex, textureUnit, texGenParamIndex, bufferIndex);
}

void Fbo_Handler::CreateFBOMipMap(const GLuint& fboIndex, const GLuint& texGenParamIndex, const GLuint& bufferIndex) const
{
	Pimpl()->CreateFBOMipMap(fboIndex, texGenParamIndex, bufferIndex);
}

void Fbo_Handler::ResizeFBO(const GLuint& width, const GLuint& height)
{
	Pimpl()->ResizeFBO(width, height);
}

void Fbo_Handler::Blit(const GLuint& fboIndex, const Fbo_Handler& toFboHandlr, const GLuint& toFboIndex) const
{
	Pimpl()->m_FboVec->at(fboIndex)->Blit(toFboHandlr.GetFBOId(toFboIndex));
}

const GLuint& Fbo_Handler::GetFBOWidth(const GLuint& fboIndex) const
{
	return Pimpl()->m_FboVec->at(fboIndex)->GetWidth();
}

const GLuint& Fbo_Handler::GetFBOHeight(const GLuint& fboIndex) const
{
	return Pimpl()->m_FboVec->at(fboIndex)->GetHeight();
}

const GLuint& Fbo_Handler::GetFBOBuffer(const GLuint& fboIndex, const GLuint& bufferIndex) const
{
	return Pimpl()->m_FboVec->at(fboIndex)->GetBuffer(bufferIndex);
}

const GLuint& NarakaKarEngine::RenderEngine::Fbo_Handler::GetFBOId(const GLuint& fboIndex) const
{
	return Pimpl()->m_FboVec->at(fboIndex)->GetFboId();
}

Fbo_Handler::~Fbo_Handler() = default;

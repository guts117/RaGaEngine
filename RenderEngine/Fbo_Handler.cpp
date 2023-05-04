#include "render_pch.h"
#include "Fbo_Handler.h"
#include "FrameBufferObject.h"

using namespace NarakaRenderEngine;
using namespace RenderEngine;

struct alignas(alignof(std::string)) Fbo_Handler::Impl
{
	std::string m_HandlerName;
	std::vector<FrameBufferObject> m_FboVec;
	Fbo_Handler* m_PrevHandler;
	Fbo_Handler* m_NextHandler;
	GLuint m_Width;
	GLuint m_Height;
	bool m_isWindowSized;

	Impl() = delete;

	Impl(std::vector<FrameBufferObject>&& fboVector
		, const std::string& handlerName
		, const bool& isWindowSized
		, const GLuint& width
		, const GLuint& height
		, Fbo_Handler* prevHandler
		, Fbo_Handler* nextHandler)
		: m_HandlerName{ handlerName }	
		, m_FboVec{ std::move(fboVector) }
		, m_PrevHandler{ prevHandler }
		, m_NextHandler{ nextHandler }
		, m_Width{ width }
		, m_Height{ height }
		, m_isWindowSized{ isWindowSized }
	{
	}

	Impl(Impl&& rhs) noexcept 
		: m_HandlerName{std::move(rhs.m_HandlerName)}
		, m_FboVec{std::move(rhs.m_FboVec)}
		, m_PrevHandler{std::exchange(rhs.m_PrevHandler, nullptr)}
		, m_NextHandler{std::exchange(rhs.m_NextHandler, nullptr)}
		, m_Width{std::exchange(rhs.m_Width, 0)}
		, m_Height{std::exchange(rhs.m_Height, 0)}
		, m_isWindowSized{std::exchange(rhs.m_isWindowSized, false)}
	{
	};
	Impl& operator=(Impl&& rhs) noexcept 
	{
		m_HandlerName = std::move(rhs.m_HandlerName);
		m_FboVec = std::move(rhs.m_FboVec);
		m_PrevHandler = std::exchange(rhs.m_PrevHandler, nullptr);
		m_NextHandler = std::exchange(rhs.m_NextHandler, nullptr);
		m_Width = std::exchange(rhs.m_Width, 0);
		m_Height = std::exchange(rhs.m_Height, 0);
		m_isWindowSized = std::exchange(rhs.m_isWindowSized, false);
		return *this;
	};

	Impl(const Impl& rhs) noexcept = delete;
	Impl& operator=(const Impl& rhs) noexcept = delete;

	const std::string& GetHandlerName() const
	{
		return m_HandlerName;
	}

	bool CanResizeWithWindow() const
	{
		return m_isWindowSized;
	}

	void Bind(const GLuint& fboIndex) const
	{
		m_FboVec.at(fboIndex).Bind();
	}

	void WriteToFBO(const GLuint& fboIndex, const GLuint& texGenParamIndex, const GLuint& bufferIndex, const GLuint& faceId, const GLuint& mipLevel) const
	{
		m_FboVec.at(fboIndex).WriteToBuffer(texGenParamIndex, bufferIndex, faceId, mipLevel);
	}
	void AttachFBOToTextureUnit(const GLuint& fboIndex, const GLuint& textureUnit, const GLuint& texGenParamIndex, const GLuint& bufferIndex) const
	{
		m_FboVec.at(fboIndex).AttachColorBufferToTexture(GL_TEXTURE0 + textureUnit, texGenParamIndex, bufferIndex);
	}

	void CreateFBOMipMap(const GLuint& fboIndex, const GLuint& texGenParamIndex, const GLuint& bufferIndex) const
	{
		m_FboVec.at(fboIndex).CreateMipMap(texGenParamIndex, bufferIndex);
	}

	void ResizeFBO(const GLuint& width, const GLuint& height)
	{
		for (auto i = 0; i < m_FboVec.size(); ++i)
		{
			m_FboVec.at(i).ResizeBuffers(width, height);
		}
	}

	void Blit(const GLuint& fboIndex, const Fbo_Handler* toFboHandlr, const GLuint& toFboIndex) const
	{
		m_FboVec.at(fboIndex).Blit(toFboHandlr->GetFBOId(toFboIndex));
	}

	const GLuint& GetFBOWidth(const GLuint& fboIndex) const
	{
		return m_FboVec.at(fboIndex).GetWidth();
	}

	const GLuint& GetFBOHeight(const GLuint& fboIndex) const
	{
		return m_FboVec.at(fboIndex).GetHeight();
	}

	const GLuint& GetFBOBuffer(const GLuint& fboIndex, const GLuint& bufferIndex) const
	{
		return m_FboVec.at(fboIndex).GetBuffer(bufferIndex);
	}

	const GLuint& GetFBOId(const GLuint& fboIndex) const
	{
		return m_FboVec.at(fboIndex).GetFboId();
	}

	~Impl() noexcept = default;
};

Fbo_Handler::Fbo_Handler(std::vector<FrameBufferObject>&& fboVectorPtr
	, const std::string& handlerName
	, const bool& isWindowSized
	, const GLuint& width
	, const GLuint& height
	, Fbo_Handler* prevHandler
	, Fbo_Handler* nextHandler)
	: m_pImpl{ Impl(std::move(fboVectorPtr), handlerName, isWindowSized, width, height, prevHandler, nextHandler) }
{
}

Fbo_Handler::Fbo_Handler(Fbo_Handler&& rhs) noexcept = default;
Fbo_Handler& Fbo_Handler::operator=(Fbo_Handler&& rhs) noexcept = default;

const std::string& Fbo_Handler::GetHandlerName() const
{
	return Pimpl().GetHandlerName();
}

bool Fbo_Handler::CanResizeWithWindow() const
{
	return Pimpl().CanResizeWithWindow();
}

void Fbo_Handler::BindFBO(const GLuint& fboIndex) const
{
	Pimpl().Bind(fboIndex);
}

void Fbo_Handler::WriteToFBOBuffer(const GLuint& fboIndex, const GLuint& texGenParamIndex, const GLuint& bufferIndex, const GLuint& faceId, const GLuint& mipLevel) const
{
	Pimpl().WriteToFBO(fboIndex, texGenParamIndex, bufferIndex, faceId, mipLevel);
}

void Fbo_Handler::AttachFBOToTextureUnit(const GLuint& fboIndex, const GLuint& textureUnit, const GLuint& texGenParamIndex, const GLuint& bufferIndex) const
{
	Pimpl().AttachFBOToTextureUnit(fboIndex, textureUnit, texGenParamIndex, bufferIndex);
}

void Fbo_Handler::CreateFBOMipMap(const GLuint& fboIndex, const GLuint& texGenParamIndex, const GLuint& bufferIndex) const
{
	Pimpl().CreateFBOMipMap(fboIndex, texGenParamIndex, bufferIndex);
}

void Fbo_Handler::ResizeFBO(const GLuint& width, const GLuint& height)
{
	Pimpl().ResizeFBO(width, height);
}

void Fbo_Handler::Blit(const GLuint& fboIndex, const Fbo_Handler* toFboHandlr, const GLuint& toFboIndex) const
{
	Pimpl().Blit(fboIndex, toFboHandlr, toFboIndex);
}

const GLuint& Fbo_Handler::GetFBOWidth(const GLuint& fboIndex) const
{
	return Pimpl().GetFBOWidth(fboIndex);
}

const GLuint& Fbo_Handler::GetFBOHeight(const GLuint& fboIndex) const
{
	return Pimpl().GetFBOHeight(fboIndex);
}

const GLuint& Fbo_Handler::GetFBOBuffer(const GLuint& fboIndex, const GLuint& bufferIndex) const
{
	return Pimpl().GetFBOBuffer(fboIndex, bufferIndex);
}

const GLuint& Fbo_Handler::GetFBOId(const GLuint& fboIndex) const
{
	return Pimpl().GetFBOId(fboIndex);
}

Fbo_Handler::~Fbo_Handler() noexcept = default;
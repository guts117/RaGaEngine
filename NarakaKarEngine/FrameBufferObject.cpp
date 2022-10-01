#include "pch.h"
#include "FrameBufferObject.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

struct FrameBufferObject::Impl
{
	GLuint m_FboId;
	std::shared_ptr<FBOParams> m_FboParam;
	std::vector<GLuint>m_ColorBuffers;
	GLuint m_DepthBuffer;
	GLuint m_StencilBuffer;
	GLuint m_DepthStencilBuffer;

	Impl() = delete;

	Impl(const std::shared_ptr<FBOParams>& fboParams)
		: m_FboId{ 0 }
		, m_FboParam{ fboParams }
		, m_ColorBuffers{ std::vector<GLuint>() }
		, m_DepthBuffer{ fboParams->DepthBufferFormat }
		, m_StencilBuffer{ fboParams->StencilBufferFormat }
		, m_DepthStencilBuffer{ fboParams->DepthStencilBufferFormat }
	{
		glGenFramebuffers(1, &m_FboId);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FboId);

		for (auto fboTexParamIndex = 0; fboTexParamIndex < m_FboParam->FboTexGenParams.size(); ++fboTexParamIndex)
		{
			auto texCount = m_FboParam->FboTexGenParams[fboTexParamIndex].ColorBufferSize;
			for (auto i = 0; i < texCount; ++i)
			{
				GLuint tempColorBuf = 0;
				glGenTextures(1, &tempColorBuf);
				m_ColorBuffers.push_back(tempColorBuf);

				FBOTexGenParams& bufferParams = m_FboParam->FboTexGenParams[fboTexParamIndex];
				glBindTexture(bufferParams.Target, m_ColorBuffers[i]);
				glTexImage2D(bufferParams.Target, 0, bufferParams.InternalFormat, m_FboParam->Width, m_FboParam->Height, 0, bufferParams.Format, bufferParams.Type, bufferParams.PixelData);

				for (auto j = 0; j < bufferParams.FboTexParams.size(); ++j)
				{
					SetTexParams(bufferParams, j);
				}

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, bufferParams.Target, m_ColorBuffers[i], 0);
				glDrawBuffer(GL_COLOR_ATTACHMENT0 + i);
			}
		}

		if (m_FboParam->DepthBufferFormat)
		{
			glGenRenderbuffers(1, &m_DepthBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, m_DepthBuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, m_FboParam->DepthBufferFormat, m_FboParam->Width, m_FboParam->Height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthBuffer);
		}
		if (m_FboParam->StencilBufferFormat)
		{
			//ToDo: Generate stencil buffer if needed
		}
		if (m_FboParam->DepthStencilBufferFormat)
		{
			//ToD: Generate Depth_Stencil buffer if needed
		}

		auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			printf("Framebuffer Genereration failed \n error: %i\n", status);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void SetTexParams(const FBOTexGenParams& params, const int& texParamIndex)
	{
		const GLenum& target = params.Target;
		const FBOTexParams& texParam = params.FboTexParams[texParamIndex];

		if (auto data = std::get_if<GLfloat>(&texParam.Params))
		{
			glTexParameterf(target, texParam.ParamName, *data);
		}
		else if (auto data = std::get_if<GLint>(&texParam.Params))
		{
			glTexParameteri(target, texParam.ParamName, *data);
		}
		else if (auto data = std::get_if<GLfloat*>(&texParam.Params))
		{
			glTexParameterfv(target, texParam.ParamName, *data);
		}
		else if (auto data = std::get_if<GLint*>(&texParam.Params))
		{
			glTexParameteriv(target, texParam.ParamName, *data);
			//ToDo: Add support for glTexParameterIiv
		}
		else if (auto data = std::get_if<GLuint*>(&texParam.Params))
		{
			glTexParameterIuiv(target, texParam.ParamName, *data);
		}
	}

	void WriteToBuffer() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FboId);
	}

	void AttachColorBufferToTexture(const GLenum& textureUnit, const GLuint& texParamIndex, const GLuint& bufferIndex) const
	{
		glActiveTexture(textureUnit);
		glBindTexture(m_FboParam->FboTexGenParams[texParamIndex].Target, m_ColorBuffers[bufferIndex]);
	}

	void ResizeBuffers(int width, int height)
	{
		m_FboParam->Width = width;
		m_FboParam->Height = height;

		for (auto fboTexParamIndex = 0; fboTexParamIndex < m_FboParam->FboTexGenParams.size(); ++fboTexParamIndex)
		{
			auto texCount = m_FboParam->FboTexGenParams[fboTexParamIndex].ColorBufferSize;
			FBOTexGenParams& bufferParams = m_FboParam->FboTexGenParams[fboTexParamIndex];
			for (auto i = 0; i < texCount; ++i)
			{
				glBindTexture(bufferParams.Target, m_ColorBuffers[i]);
				glTexImage2D(bufferParams.Target, 0, bufferParams.InternalFormat, width, height, bufferParams.Border, bufferParams.Format, bufferParams.Type, bufferParams.PixelData);
			}
			glBindTexture(bufferParams.Target, 0);
		}

		if (m_DepthBuffer)
		{
			glNamedRenderbufferStorage(m_DepthBuffer, GL_DEPTH_COMPONENT, width, height);
		}

		if (m_StencilBuffer)
		{
			glNamedRenderbufferStorage(m_StencilBuffer, GL_STENCIL_ATTACHMENT, width, height);
		}

		if (m_DepthStencilBuffer)
		{
			glNamedRenderbufferStorage(m_DepthStencilBuffer, GL_DEPTH_STENCIL_ATTACHMENT, width, height);
		}
	}

	GLuint GetWidth() const
	{
		return m_FboParam->Width;
	}
	GLuint GetHeight() const
	{
		return m_FboParam->Height;
	}

	~Impl()
	{
		if (m_FboId) { glDeleteFramebuffers(1, &m_FboId); }
		for (auto i = 0; i < m_ColorBuffers.size(); ++i)
		{
			if (m_ColorBuffers[i]) { glDeleteTextures(1, &m_ColorBuffers[i]); }
		}
		if (m_DepthBuffer) { glDeleteBuffers(1, &m_DepthBuffer); }
		if (m_StencilBuffer) { glDeleteBuffers(1, &m_StencilBuffer); }
		if (m_DepthStencilBuffer) { glDeleteBuffers(1, &m_DepthStencilBuffer); }
	}

};

FrameBufferObject::FrameBufferObject(const std::shared_ptr<FBOParams>& fboParams) : m_pImpl{ new Impl(fboParams) } {}

void FrameBufferObject::WriteToBuffer() const
{
	Pimpl()->WriteToBuffer();
}

void FrameBufferObject::AttachColorBufferToTexture(const GLenum& textureUnit, const GLuint& texParamIndex, const GLuint& bufferIndex) const
{
	Pimpl()->AttachColorBufferToTexture(textureUnit, texParamIndex, bufferIndex);
}

void FrameBufferObject::ResizeBuffers(int width, int height)
{
	Pimpl()->ResizeBuffers(width, height);
}

GLuint FrameBufferObject::GetWidth() const 
{
	return Pimpl()->GetWidth();
}
GLuint FrameBufferObject::GetHeight() const 
{
	return Pimpl()->GetHeight();
}

FrameBufferObject::~FrameBufferObject() = default;

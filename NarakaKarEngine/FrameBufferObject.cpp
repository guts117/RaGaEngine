#include "pch.h"
#include "FrameBufferObject.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

struct FrameBufferObject::Impl
{
	GLuint m_FboId;
	std::shared_ptr<FBOParams> m_FboParam;
	std::vector<GLuint>m_ReadWriteBuffers;
	std::vector<GLuint>m_WriteBuffers;

	Impl() = delete;

	Impl(const std::shared_ptr<FBOParams>& fboParams)
		: m_FboId{ 0 }
		, m_FboParam{ fboParams }
		, m_ReadWriteBuffers{ std::vector<GLuint>() }
		, m_WriteBuffers{ std::vector<GLuint>() }
 
	{
		glGenFramebuffers(1, &m_FboId);
		glBindFramebuffer(m_FboParam->Target, m_FboId);

		auto has_Color_Attachment = m_FboParam->Attachment == GL_COLOR_ATTACHMENT0;

		for (auto fboTexParamIndex = 0; fboTexParamIndex < m_FboParam->FboTexGenParams.size(); ++fboTexParamIndex)
		{
			auto texCount = has_Color_Attachment ? m_FboParam->FboTexGenParams[fboTexParamIndex].ColorBufferSize : 1;
			for (auto i = 0; i < texCount; ++i)
			{
				GLuint tempColorBuf = 0;
				glGenTextures(1, &tempColorBuf);

				FBOTexGenParams& bufferParams = m_FboParam->FboTexGenParams[fboTexParamIndex];
				glBindTexture(bufferParams.Target, tempColorBuf);
				if (bufferParams.Target == GL_TEXTURE_CUBE_MAP)
				{
					for (size_t i = 0; i < 6; i++) 
					{
						glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, bufferParams.Level, bufferParams.InternalFormat, m_FboParam->Width, m_FboParam->Height, bufferParams.Border, bufferParams.Format, bufferParams.Type, bufferParams.PixelData);
					}
				}
				else
				{
					glTexImage2D(bufferParams.Target, bufferParams.Level, bufferParams.InternalFormat, m_FboParam->Width, m_FboParam->Height, bufferParams.Border, bufferParams.Format, bufferParams.Type, bufferParams.PixelData);
				}

				if (!m_FboParam->IsAttachBufferLater)
				{
					//useless since OpenGL 4.5
					//glFramebufferTexture2D(m_FboParam->Target, m_FboParam->Attachment + m_ReadWriteBuffers.size(), bufferParams.Target, tempColorBuf, 0);
					glFramebufferTexture(m_FboParam->Target, m_FboParam->Attachment + m_ReadWriteBuffers.size(), tempColorBuf, 0);
				}

				for (auto j = 0; j < bufferParams.FboTexParams.size(); ++j)
				{
					SetTexParams(bufferParams, j);
				}

				m_ReadWriteBuffers.push_back(tempColorBuf);
			}
		}

		if (has_Color_Attachment)
		{
			std::unique_ptr<GLenum[]> attachments = std::make_unique<GLenum[]>(m_ReadWriteBuffers.size());
			for (int i = 0; i < m_ReadWriteBuffers.size(); ++i)
			{
				attachments[i] = m_FboParam->Attachment + i;
			}

			glDrawBuffers(m_ReadWriteBuffers.size(), attachments.get());
		}

		for(auto rboParamIndex = 0; rboParamIndex < m_FboParam->FBORenderBufferParams.size(); ++rboParamIndex)
		{
			GLuint tempRbo = 0;
			glGenRenderbuffers(1, &tempRbo);
			glBindRenderbuffer(GL_RENDERBUFFER, tempRbo);
			glRenderbufferStorage(GL_RENDERBUFFER, m_FboParam->FBORenderBufferParams[rboParamIndex].InternalFormat, m_FboParam->Width, m_FboParam->Height);
			glFramebufferRenderbuffer(m_FboParam->Target, m_FboParam->FBORenderBufferParams[rboParamIndex].Attachment + m_WriteBuffers.size(), GL_RENDERBUFFER, tempRbo);
			m_WriteBuffers.push_back(tempRbo);
		}

		auto status = glCheckFramebufferStatus(m_FboParam->Target);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			printf("Framebuffer Genereration failed \n error: %i\n", status);
		}

		glBindFramebuffer(m_FboParam->Target, 0);
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

	void Bind() const
	{
		glBindFramebuffer(m_FboParam->Target, m_FboId);
	}

	void WriteToBuffer(const GLuint& texParamIndex, const GLuint& bufferIndex, const GLuint& faceId, const GLuint& mipLevel) const
	{
		if (m_FboParam->FboTexGenParams[texParamIndex].Target == GL_TEXTURE_CUBE_MAP)
		{
			glFramebufferTextureLayer(m_FboParam->Target, m_FboParam->Attachment + bufferIndex, m_ReadWriteBuffers[bufferIndex], mipLevel, faceId);
			//useless since OpenGL 4.5
			//glFramebufferTexture2D(m_FboParam->Target, m_FboParam->Attachment + bufferIndex, GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceId, m_ReadWriteBuffers[bufferIndex], 0);
		}
	}

	void AttachColorBufferToTexture(const GLenum& textureUnit, const GLuint& texParamIndex, const GLuint& bufferIndex) const
	{
		glActiveTexture(textureUnit);
		glBindTexture(m_FboParam->FboTexGenParams[texParamIndex].Target, m_ReadWriteBuffers[bufferIndex]);
	}

	void CreateMipMap(const GLuint& texParamIndex, const GLuint& bufferIndex) const
	{
		if (m_ReadWriteBuffers[bufferIndex])
		{
			glBindTexture(m_FboParam->FboTexGenParams[texParamIndex].Target, m_ReadWriteBuffers[bufferIndex]);
			glGenerateMipmap(m_FboParam->FboTexGenParams[texParamIndex].Target);
		}
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
				glBindTexture(bufferParams.Target, m_ReadWriteBuffers[i]);
				glTexImage2D(bufferParams.Target, 0, bufferParams.InternalFormat, width, height, bufferParams.Border, bufferParams.Format, bufferParams.Type, bufferParams.PixelData);
			}
			glBindTexture(bufferParams.Target, 0);
		}

		for (auto rboParamIndex = 0; rboParamIndex < m_FboParam->FBORenderBufferParams.size(); ++rboParamIndex)
		{
			glGenRenderbuffers(1, &m_WriteBuffers[rboParamIndex]);
			glBindRenderbuffer(GL_RENDERBUFFER, m_WriteBuffers[rboParamIndex]);
			glRenderbufferStorage(GL_RENDERBUFFER, m_FboParam->FBORenderBufferParams[rboParamIndex].InternalFormat, m_FboParam->Width, m_FboParam->Height);
			glFramebufferRenderbuffer(m_FboParam->Target, m_FboParam->FBORenderBufferParams[rboParamIndex].Attachment, GL_RENDERBUFFER, m_WriteBuffers[rboParamIndex]);
		
			glNamedRenderbufferStorage(m_WriteBuffers[rboParamIndex], m_FboParam->FBORenderBufferParams[rboParamIndex].InternalFormat, width, height);
		}
	}

	const GLuint& GetWidth() const
	{
		return m_FboParam->Width;
	}

	const GLuint& GetHeight() const
	{
		return m_FboParam->Height;
	}

	~Impl()
	{
		if (m_FboId) { glDeleteFramebuffers(1, &m_FboId); }
		for (auto i = 0; i < m_ReadWriteBuffers.size(); ++i)
		{
			if (m_ReadWriteBuffers[i]) { glDeleteTextures(1, &m_ReadWriteBuffers[i]); }
		}
		for (auto i = 0; i < m_WriteBuffers.size(); ++i)
		{
			if (m_WriteBuffers[i]) { glDeleteBuffers(1, &m_WriteBuffers[i]); }
		}
	}
};

FrameBufferObject::FrameBufferObject(const std::shared_ptr<FBOParams>& fboParams) : m_pImpl{ new Impl(fboParams) } {}

void FrameBufferObject::Bind() const
{
	Pimpl()->Bind();
}

void FrameBufferObject::WriteToBuffer(const GLuint& texParamIndex, const GLuint& bufferIndex, const GLuint& faceId, const GLuint& mipLevel) const
{
	Pimpl()->WriteToBuffer(texParamIndex, bufferIndex, faceId, mipLevel);
}

void FrameBufferObject::AttachColorBufferToTexture(const GLenum& textureUnit, const GLuint& texParamIndex, const GLuint& bufferIndex) const
{
	Pimpl()->AttachColorBufferToTexture(textureUnit, texParamIndex, bufferIndex);
}

void FrameBufferObject::CreateMipMap(const GLuint& texParamIndex, const GLuint& bufferIndex) const
{
	Pimpl()->CreateMipMap(texParamIndex, bufferIndex);
}

void FrameBufferObject::ResizeBuffers(int width, int height)
{
	Pimpl()->ResizeBuffers(width, height);
}

const GLuint& FrameBufferObject::GetWidth() const
{
	return Pimpl()->GetWidth();
}

const GLuint& FrameBufferObject::GetHeight() const
{
	return Pimpl()->GetHeight();
}

const GLuint& FrameBufferObject::GetBuffer(const GLuint& bufferIndex) const
{
	return Pimpl()->m_ReadWriteBuffers[bufferIndex];
}

FrameBufferObject::~FrameBufferObject() = default;

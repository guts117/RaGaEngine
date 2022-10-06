#ifndef PRE_FILTER_PASS_FBO_HANDLER
#define PRE_FILTER_PASS_FBO_HANDLER

#include "pch.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class Pre_Filter_Pass_Fbo_Handler
		{
		public:
			Pre_Filter_Pass_Fbo_Handler() = delete;
			explicit Pre_Filter_Pass_Fbo_Handler(const GLuint& width, const GLuint& height);

			Pre_Filter_Pass_Fbo_Handler(Pre_Filter_Pass_Fbo_Handler&& rhs) noexcept = default;
			Pre_Filter_Pass_Fbo_Handler& operator=(Pre_Filter_Pass_Fbo_Handler&& rhs) noexcept = default;

			Pre_Filter_Pass_Fbo_Handler(const Pre_Filter_Pass_Fbo_Handler& rhs) noexcept = delete;
			Pre_Filter_Pass_Fbo_Handler& operator=(const Pre_Filter_Pass_Fbo_Handler& rhs) noexcept = delete;

			void BindFBO() const;
			void WriteToFBOBuffer(const GLuint& faceId, const GLuint& mipLevel) const;
			void AttachFBOToTextureUnit(const GLenum& textureUnit) const;
			void CreateFBOMipMap() const;
			void ResizeFBO(const GLuint& width, const GLuint& height);

			const GLuint& GetFBOWidth() const;
			const GLuint& GetFBOHeight() const;
			const GLuint& GetFBOBuffer(const GLuint& bufferIndex) const;

			~Pre_Filter_Pass_Fbo_Handler();

		private:
			struct Impl;

			const Impl* Pimpl() const { return m_pImpl.get(); }
			Impl* Pimpl() { return m_pImpl.get(); }

			std::unique_ptr<Impl> m_pImpl;
		};
	}
}

#endif
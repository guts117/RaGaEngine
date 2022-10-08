#ifndef EQUIRECT_TO_CUBEMAP_PASS_FBO_HANDLER
#define EQUIRECT_TO_CUBEMAP_PASS_FBO_HANDLER

#include "pch.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class Equirect_To_Cubemap_Pass_Fbo_Handler
		{
		public:
			Equirect_To_Cubemap_Pass_Fbo_Handler() = delete;
			explicit Equirect_To_Cubemap_Pass_Fbo_Handler(const GLuint& width, const GLuint& height, const bool& hasMipMap = false);

			Equirect_To_Cubemap_Pass_Fbo_Handler(Equirect_To_Cubemap_Pass_Fbo_Handler&& rhs) noexcept = default;
			Equirect_To_Cubemap_Pass_Fbo_Handler& operator=(Equirect_To_Cubemap_Pass_Fbo_Handler&& rhs) noexcept = default;

			Equirect_To_Cubemap_Pass_Fbo_Handler(const Equirect_To_Cubemap_Pass_Fbo_Handler& rhs) noexcept = delete;
			Equirect_To_Cubemap_Pass_Fbo_Handler& operator=(const Equirect_To_Cubemap_Pass_Fbo_Handler& rhs) noexcept = delete;

			void BindFBO() const;
			void WriteToFBOBuffer(const GLuint& faceId) const;
			void AttachFBOToTextureUnit(const GLenum& textureUnit) const;
			void CreateFBOMipMap() const;
			void ResizeFBO(const GLuint& width, const GLuint& height);

			const GLuint& GetFBOWidth() const;
			const GLuint& GetFBOHeight() const;
			const GLuint& GetFBOBuffer(const GLuint& bufferIndex) const;

			~Equirect_To_Cubemap_Pass_Fbo_Handler();

		private:
			struct Impl;

			const Impl* Pimpl() const { return m_pImpl.get(); }
			Impl* Pimpl() { return m_pImpl.get(); }

			std::unique_ptr<Impl> m_pImpl;
		};
	}
}

#endif
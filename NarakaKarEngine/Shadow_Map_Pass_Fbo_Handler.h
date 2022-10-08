#ifndef SHADOW_MAP_FRAME_BUFFER
#define SHADOW_MAP_FRAME_BUFFER

#include "pch.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class Shadow_Map_Pass_Fbo_Handler
		{
		public:
			Shadow_Map_Pass_Fbo_Handler() = delete;
			explicit Shadow_Map_Pass_Fbo_Handler(const GLuint& width, const GLuint& height);

			Shadow_Map_Pass_Fbo_Handler(Shadow_Map_Pass_Fbo_Handler&& rhs) noexcept = default;
			Shadow_Map_Pass_Fbo_Handler& operator=(Shadow_Map_Pass_Fbo_Handler&& rhs) noexcept = default;

			Shadow_Map_Pass_Fbo_Handler(const Shadow_Map_Pass_Fbo_Handler& rhs) noexcept = delete;
			Shadow_Map_Pass_Fbo_Handler& operator=(const Shadow_Map_Pass_Fbo_Handler& rhs) noexcept = delete;

			void BindFBO() const;
			void WriteToFBOBuffer(const GLuint& cascadeIndex) const;
			void AttachFBOToTextureUnit(const GLenum& textureUnit, const GLenum& bufferIndex) const;
			void CreateFBOMipMap() const;
			void ResizeFBO(const GLuint& width, const GLuint& height);

			const GLuint& GetFBOWidth() const;
			const GLuint& GetFBOHeight() const;
			const GLuint& GetFBOBuffer(const GLuint& bufferIndex) const;

			~Shadow_Map_Pass_Fbo_Handler();

		private:
			struct Impl;

			const Impl* Pimpl() const { return m_pImpl.get(); }
			Impl* Pimpl() { return m_pImpl.get(); }

			std::unique_ptr<Impl> m_pImpl;
		};
	}
}

#endif
#ifndef TEXTURE
#define TEXTURE

#include "render_pch.h"
//#include "spimpl.h"

namespace NarakaRenderEngine
{
	namespace RenderEngine
	{
		enum TexType
		{
			Default,
			Albedo,
			Metallic,
			Roughness,
			Normal,
			Parallax,
			Glow,
			Displacement,
			Blend,
			Noise,
			Max
		};

		class Texture
		{
		public:
			struct Impl;

			explicit Texture();
			explicit Texture(std::string fileLoc, bool isSRGB = false);

			Texture(Texture&& rhs) noexcept = default;
			Texture& operator=(Texture&& rhs) noexcept = default;

			Texture(const Texture& rhs) noexcept = delete;
			Texture& operator=(const Texture& rhs) noexcept = delete;

			bool LoadTexture2D();
			bool LoadTextureArray(const glm::vec2& resolution, const int numOfLayers);
			bool LoadCubeMap();
			bool LoadTextureHDR();

			bool LoadNativeTexture(const std::vector<glm::vec3>& noiseData);
			bool CreateTextureArray(const glm::vec2& resolution, const int numOfLayers, bool createMipMaps = false);

			bool CreateTexture3D(const glm::vec3& resolution, bool createMipMaps = false);

			bool CreateTexture(const glm::vec2& resolution);

			void UseTextureTemp(GLuint i);

			void UseTexture(GLuint i);
			void UseTextureArray(GLuint i);
			void UseTexture3D(GLuint i);
			void UseTextureReadWrite(GLuint i, bool isWriteOnly, bool isLayered);
			void UseCubeMap(GLuint i);

			void GetTextureData(float* data);

			//Getters
			const int GetWidth();
			const int GetHeight();
			const int GetBitDepth();
			const GLuint GetTextureID();

			~Texture();

		private:

			const Impl* Pimpl() const { return m_pImpl; }
			Impl* Pimpl() { return m_pImpl; }

#ifdef NDEBUG //size of string is different between debug and release
			static const size_t BuffSize = alignof(std::string) * 7;
#else
			static const size_t BuffSize = alignof(std::string) * 8;
#endif
			static const size_t BuffAlign = alignof(std::string);
			
			alignas(BuffAlign) std::byte buffer[BuffSize];
			Impl* m_pImpl;
		};
	}
}
#endif
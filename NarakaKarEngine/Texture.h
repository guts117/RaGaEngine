#ifndef TEXTURE
#define TEXTURE

#include "pch.h"
//#include "spimpl.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class Texture
		{
		public:
			explicit Texture();
			explicit Texture(std::string fileLoc, bool isSRGB = false);

			Texture(Texture&& rhs) noexcept = default;
			Texture& operator=(Texture&& rhs) noexcept = default;

			Texture(const Texture& rhs) noexcept = delete;
			Texture& operator=(const Texture& rhs) noexcept = delete;

			bool LoadTextureWithAlpha();
			bool LoadTextureNoAlpha();
			bool LoadTextureArray(const glm::vec2& resolution, const int numOfLayers);
			bool LoadCubeMap();
			bool LoadTextureHDR();

			bool LoadNativeTexture(const std::vector<glm::vec3>& noiseData);
			bool CreateTextureArray(const glm::vec2& resolution, const int numOfLayers);

			bool CreateTexture(const glm::vec2& resolution);

			void UseTexture(GLuint i);
			void UseTextureArray(GLuint i);
			void UseTextureReadWrite(GLuint i, bool isWriteOnly, bool isLayered);
			void UseCubeMap(GLuint i);

			//Getters
			const int GetWidth();
			const int GetHeight();
			const int GetBitDepth();

			~Texture();
		private:
			struct Impl;

			const Impl* Pimpl() const { return m_pImpl.get(); }
			Impl* Pimpl() { return m_pImpl.get(); }

			std::unique_ptr<Impl> m_pImpl;
		};
	}
}
#endif
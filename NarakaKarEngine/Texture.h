#ifndef TEXTURE
#define TEXTURE

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <memory>
//#include "spimpl.h"

class Texture
{
public:
	explicit Texture();
	explicit Texture(std::string fileLoc);

	Texture(Texture&& rhs) noexcept = default;
	Texture& operator=(Texture&& rhs) noexcept = default;

	Texture(const Texture& rhs) noexcept;
	Texture& operator=(const Texture& rhs) noexcept;

	bool LoadTexture();
	bool LoadTextureA();
	bool LoadTextureSRGB();
	bool LoadTextureArray(bool is_SRGB, bool is_lowres);
	bool LoadTextureSRGBA();
	bool LoadCubeMap(const std::vector<std::string>& faceLocation);
	bool LoadCubeMapSRGB(const std::vector<std::string>& faceLocation);
	bool LoadTextureHDR();

	bool GenerateNoiseTexture(const std::vector<glm::vec3>& noiseData);

	void UseTexture(GLuint i);
	void UseTextureArray(GLuint i);
	void UseCubeMap(GLuint i);

	//Getters
	int GetWidth();
	int GetHeight();
	int GetBitDepth();

	~Texture();
private:
	struct Impl;

	const Impl* Pimpl() const { return m_pImpl.get(); }
	Impl* Pimpl() { return m_pImpl.get(); }

	std::unique_ptr<Impl> m_pImpl;
};

#endif
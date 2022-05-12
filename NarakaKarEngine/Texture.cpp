#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION 
#include "stb_image.h"
#include "CommonValues.h"
#include <cstring>
#include <cstdio>

struct Texture::Impl
{
	GLuint textureID = 0;
	int width = 0, height = 0, bitDepth = 0;
	std::string fileLocation = "";

	Impl() = default;

	Impl(std::string fileLoc)
		: fileLocation{ fileLoc }
	{}

	Impl(Impl&& rhs) noexcept
		: textureID{ std::exchange(rhs.textureID, 0) }
		, width{ std::exchange(rhs.width, 0) }
		, height{ std::exchange(rhs.height, 0) }
		, bitDepth{ std::exchange(rhs.bitDepth, 0) }
		, fileLocation{ std::move(rhs.fileLocation) }
	{}

	Impl& operator=(Impl&& rhs) noexcept {
		if (this != &rhs) {
			textureID = std::exchange(rhs.textureID, 0);
			width = std::exchange(rhs.width, 0);
			height = std::exchange(rhs.height, 0);
			bitDepth = std::exchange(rhs.bitDepth, 0);
			fileLocation = std::move(rhs.fileLocation);
		}
		return *this;
	}

	Impl(const Impl& rhs) noexcept
		: textureID{ rhs.textureID }
		, width{ rhs.width }
		, height{ rhs.height }
		, bitDepth{ rhs.bitDepth }
		, fileLocation{ rhs.fileLocation }
	{}

	Impl& operator=(const Impl& rhs) {
		if (this != &rhs) {
			auto temp(rhs);
			std::swap(*this, temp);
			return *this;
		}
		return *this;
	}

	bool LoadTextureA() {
		stbi_set_flip_vertically_on_load(false);
		unsigned char* texData = stbi_load(fileLocation.c_str(), &width, &height, &bitDepth, 0);

		if (!texData) {
			printf("Failed to find: %s\n", fileLocation.c_str());
			return false;
		}
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(texData);

		return true;
	}


	bool LoadTextureSRGBA()
	{
		stbi_set_flip_vertically_on_load(false);
		unsigned char* texData = stbi_load(fileLocation.c_str(), &width, &height, &bitDepth, 0);

		if (!texData) {
			printf("Failed to find: %s\n", fileLocation.c_str());
			return false;
		}
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(texData);

		return true;
	}

	bool LoadTexture()
	{
		stbi_set_flip_vertically_on_load(false);
		unsigned char* texData = stbi_load(fileLocation.c_str(), &width, &height, &bitDepth, 0);

		if (!texData) {
			printf("Failed to find: %s\n", fileLocation.c_str());
			return false;
		}
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(texData);

		return true;
	}

	bool LoadTextureArray(bool is_SRGB, bool is_lowres)
	{
		stbi_set_flip_vertically_on_load(false);

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);

		GLenum intFormat;

		if (is_SRGB) {
			intFormat = GL_SRGB8;
		}
		else {
			intFormat = GL_RGB8;
		}

		int w, h;

		if (is_lowres) w = h = 256;
		else w = h = 1024;
		// Allocate the storage.
		glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, intFormat, w, h, NUM_TERRAIN_LAYERS);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, NUM_TERRAIN_LAYERS);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

		unsigned char* texData[NUM_TERRAIN_LAYERS] = { nullptr };
		for (int i = 0; i < NUM_TERRAIN_LAYERS; ++i)
		{
			std::string loc = fileLocation + std::to_string(i) + ".jpg";

			texData[i] = stbi_load(loc.c_str(), &width, &height, &bitDepth, 0);

			if (!texData[i]) {
				printf("Failed to find: %s\n", loc.c_str());
				return false;
			}

			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, w, h, 1, GL_RGB, GL_UNSIGNED_BYTE, texData[i]);
			stbi_image_free(texData[i]);
		}
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
		return true;
	}

	bool LoadTextureSRGB()
	{
		stbi_set_flip_vertically_on_load(false);
		unsigned char* texData = stbi_load(fileLocation.c_str(), &width, &height, &bitDepth, 0);

		if (!texData) {
			printf("Failed to find: %s\n", fileLocation.c_str());
			return false;
		}
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(texData);

		return true;
	}

	bool LoadCubeMap(const std::vector<std::string>& faceLocation)
	{
		stbi_set_flip_vertically_on_load(false);
		//texture setup
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, bitDepth;
		for (size_t i = 0; i < 6; i++)
		{
			unsigned char* texData = stbi_load(faceLocation[i].c_str(), &width, &height, &bitDepth, 0);

			if (!texData) {
				printf("Failed to find: %s\n", faceLocation[i].c_str());
				stbi_image_free(texData);
			}
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
			stbi_image_free(texData);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return true;
	}

	bool LoadCubeMapSRGB(const std::vector<std::string>& faceLocation)
	{
		stbi_set_flip_vertically_on_load(false);
		//texture setup
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, bitDepth;
		for (size_t i = 0; i < 6; i++)
		{
			unsigned char* texData = stbi_load(faceLocation[i].c_str(), &width, &height, &bitDepth, 0);

			if (!texData) {
				printf("Failed to find: %s\n", faceLocation[i].c_str());
				stbi_image_free(texData);
			}
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
			stbi_image_free(texData);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return true;
	}

	bool LoadTextureHDR()
	{
		stbi_set_flip_vertically_on_load(true);
		float* texData = stbi_loadf(fileLocation.c_str(), &width, &height, &bitDepth, 0);

		if (!texData) {
			printf("Failed to find HDR TEXTURE: %s\n", fileLocation.c_str());
			return false;
		}
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, texData);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(texData);

		return true;
	}

	bool GenerateNoiseTexture(const std::vector<glm::vec3>& noiseData)
	{
		stbi_set_flip_vertically_on_load(false);
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, &noiseData[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		return true;
	}

	void UseTexture(GLuint i) {
		glActiveTexture(GL_TEXTURE1 + i);
		glBindTexture(GL_TEXTURE_2D, textureID);
	}

	void UseTextureArray(GLuint i) {
		glActiveTexture(GL_TEXTURE1 + i);
		glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);
	}

	void UseCubeMap(GLuint i)
	{
		glActiveTexture(GL_TEXTURE1 + i);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	}

	void ClearTexture() {
		glDeleteTextures(1, &textureID);
	}

	~Impl() {
		ClearTexture();
	};
};

Texture::Texture() :  m_pImpl{ new Impl() } {}

Texture::Texture(std::string fileLoc) : m_pImpl{ new Impl(fileLoc) } {}

Texture::Texture(const Texture& rhs) noexcept : m_pImpl{ new Impl(*rhs.m_pImpl)} {}

Texture& Texture::operator=(const Texture& rhs) noexcept {
	if (this != &rhs)
	{
		m_pImpl.reset(new Impl(*rhs.m_pImpl));
	}
	return *this;
}

bool Texture::LoadTextureA() {
	return Pimpl()->LoadTextureA();
}

bool Texture::LoadTextureSRGBA() {
	return Pimpl()->LoadTextureSRGBA();
}

bool Texture::LoadTexture() {
	return Pimpl()->LoadTexture();
}

bool Texture::LoadTextureArray(bool is_SRGB, bool is_lowres) {
	return Pimpl()->LoadTextureArray(is_SRGB, is_lowres);
}

bool Texture::LoadTextureSRGB() {
	return Pimpl()->LoadTextureSRGB();
}

bool Texture::LoadCubeMap(const std::vector<std::string>& faceLocation) {
	return Pimpl()->LoadCubeMap(faceLocation);
}

bool Texture::LoadCubeMapSRGB(const std::vector<std::string>& faceLocation) {
	return Pimpl()->LoadCubeMapSRGB(faceLocation);
}

bool Texture::LoadTextureHDR() {
	return Pimpl()->LoadTextureHDR();
}

bool Texture::GenerateNoiseTexture(const std::vector<glm::vec3>& noiseData) {
	return Pimpl()->GenerateNoiseTexture(noiseData);
}

void Texture::UseTexture(GLuint i) {
	Pimpl()->UseTexture(i);
}

void Texture::UseTextureArray(GLuint i) {
	Pimpl()->UseTextureArray(i);
}

void Texture::UseCubeMap(GLuint i) {
	Pimpl()->UseCubeMap(i);
}

//Getters
int Texture::GetWidth() {
	return Pimpl()->width;
}

int Texture::GetHeight() {
	return Pimpl()->height;
}

int Texture::GetBitDepth() {
	return Pimpl()->bitDepth;
}

Texture::~Texture() = default;
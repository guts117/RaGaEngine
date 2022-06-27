#include "pch.h"
#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION 
#include "stb_image.h"
#include "CommonValues.h"


struct Texture::Impl
{
	GLuint m_textureID			= 0;
	int m_width = 0, m_height = 0, m_bitDepth = 0;
	std::string m_fileLocation	= "";
	bool m_isSRGB				= false;

	Impl() = default;

	Impl(std::string fileLoc, bool isSRGB)
		: m_fileLocation{ fileLoc }
		, m_isSRGB{ isSRGB }
	{}

	Impl(Impl&& rhs) noexcept
		: m_textureID{ std::exchange(rhs.m_textureID, 0) }
		, m_width{ std::exchange(rhs.m_width, 0) }
		, m_height{ std::exchange(rhs.m_height, 0) }
		, m_bitDepth{ std::exchange(rhs.m_bitDepth, 0) }
		, m_fileLocation{ std::move(rhs.m_fileLocation) }
		, m_isSRGB{ std::exchange(rhs.m_isSRGB, false)}
	{}

	Impl& operator=(Impl&& rhs) noexcept {
		if (this != &rhs) {
			m_textureID = std::exchange(rhs.m_textureID, 0);
			m_width = std::exchange(rhs.m_width, 0);
			m_height = std::exchange(rhs.m_height, 0);
			m_bitDepth = std::exchange(rhs.m_bitDepth, 0);
			m_fileLocation = std::move(rhs.m_fileLocation);
			m_isSRGB = std::exchange(rhs.m_isSRGB, false);
		}
		return *this;
	}

	Impl(const Impl& rhs) noexcept = delete;
	Impl& operator=(const Impl& rhs) = delete;

	bool LoadTextureWithAlpha() {
		stbi_set_flip_vertically_on_load(false);
		auto texData = stbi_load(m_fileLocation.c_str(), &m_width, &m_height, &m_bitDepth, 0);

		if (!texData) {
			printf("Failed to find: %s\n", m_fileLocation.c_str());
			return false;
		}

		auto intFormat = m_isSRGB ? GL_SRGB_ALPHA : GL_RGBA;

		glGenTextures(1, &m_textureID);
		glBindTexture(GL_TEXTURE_2D, m_textureID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, intFormat, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(texData);

		return true;
	}

	bool LoadTextureNoAlpha()
	{
		stbi_set_flip_vertically_on_load(false);
		auto texData = stbi_load(m_fileLocation.c_str(), &m_width, &m_height, &m_bitDepth, 0);

		if (!texData) {
			printf("Failed to find: %s\n", m_fileLocation.c_str());
			return false;
		}

		auto intFormat = m_isSRGB ? GL_SRGB : GL_RGB;

		glGenTextures(1, &m_textureID);
		glBindTexture(GL_TEXTURE_2D, m_textureID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, intFormat, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(texData);

		return true;
	}

	bool LoadTextureArray(const glm::vec2& resolution, const int numOfLayers)
	{
		stbi_set_flip_vertically_on_load(false);

		glGenTextures(1, &m_textureID);
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureID);

		auto intFormat = m_isSRGB ? GL_SRGB8 : GL_RGB8;
		
		// Allocate the storage.
		glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, intFormat, resolution.x, resolution.y, numOfLayers);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, numOfLayers);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

		std::vector<stbi_uc*> texData;
		for (int i = 0; i < numOfLayers; ++i)
		{
			auto loc0 = m_fileLocation.find_first_of(".");
			auto loc = m_fileLocation;
			loc.insert(loc0, std::to_string(i));

			texData.push_back(stbi_load(loc.c_str(), &m_width, &m_height, &m_bitDepth, 0));

			if (!texData[i]) {
				printf("Failed to find: %s\n", loc.c_str());
				return false;
			}

			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, resolution.x, resolution.y, 1, GL_RGB, GL_UNSIGNED_BYTE, texData[i]);
			stbi_image_free(texData[i]);
		}
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
		return true;
	}

	bool LoadCubeMap()
	{
		stbi_set_flip_vertically_on_load(false);
		//texture setup
		glGenTextures(1, &m_textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);

		auto intFormat = m_isSRGB ? GL_SRGB : GL_RGB;

		//unpack individual face location
		char* next_token = NULL;
		auto loc = const_cast<char*>(m_fileLocation.c_str());
		std::vector<char*> faceLocation;
		auto token = strtok_s(loc, "\n", &next_token);
		while (token != NULL)
		{
			faceLocation.push_back(token);
			token = strtok_s(NULL, "\n", &next_token);
		}

		for (size_t i = 0; i < 6; i++)
		{
			auto texData = stbi_load(faceLocation[i], &m_width, &m_height, &m_bitDepth, 0);

			if (!texData) {
				printf("Failed to find: %s\n", faceLocation[i]);
				stbi_image_free(texData);
			}
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, intFormat, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
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
		auto texData = stbi_loadf(m_fileLocation.c_str(), &m_width, &m_height, &m_bitDepth, 0);

		if (!texData) {
			printf("Failed to find HDR TEXTURE: %s\n", m_fileLocation.c_str());
			return false;
		}
		glGenTextures(1, &m_textureID);
		glBindTexture(GL_TEXTURE_2D, m_textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_width, m_height, 0, GL_RGB, GL_FLOAT, texData);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(texData);

		return true;
	}

	bool LoadNativeTexture(const std::vector<glm::vec3>& texData)
	{
		glGenTextures(1, &m_textureID);
		glBindTexture(GL_TEXTURE_2D, m_textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, &texData[0]);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glBindTexture(GL_TEXTURE_2D, 0);
		return true;
	}

	void UseTexture(GLuint i) {
		glActiveTexture(GL_TEXTURE1 + i);
		glBindTexture(GL_TEXTURE_2D, m_textureID);
	}

	void UseTextureArray(GLuint i) {
		glActiveTexture(GL_TEXTURE1 + i);
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureID);
	}

	void UseCubeMap(GLuint i)
	{
		glActiveTexture(GL_TEXTURE1 + i);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);
	}

	void ClearTexture() {
		glDeleteTextures(1, &m_textureID);
	}

	~Impl() {
		ClearTexture();
	};
};

Texture::Texture() :  m_pImpl{ new Impl() } {}

Texture::Texture(std::string fileLoc, bool isSRGB) : m_pImpl{ new Impl(fileLoc, isSRGB) } {}

bool Texture::LoadTextureWithAlpha() {
	return Pimpl()->LoadTextureWithAlpha();
}

bool Texture::LoadTextureNoAlpha() {
	return Pimpl()->LoadTextureNoAlpha();
}

bool Texture::LoadTextureArray(const glm::vec2& resolution, const int numOfLayers) {
	return Pimpl()->LoadTextureArray(resolution, numOfLayers);
}

bool Texture::LoadCubeMap() {
	return Pimpl()->LoadCubeMap();
}

bool Texture::LoadTextureHDR() {
	return Pimpl()->LoadTextureHDR();
}

bool Texture::LoadNativeTexture(const std::vector<glm::vec3>& noiseData) {
	return Pimpl()->LoadNativeTexture(noiseData);
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
const int Texture::GetWidth() {
	return Pimpl()->m_width;
}

const int Texture::GetHeight() {
	return Pimpl()->m_height;
}

const int Texture::GetBitDepth() {
	return Pimpl()->m_bitDepth;
}

Texture::~Texture() = default;
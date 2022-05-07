#include "Texture.h"

Texture::Texture() {
	textureID = 0;
	width = 0;
	height = 0;
	bitDepth = 0;
	fileLocation = nullptr;
}

Texture::Texture (const char* fileLoc) {
	textureID = 0;
	width = 0;
	height = 0;
	bitDepth = 0;
	fileLocation = fileLoc;
}

bool Texture::LoadTextureA() {
	stbi_set_flip_vertically_on_load(false);
	unsigned char* texData = stbi_load(fileLocation, &width, &height, &bitDepth, 0);
	
	if (!texData) {
		printf("Failed ot find: %s\n", fileLocation);
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


bool Texture::LoadTextureSRGBA()
{
	stbi_set_flip_vertically_on_load(false);
	unsigned char* texData = stbi_load(fileLocation, &width, &height, &bitDepth, 0);

	if (!texData) {
		printf("Failed ot find: %s\n", fileLocation);
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

bool Texture::LoadTexture()
{
	stbi_set_flip_vertically_on_load(false);
	unsigned char* texData = stbi_load(fileLocation, &width, &height, &bitDepth, 0);

	if (!texData) {
		printf("Failed ot find: %s\n", fileLocation);
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

bool Texture::LoadTextureArray(bool is_SRGB, bool is_lowres)
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

	unsigned char* texData[NUM_TERRAIN_LAYERS] = {nullptr};
	for (int i = 0; i < NUM_TERRAIN_LAYERS; ++i)
	{
		std::string loc = fileLocation+std::to_string(i) + ".jpg";
		
		texData[i] = stbi_load(loc.c_str(), &width, &height, &bitDepth, 0);

		if (!texData[i]) {
			printf("Failed ot find: %s\n", loc.c_str());
			return false;
		}

		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, w, h, 1, GL_RGB, GL_UNSIGNED_BYTE, texData[i]);
		stbi_image_free(texData[i]);
	}
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	return true;
}

bool Texture::LoadTextureSRGB()
{
	stbi_set_flip_vertically_on_load(false);
	unsigned char* texData = stbi_load(fileLocation, &width, &height, &bitDepth, 0);

	if (!texData) {
		printf("Failed ot find: %s\n", fileLocation);
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

bool Texture::LoadCubeMap(std::vector<std::string> faceLocation)
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
			printf("Failed ot find: %s\n", faceLocation[i].c_str());
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

bool Texture::LoadCubeMapSRGB(std::vector<std::string> faceLocation)
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
			printf("Failed ot find: %s\n", faceLocation[i].c_str());
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

bool Texture::LoadTextureHDR()
{
	stbi_set_flip_vertically_on_load(true);
	float* texData = stbi_loadf(fileLocation, &width, &height, &bitDepth, 0);

	if (!texData) {
		printf("Failed ot find HDR TEXTURE: %s\n", fileLocation);
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

bool Texture::GenerateNoiseTexture(std::vector<glm::vec3>& noiseData)
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

void Texture::UseTexture(unsigned int i) {
	glActiveTexture(GL_TEXTURE1+i);
	glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture::UseTextureArray(unsigned int i) {
	glActiveTexture(GL_TEXTURE1 + i);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);
}

void Texture::UseCubeMap(unsigned int i)
{
	glActiveTexture(GL_TEXTURE1 + i);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
}

void Texture::ClearTexture() {
	glDeleteTextures(1, &textureID);
	textureID = 0;
	width = 0;
	height = 0;
	bitDepth = 0;
	fileLocation = nullptr;
}

Texture::~Texture() {
	ClearTexture();
}
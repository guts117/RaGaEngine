#include "Static_Object.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define GLM_FORCE_CTOR_INIT
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <math.h>
#include <functional>

#include "Static_Model.h"
#include "Static_Mesh.h"
#include "Material.h"
#include "Texture.h"
#include "Shader.h"
#include "Camera.h"
#include "DirectionalLight.h"
#include "SpotLight.h"
#include "PointLight.h"
#include "CommonValues.h"

void Static_Object::SetUpNativeModelData(std::shared_ptr<Static_Mesh> mesh,
	std::string albedoPath,
	std::string metalPath,
	std::string roughPath,
	std::string normalPath,
	std::string parallaxPath,
	std::string glowPath)
{

	LoadTexture(m_albedoTexture.get(), albedoPath, true);
	LoadTexture(m_metallicTexture.get(), metalPath);
	LoadTexture(m_roughTexture.get(), roughPath);
	LoadTexture(m_normalTexture.get(), normalPath);
	LoadTexture(m_parallaxTexture.get(), parallaxPath);
	LoadTexture(m_glowTexture.get(), glowPath);
	m_staticMesh = mesh;
	m_material = std::make_unique<Material>(1, 6, 7, 11, 12, 13);
}

void Static_Object::SetUpImportedModelData(std::string modelPath)
{
	StaticModel->LoadModel(modelPath);
	m_material = std::make_unique<Material>(1, 6, 7, 11, 12, 13);
}

void Static_Object::LoadTexture(Texture* texture, std::string path, bool isSRGB)
{
	if (path != "")
	{
		auto hasAlpha = [](std::string fileName) -> bool {return !(fileName.find(".png") == std::string::npos); };

		auto loadTexture = [hasAlpha](Texture* texture, std::string path, bool isSRGB = false)
			-> void
		{
			if (hasAlpha(path))
			{
				if (isSRGB)
					texture->LoadTextureSRGBA();
				else
					texture->LoadTextureA();
			}
			else
			{
				if (isSRGB)
					texture->LoadTextureSRGB();
				else
					texture->LoadTexture();
			}
		};
	}
}

void Static_Object::SetUniformLocations(std::shared_ptr<Shader> shader)
{
	m_uniformModel = shader->GetModelLocation();
	m_uniformProjection = shader->GetProjectionLocation();
	m_uniformView = shader->GetViewLocation();
	m_uniformPrevPVM = shader->GetPrevPVMLocation();
	m_uniformEyePosition = shader->GetEyePositionLocation();
	m_uniformHeightScale = shader->GetHeightScaleLocation();
	m_uniformAlbedoMap = shader->GetAlbedoLocation();
	m_uniformMetallicMap = shader->GetMetallicLocation();
	m_uniformNormalMap = shader->GetNormalLocation();
	m_uniformRoughnessMap = shader->GetRoughnessLocation();
	m_uniformParallaxMap = shader->GetParallaxLocation();
	m_uniformGlowMap = shader->GetGlowLocation();
}

void Static_Object::DrawNativeObject(glm::mat4 projectionMatrix,
									glm::mat4 viewMatrix,
									int pointLightCount,
									int spotLightCount,
									glm::mat4 prevProjView,
									float terrainScaleFactor,
									float rotationAngle,
									glm::vec3 position,
									glm::vec3 rotationAxis,
									glm::vec3 scale)
{
	//m_model = glm::translate(m_model, position);
	//m_model = glm::rotate(m_model, rotationAngle * toRadians, rotationAxis);  //if you put the rotate at the last place(i.e on the top) it will have a bouncy effect
	//m_model = glm::scale(m_model, scale);
	//glUniformMatrix4fv(m_uniformModel, 1, GL_FALSE, glm::value_ptr(m_model));
	m_prevPVM = prevProjView * m_staticMesh->prevMesh;
	glUniformMatrix4fv(m_uniformPrevPVM, 1, GL_FALSE, glm::value_ptr(m_prevPVM));
	m_albedoTexture->UseTexture(albedoTexUnit);
	m_metallicTexture->UseTexture(metallicTexUnit);
	m_normalTexture->UseTexture(normalTexUnit);
	m_roughTexture->UseTexture(roughTexUnit);
	m_parallaxTexture->UseTexture(parallaxTexUnit);
	m_glowTexture->UseTexture(glowTexUnit);
	m_material->UseMaterial(m_uniformAlbedoMap, m_uniformMetallicMap, m_uniformNormalMap, m_uniformRoughnessMap, m_uniformParallaxMap, m_uniformGlowMap);
	m_staticMesh->RenderMesh();
	//m_staticMesh->prevMesh = m_model;
}

void Static_Object::DrawImportedObject(std::shared_ptr<Shader> shader, glm::mat4 prevProjView, glm::mat4 model)
{
	SetUniformLocations(shader);

	m_prevPVM = prevProjView * StaticModel->prevModel;
	glUniformMatrix4fv(m_uniformPrevPVM, 1, GL_FALSE, glm::value_ptr(m_prevPVM));
	m_material->UseMaterial(m_uniformAlbedoMap, m_uniformMetallicMap, m_uniformNormalMap, m_uniformRoughnessMap, m_uniformParallaxMap, m_uniformGlowMap);
	StaticModel->RenderModel();
	StaticModel->prevModel = model;
}

void Static_Object::DrawImportedObject(std::shared_ptr<Shader> shader, glm::mat4 prevProjView, glm::mat4 model, glm::vec3 position, glm::vec3 scale)
{
	SetUniformLocations(shader);

	model = glm::translate(model, position);
	//model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));  //if you put the rotate at the last place(i.e on the top) it will have a bouncy effect
	//model = glm::rotate(model, 180.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, scale);
	glUniformMatrix4fv(m_uniformModel, 1, GL_FALSE, glm::value_ptr(model));

	m_prevPVM = prevProjView * StaticModel->prevModel;
	glUniformMatrix4fv(m_uniformPrevPVM, 1, GL_FALSE, glm::value_ptr(m_prevPVM));
	m_material->UseMaterial(m_uniformAlbedoMap, m_uniformMetallicMap, m_uniformNormalMap, m_uniformRoughnessMap, m_uniformParallaxMap, m_uniformGlowMap);
	StaticModel->RenderModel();
	StaticModel->prevModel = model;
	model = glm::mat4();
}
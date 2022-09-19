#include "pch.h"
#include "Static_Object.h"
#include "Static_Model.h"
#include "Static_Mesh.h"
#include "Material.h"
#include "Texture.h"
#include "Shader.h"
#include "Camera.h"
#include "DirectionalLight.h"
#include "SpotLight.h"
#include "PointLight.h"
#include "RenderingCommonValues.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

void Static_Object::SetUpNativeModelData(std::shared_ptr<Static_Mesh> mesh,
	std::string albedoPath,
	std::string metalPath,
	std::string roughPath,
	std::string normalPath,
	std::string parallaxPath,
	std::string glowPath)
{

	LoadTexture(m_albedoTexture, albedoPath, true);
	LoadTexture(m_metallicTexture, metalPath);
	LoadTexture(m_roughTexture, roughPath);
	LoadTexture(m_normalTexture, normalPath);
	LoadTexture(m_parallaxTexture, parallaxPath);
	LoadTexture(m_glowTexture, glowPath);
	m_staticMesh = mesh;
	m_material = std::make_unique<Material>(1, 6, 7, 11, 12, 13);
}

void Static_Object::SetUpImportedModelData(std::string modelPath)
{
	StaticModel->LoadModel(modelPath);
	m_material = std::make_unique<Material>(1, 6, 7, 11, 12, 13);
}

void Static_Object::LoadTexture(std::unique_ptr<Texture>& texture, std::string path, bool isSRGB)
{
	if (path != "")
	{
		texture = std::make_unique<Texture>(path, isSRGB);
		//ToDo check for alpha channel/ currently incorrect use of find
		if (path.find(".png" ||".gif" || ".tiff" || ".tga" || ".jp2" || ".jpx") != std::string::npos)
		{
			texture->LoadTextureWithAlpha();
		}
		else
		{
			texture->LoadTextureNoAlpha();
		}
	}
}

void Static_Object::SetUniformLocations(std::shared_ptr<Shader> shader, std::shared_ptr<Camera> camera)
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

	glUniformMatrix4fv(m_uniformProjection, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));
	glUniformMatrix4fv(m_uniformView, 1, GL_FALSE, glm::value_ptr(camera->CalculateViewMatrix()));
	glUniform3f(m_uniformEyePosition, camera->getCameraPosition().x, camera->getCameraPosition().y, camera->getCameraPosition().z);
	glUniform1f(m_uniformHeightScale, 0.02f);

	shader->SetDirectionalShadowMap(2);
	shader->SetIrradianceMap(8);
	shader->SetPrefilterMap(9);
	shader->SetBRDFLUT(10);
	shader->SetAOMap(14);
	shader->SetDepthMap(18);
}

void Static_Object::Translate(float x, float y, float z) {
	m_model = glm::translate(m_model, glm::vec3(x, y, z));
}

void Static_Object::Rotate(float angleInDegrees, float x, float y, float z) {
	m_model = glm::rotate(m_model, angleInDegrees * toRadians, glm::vec3(x, y, z));
}

glm::mat4* Static_Object::GetModelMatrixForPhysics()
{
	isRigidBody = true;
	return &m_model;
};

void Static_Object::Scale(float x, float y, float z) {
	m_model = glm::scale(m_model, glm::vec3(x, y, z));
}

void Static_Object::DrawNativeObject(std::shared_ptr<Shader> shader, std::shared_ptr<Camera> camera)
{
	SetUniformLocations(shader, camera);

	glUniformMatrix4fv(m_uniformModel, 1, GL_FALSE, glm::value_ptr(m_model));

	m_prevPVM = camera->GetPreviousProjectionViewMatrix() * m_staticMesh->PrevMesh;
	glUniformMatrix4fv(m_uniformPrevPVM, 1, GL_FALSE, glm::value_ptr(m_prevPVM));

	m_albedoTexture->UseTexture(albedoTexUnit);
	m_metallicTexture->UseTexture(metallicTexUnit);
	m_normalTexture->UseTexture(normalTexUnit);
	m_roughTexture->UseTexture(roughTexUnit);
	m_parallaxTexture->UseTexture(parallaxTexUnit);
	m_glowTexture->UseTexture(glowTexUnit);
	m_material->UseMaterial(m_uniformAlbedoMap, m_uniformMetallicMap, m_uniformNormalMap, m_uniformRoughnessMap, m_uniformParallaxMap, m_uniformGlowMap);
	
	m_staticMesh->RenderMesh();
	m_staticMesh->PrevMesh = m_model;
	if (!isRigidBody) { m_model = glm::mat4(1.0f); }
}

void Static_Object::DrawImportedObject(std::shared_ptr<Shader> shader,std::shared_ptr<Camera> camera)
{
	SetUniformLocations(shader, camera);

	glUniformMatrix4fv(m_uniformModel, 1, GL_FALSE, glm::value_ptr(m_model));

	m_prevPVM = camera->GetPreviousProjectionViewMatrix() * StaticModel->prevModel;
	glUniformMatrix4fv(m_uniformPrevPVM, 1, GL_FALSE, glm::value_ptr(m_prevPVM));		

	m_material->UseMaterial(m_uniformAlbedoMap, m_uniformMetallicMap, m_uniformNormalMap,
							m_uniformRoughnessMap, m_uniformParallaxMap, m_uniformGlowMap);
		
	StaticModel->RenderModel();
	StaticModel->prevModel = m_model;
	if (!isRigidBody) { m_model = glm::mat4(1.0f); }
}
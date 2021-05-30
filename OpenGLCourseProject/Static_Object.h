#ifndef  STATIC_OBJECT
#include <memory>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <String>

class Texture;
class Material;
class Static_Model;
class Static_Mesh;
class Shader;
class Camera;
class DirectionalLight;
class PointLight;
class SpotLight;

class Static_Object
{
public:
	Static_Object() = default;

	void SetUpNativeModelData(std::shared_ptr<Static_Mesh> mesh,
		std::string albedoPath = "",
		std::string metalPath = "",
		std::string roughPath = "",
		std::string normalPath = "",
		std::string parallaxPath = "",
		std::string glowPath = "");

	void DrawNativeObject(glm::mat4 projectionMatrix,
		glm::mat4 viewMatrix,
		int pointLightCount,
		int spotLightCount, glm::mat4 prevProjView,
						float terrainScaleFactor = 0.0f,
						float rotationAngle = 0.0f,
						glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
						glm::vec3 rotationAxis = glm::vec3(0.0f, 0.0f, 0.0f),
						glm::vec3 scale = glm::vec3(0.0f, 0.0f, 0.0f));

	void SetUpImportedModelData(std::string modelPath = "");

	void DrawImportedObject(std::shared_ptr<Shader> shader, glm::mat4 prevProjView, glm::mat4 model);
	void DrawImportedObject(std::shared_ptr<Shader> shader, glm::mat4 prevProjView, glm::mat4 model, glm::vec3 position, glm::vec3 scale);

	std::shared_ptr <Static_Model> StaticModel = std::make_shared<Static_Model>();

	~Static_Object() = default;
private:
	void LoadTexture(Texture* texture, std::string path, bool isSRGB = false);
	void SetUniformLocations(std::shared_ptr<Shader> shader);

	std::shared_ptr <Texture> m_albedoTexture = std::make_unique<Texture>();
	std::shared_ptr <Texture> m_metallicTexture = std::make_unique<Texture>();
	std::shared_ptr <Texture> m_roughTexture = std::make_unique<Texture>();
	std::shared_ptr <Texture> m_normalTexture = std::make_unique<Texture>();
	std::shared_ptr <Texture> m_parallaxTexture = std::make_unique<Texture>();
	std::shared_ptr <Texture> m_glowTexture = std::make_unique<Texture>();

	std::unique_ptr <Material> m_material = std::make_unique<Material>();

	std::shared_ptr <Static_Mesh> m_staticMesh = std::make_shared<Static_Mesh>();
	std::shared_ptr<Camera> m_camera = std::make_shared<Camera>();
	std::shared_ptr<DirectionalLight> m_mainLight = std::make_shared<DirectionalLight>();
	std::shared_ptr<SpotLight> m_firstSpotLight = std::make_shared<SpotLight>();
	std::shared_ptr<PointLight> m_firstPointLight = std::make_shared<PointLight>();

	GLuint m_uniformModel = 0, m_uniformProjection = 0, m_uniformView = 0, m_uniformPrevPVM = 0, m_uniformEyePosition = 0, m_uniformHeightScale = 0,
		m_uniformAlbedoMap = 0, m_uniformMetallicMap = 0, m_uniformNormalMap = 0, m_uniformRoughnessMap = 0, m_uniformParallaxMap = 0, m_uniformGlowMap = 0,
		m_uniformOmniLightPos = 0, m_uniformFarPlane = 0;

	glm::mat4 m_prevPVM = glm::mat4();
};
#endif STATIC_OBJECT// ! 


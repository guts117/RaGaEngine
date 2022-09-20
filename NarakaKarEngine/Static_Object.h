#ifndef  STATIC_OBJECT

#include "pch.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
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

			void SetUpImportedModelData(std::string modelPath = "");

			void Translate(float x, float y, float z);

			void Rotate(float angleInDegrees, float x, float y, float z);

			glm::mat4* GetModelMatrixForPhysics();

			void Scale(float x, float y, float z);

			void DrawNativeObject(std::shared_ptr<Shader> shader, std::shared_ptr<Camera> camera);

			void DrawImportedObject(std::shared_ptr<Shader> shader, std::shared_ptr<Camera> camera);

			~Static_Object() = default;
		private:
			void LoadTexture(std::unique_ptr<Texture>& texture, std::string path, bool isSRGB = false);
			void SetUniformLocations(std::shared_ptr<Shader> shader, std::shared_ptr<Camera> camera);

			std::shared_ptr <Static_Model> StaticModel = std::make_shared<Static_Model>();

			std::unique_ptr <Texture> m_albedoTexture = nullptr;
			std::unique_ptr <Texture> m_metallicTexture = nullptr;
			std::unique_ptr <Texture> m_roughTexture = nullptr;
			std::unique_ptr <Texture> m_normalTexture = nullptr;
			std::unique_ptr <Texture> m_parallaxTexture = nullptr;
			std::unique_ptr <Texture> m_glowTexture = nullptr;

			std::unique_ptr <Material> m_material = std::make_unique<Material>();

			std::shared_ptr <Static_Mesh> m_staticMesh = std::make_shared<Static_Mesh>();
			std::shared_ptr<Camera> m_camera = std::make_shared<Camera>();
			std::shared_ptr<DirectionalLight> m_mainLight = std::make_shared<DirectionalLight>();
			std::shared_ptr<SpotLight> m_firstSpotLight = std::make_shared<SpotLight>();
			std::shared_ptr<PointLight> m_firstPointLight = std::make_shared<PointLight>();

			GLuint m_uniformModel = -1, m_uniformProjection = -1, m_uniformView = -1, m_uniformPrevPVM = -1, m_uniformEyePosition = -1, m_uniformHeightScale = -1,
				m_uniformAlbedoMap = -1, m_uniformMetallicMap = -1, m_uniformNormalMap = -1, m_uniformRoughnessMap = -1, m_uniformParallaxMap = -1, m_uniformGlowMap = -1,
				m_uniformOmniLightPos = -1, m_uniformFarPlane = -1;

			glm::mat4 m_prevPVM = glm::mat4();
			glm::mat4 m_model = glm::mat4();
			bool isRigidBody = false;
		};
	}
}
#endif STATIC_OBJECT// ! 


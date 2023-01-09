#ifndef MODEL_SHADER
#define MODEL_SHADER

#include "Shader.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class Model_Shader :
			public Shader
		{
		public:
			Model_Shader() = default;

			void SetDirectionalShadowMaps(DirectionalLight* light, unsigned int i, GLuint textureUnit);
			void SetSkybox(GLuint txtureUnit);
			void SetIrradianceMap(GLuint textureUnit);
			void SetPrefilterMap(GLuint textureUnit);
			void SetBRDFLUT(GLuint textureUnit);
			void SetAOMap(GLuint textureUnit);
			void SetDepthMap(GLuint textureUnit);
			void SetDirectionalLightTransforms(const GLuint& cascadeIndex, glm::mat4* lTransform);
			void SetDirectionalLightTransform(glm::mat4 lTransform);
			void SetCascadeEndClipSpace(int i, float z);
			void SetLightMatrices(std::vector<glm::mat4> lightMatrices);

			~Model_Shader();

		private:
			GLuint uniformDirectionalLightTransforms[NUM_CASCADES] = { 0 };
			GLuint uniformCascadeEndClipSpace[NUM_CASCADES] = { 0 };
			struct {
				GLuint shadowMap;
			} uniformDirectionalShadowMaps[NUM_CASCADES];
			void CompileProgram();
		};
	}
}
#endif
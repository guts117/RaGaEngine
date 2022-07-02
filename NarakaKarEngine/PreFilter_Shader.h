#ifndef PREFILTER_SHADER
#define PREFILTER_SHADER

#include "Shader.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class PreFilter_Shader :
			public Shader
		{
		public:
			PreFilter_Shader() = default;
			void SetSkybox(GLuint textureUnit);
			void SetRoughness(float rough);
			~PreFilter_Shader();

		private:

			void CompileProgram();
			GLuint uniformRoughness = 0;
		};
	}
}
#endif
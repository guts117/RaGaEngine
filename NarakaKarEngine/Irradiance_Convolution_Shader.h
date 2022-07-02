#ifndef IRRADIANCE_CONVOLUTION_SHADER
#define IRRADIANCE_CONVOLUTION_SHADER

#include "Shader.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class Irradiance_Convolution_Shader :
			public Shader
		{
		public:
			Irradiance_Convolution_Shader() = default;
			void SetSkybox(GLuint textureUnit);
			~Irradiance_Convolution_Shader();

		private:

			void CompileProgram();
		};
	}
}
#endif
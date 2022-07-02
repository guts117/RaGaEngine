#ifndef EQUIRECTANGULAR_TO_CUBEMAP_SHADER
#define EQUIRECTANGULAR_TO_CUBEMAP_SHADER

#include "Shader.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class Equirectangular_to_CubeMap_Shader :
			public Shader
		{
		public:
			Equirectangular_to_CubeMap_Shader() = default;
			void SetTexture(GLuint textureUnit);
			~Equirectangular_to_CubeMap_Shader();

		private:

			void CompileProgram();
		};
	}
}
#endif
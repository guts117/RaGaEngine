#ifndef	SSAOBLUR_SHADER
#define SSAOBLUR_SHADER

#include "Shader.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class SSAOBlur_Shader :
			public Shader
		{
		public:
			SSAOBlur_Shader() = default;

			void SetTexture(GLuint textureUnit);
			~SSAOBlur_Shader();


		private:
			void CompileProgram();
		};
	}
}
#endif
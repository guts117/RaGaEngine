#ifndef BRDF_SHADER
#define BRDF_SHADER

#include "Shader.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class BRDF_Shader :
			public Shader
		{
		public:
			BRDF_Shader() = default;
			~BRDF_Shader();

		private:

			void CompileProgram();
		};
	}
}
#endif
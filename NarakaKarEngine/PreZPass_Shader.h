#ifndef PREZPASS_SHADER
#define PREZPASS_SHADER

#include "Shader.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class PreZPass_Shader :
			public Shader
		{
		public:
			PreZPass_Shader() = default;

			~PreZPass_Shader();

		private:
			void CompileProgram();
		};
	}
}
#endif
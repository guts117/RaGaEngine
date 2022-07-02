#ifndef COMPUTE_SHADER
#define COMPUTE_SHADER

#include "Shader.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class Compute_Shader :
			public Shader
		{
		public:
			Compute_Shader() = default;
			void Dispatch(unsigned int x, unsigned int y, unsigned int z) const;
			~Compute_Shader();

		private:
			void CompileProgram();
		};
	}
}
#endif
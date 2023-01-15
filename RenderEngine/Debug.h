#ifndef DEBUG
#define DEBUG

#include "render_pch.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class Debug
		{
		public:
			Debug() = default;
			void static DebugPrintReferenceTBN(std::string header, GLfloat* vertices, int offset, glm::vec3 nm);
			void static DebugPrintTBN(std::string header, GLfloat* vertices, int offsetN, int offsetT, int offsetB = -1);
		private:
			~Debug() = default;
		};
	}
}
#endif
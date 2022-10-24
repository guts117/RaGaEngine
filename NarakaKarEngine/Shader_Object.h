#ifndef SHADER_OBJECT
#define SHADER_OBJECT

#include "pch.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class Shader_Object
		{
		public:
			explicit Shader_Object() = delete;
			explicit Shader_Object(const std::vector<std::string>& shaderLocs);

			void ValidateShaderObject() const;
			void UseShaderObject() const;
			const GLuint& GetShaderObjectID() const;

			void SetVariable(const std::string& varName, const std::any& value, const GLuint& index = 0, const std::string& memName = "") const;

			~Shader_Object();

		private:
			struct Impl;

			const Impl* Pimpl() const { return m_pImpl.get(); }
			Impl* Pimpl() { return m_pImpl.get(); }

			std::unique_ptr<Impl> m_pImpl;
		};
	}
}
#endif

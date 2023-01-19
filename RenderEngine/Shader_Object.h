#ifndef SHADER_OBJECT
#define SHADER_OBJECT

#include "render_pch.h"

namespace NarakaRenderEngine
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
			void DispatchShaderObject(const glm::uvec3& threadGroupCnt) const;
			const GLuint& GetShaderObjectProgramID() const;
			const GLuint SetTextureUnit(std::string&& textureName);
			const GLuint SetTextureUnit(std::string&& varName, const GLuint& index, std::string&& texName);
			const GLuint GetTextureUnit() const;
			const void ResetTextureUnit(GLuint&& resetToUnit);

			void SetVariable(std::string&& varName, const std::any& value, const GLuint& index = 0, std::string&& memName = "") const;

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

#ifndef SHADER_OBJECT
#define SHADER_OBJECT

#include "render_pch.h"
#include "ForwardDeclaredPimpl.h"

namespace NarakaRenderEngine
{
	namespace RenderEngine
	{
		class Shader_Object
		{
		public:
			explicit Shader_Object() = delete;
			explicit Shader_Object(std::vector<std::string>&& shaderLocs);

			Shader_Object(Shader_Object&& rhs) noexcept;
			Shader_Object& operator=(Shader_Object&& rhs) noexcept;

			Shader_Object(const Shader_Object& rhs) noexcept = delete;
			Shader_Object& operator=(const Shader_Object& rhs) noexcept = delete;

			void ValidateShaderObject() const;
			void UseShaderObject() const;
			void DispatchShaderObject(const glm::uvec3& threadGroupCnt) const;
			const GLuint& GetShaderObjectProgramID() const;
			void SetTextureUnit(std::string&& textureName);
			void SetTextureUnit(std::string&& varName, const GLuint& index, std::string&& texName);
			const GLuint GetTextureUnit() const;
			void ResetTextureUnit(GLuint resetToUnit);

			void SetVariable(std::string&& varName, const std::any& value, const GLuint& index = 0, std::string&& memName = "") const;

			~Shader_Object() noexcept;

		private:
			struct Impl;

			const Impl& Pimpl() const { return m_pImpl.Get(); }
			Impl& Pimpl() { return m_pImpl.Get(); }

#ifdef NDEBUG //size of vector<> is different between debug(32) and release(24)
			ForwardDeclaredPimpl<Impl, alignof(void*) * 8, alignof(void*)> m_pImpl;
#else
			ForwardDeclaredPimpl<Impl, alignof(void*) * 10, alignof(void*)> m_pImpl;
#endif;
		};
	}
}
#endif

#ifndef SHADER_OBJECT
#define SHADER_OBJECT

#include "pch.h"
#include <any>

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		struct SLStructMember
		{
			std::string VarType;
			std::string VarName;
			GLint VarLocation;
		};

		struct SLStructArr
		{
			std::vector<SLStructMember> StructMemArray;
		};

		struct SLDataTypeArr
		{
			std::vector<GLint> VarLocArray;
		};

		//ToDo: Serialize this
		struct ShaderInputVariable
		{
			std::string VarType;
			std::string VarName;
			std::any VarData;
		};

		class Shader_Object
		{
		public:
			explicit Shader_Object() = delete;
			explicit Shader_Object(const std::vector<std::string>& shaderLocs);

			void ValidateShaderObject() const;
			void UseShaderObject() const;
			const GLuint& GetShaderObjectID() const;

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

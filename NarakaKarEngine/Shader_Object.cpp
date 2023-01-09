#include "pch.h"
#include "Shader_Object.h"
#include "RenderingCommonValues.h"
#include <sstream>
#include <regex>
#include <iterator>
#include "exprtk.hpp"

using namespace NarakaKarEngine;
using namespace RenderEngine;

struct Shader_Object::Impl
{	
	struct ShaderCodeType
	{
		std::string Code;
		GLenum Type;
	};

	struct SLStructMember
	{
		std::string VarType;
		std::string VarName;
		GLint VarLocation;
	};

	struct SLStruct
	{
		std::vector<SLStructMember> StructMemArray;
		explicit operator bool() const
		{
			return StructMemArray.size() != 0;
		}
	};

	struct SLStructTypeArr
	{
		std::vector<SLStruct> StructArray;
		explicit operator bool() const
		{
			return StructArray.size() != 0;
		}
	};

	struct SLDataTypeArr
	{
		std::vector<GLint> VarLocArray;
		explicit operator bool() const
		{
			return VarLocArray.size() != 0;
		}
	};

	//ToDo: Serialize this
	struct ShaderInputVariable
	{
		std::string VarType;
		std::string VarName;
		std::any VarData;
	};

	std::unique_ptr<std::vector<std::string>> m_ShaderLocs;
	GLuint m_ShaderProgramID;
	std::unique_ptr<std::vector<ShaderInputVariable>> m_ShaderInputs;
	GLenum m_ShaderType;
	GLint m_TextureUnit;

	Impl(const std::vector<std::string>& shaderLocs)
		: m_ShaderLocs { std::make_unique<std::vector<std::string>>(shaderLocs) }
		, m_ShaderProgramID { 0 }
		, m_ShaderInputs{ std::make_unique<std::vector<ShaderInputVariable>>()}
		, m_ShaderType { 0 }
		, m_TextureUnit { 0 }
	{
		std::vector<ShaderCodeType> shaderCodes;
		for(auto locIndex = 0; locIndex < shaderLocs.size(); ++locIndex)
		{
			shaderCodes.push_back(ReadFile(shaderLocs[locIndex]));
		}
		
		CompileShader(shaderCodes);
	}

	void CompileShader(const std::vector<ShaderCodeType>& shaderCodesTypes)
	{
		m_ShaderProgramID = glCreateProgram();

		if (!m_ShaderProgramID) {
			printf("Error creating shader program!\n");
			glfwTerminate();
			return;
		}
		
		for (auto codeIndex = 0; codeIndex < shaderCodesTypes.size(); ++codeIndex)
		{
			AddShader(m_ShaderProgramID, shaderCodesTypes[codeIndex]);
		}

		CompileShaderProgram(shaderCodesTypes);
		glDeleteShader(m_ShaderProgramID);
	}

	//ToDo: Find a way to load shaders faster
	ShaderCodeType ReadFile(std::string fileLocation)
	{
		std::string content;
		std::ifstream fileStream(fileLocation, std::ios::in);
		GLenum type = 0;

		if (!fileStream.is_open()) {
			printf("Failed to read at %s! Shader file doesn't exist.", fileLocation.c_str());
			return ShaderCodeType{ "", type };
		}

		std::string line = "";
		while (!fileStream.eof()) {
			std::getline(fileStream, line);
			content.append(line + "\n");
		}
		fileStream.close();

		if (fileLocation.find(".vert") != std::string::npos)
		{
			type = GL_VERTEX_SHADER;
		}
		else if (fileLocation.find(".tessc") != std::string::npos)
		{
			type = GL_TESS_CONTROL_SHADER;
		}
		else if (fileLocation.find(".tesse") != std::string::npos)
		{
			type = GL_TESS_EVALUATION_SHADER;
		}
		else if (fileLocation.find(".geom") != std::string::npos)
		{
			type = GL_GEOMETRY_SHADER;
		}
		else if (fileLocation.find(".frag") != std::string::npos)
		{
			type = GL_FRAGMENT_SHADER;
		}
		else if (fileLocation.find(".comp") != std::string::npos) 
		{
			type = GL_COMPUTE_SHADER;
		}

		m_ShaderType = type;

		return ShaderCodeType{ content.c_str(), type };
	}


	//ToDo: Find a way to compile shaders faster
	void AddShader(GLuint theProgram, const ShaderCodeType& codeType)
	{
		auto shader = glCreateShader(codeType.Type);

		const GLchar* theCode[1];
		theCode[0] = codeType.Code.c_str();

		GLint codeLength[1];
		codeLength[0] = strlen(codeType.Code.c_str());

		glShaderSource(shader, 1, theCode, codeLength);
		glCompileShader(shader);

		GLint result = 0;
		GLchar eLog[1024] = { 0 };

		glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
		if (!result) {
			glGetShaderInfoLog(shader, sizeof(eLog), NULL, eLog);
			printf("Error compiling the %d program: '%s'\n", codeType.Type, eLog);
			return;
		}

		glAttachShader(theProgram, shader);
	}

	//ToDo: Rewrite and optimize after testing all the shaders
	void CompileShaderProgram(const std::vector<ShaderCodeType>& shaderCodesTypes)
	{
		GLint result = 0;
		GLchar eLog[1024] = { 0 };

		glLinkProgram(m_ShaderProgramID);
		glGetProgramiv(m_ShaderProgramID, GL_LINK_STATUS, &result);
		if (!result) {
			glGetProgramInfoLog(m_ShaderProgramID, sizeof(eLog), NULL, eLog);
			printf("Error linking shader program: '%s'\n", eLog);
			return;
		}

		struct slStructTemp
		{
			std::string type;
			std::string memberName;
		};

		struct slUniformTemp
		{
			std::string type;
			std::string name;
		};


		for (auto& shader : shaderCodesTypes) 
		{
			std::vector<std::string> defStructs;
			std::vector<slUniformTemp> defUniforms;
			std::map<std::string, int> defConsts;

			std::istringstream ss(shader.Code);
			std::string line;
			bool isStructStart = false;
			std::string structStr;

			while (std::getline(ss, line))
			{
				if (line.starts_with("//")) continue;

				std::string lastWord = "";
				std::string word = "";
				std::string varName = "";

				if (line.starts_with("const"))
				{
					std::istringstream css(line);

					while (css >> word)
					{
						auto pos = word.find("=");

						if (pos == std::string::npos)
						{
							if (varName == "")
							{
								lastWord = word;
								continue;
							}
						}
						else if (pos == 0)
						{
							varName = lastWord;
							if (word.size() == 1)
							{
								continue;
							}
						}
						else if (pos == (word.size() - 1))
						{
							varName = word;
							continue;
						}
						else
						{
							varName = word.substr(0, pos);
							word = word.substr(pos);
						}

						if (varName != "")
						{
							word.erase(remove(word.begin(), word.end(), '='), word.end());
							int val;

							if (word.find(".") == std::string::npos && (val = std::stoi(word)) > 0)
							{
								varName.erase(remove(varName.begin(), varName.end(), '='), word.end());
								word.erase(remove(word.begin(), word.end(), ';'), word.end());
								defConsts.emplace(varName, val);
							}
							break;
						}
					}
				}

				if (line.starts_with("struct"))
				{
					isStructStart = true;
				}
				if (isStructStart)
				{
					structStr.append(line);
					if (line.find("}") != std::string::npos)
					{
						defStructs.push_back(structStr);
						isStructStart = false;
						structStr = "";
					}
				}

				lastWord = "";
				word = "";
				std::string type = "";

				if (line.starts_with("uniform"))
				{
					std::istringstream uss(line);
					while (uss >> word)
					{
						if (lastWord == "uniform")
						{
							type = word;
							lastWord = "";
							continue;
						}
						if (type != "")
						{
							if (word.ends_with(";"))
							{
								word.erase(remove(word.begin(), word.end(), ';'), word.end());
								defUniforms.push_back(slUniformTemp{ type, lastWord + word });
							}
							else if (word == ";")
							{
								defUniforms.push_back(slUniformTemp{ type, lastWord });
							}
							else
							{
								lastWord += word;
							}
						}
						else
						{
							lastWord = word;
						}
					}
				}
			}

			std::map<std::string, std::vector<slStructTemp>> structTMaps;

			std::vector<char> ign_struct = { '{', '}', ';' };

			for (int i = 0; i < defStructs.size(); ++i)
			{
				std::string lastWord = "";
				std::string word;
				std::string lastStruct = "";
				std::stringstream iss(defStructs[i]);
				bool isType = true;


				while (iss >> word)
				{
					word.erase(remove_if(word.begin(), word.end(), [&](char c)
						{
							return std::find(ign_struct.begin(), ign_struct.end(), c) != ign_struct.end();
						}), word.end());

					if (lastWord == "struct")
					{
						structTMaps.emplace(word, std::vector<slStructTemp>());
						lastStruct = word;
						lastWord = "";
						continue;
					}

					if (lastWord != lastStruct)
					{
						if (!isType)
						{
							structTMaps[lastStruct].push_back(slStructTemp{ lastWord, word });
						}
						isType = !isType;
					}

					lastWord = word;
				}
			}

			for (auto var = 0; var < defUniforms.size(); ++var)
			{
				auto& type = defUniforms[var].type;
				auto& fullVarName = defUniforms[var].name;

				if (fullVarName.ends_with("]"))
				{
					auto pos = fullVarName.find("[");

					auto varName = fullVarName.substr(0, pos);
					auto arr = fullVarName.substr(pos + 1);

					arr.erase(remove(arr.begin(), arr.end(), ']'), arr.end());

					std::vector<std::tuple<int, char>> constVec = std::vector<std::tuple<int, char>>();

					auto hasArithmetic = false;

					for (auto& c : arr)
					{
						if (c != '_' && !std::isalnum(c))
						{
							hasArithmetic = true;
							break;
						}
					}

					int arrSize = 0;

					if (hasArithmetic)
					{
						for (const auto& key : defConsts) {
							arr = std::regex_replace(arr, std::regex(key.first), std::to_string(key.second)); // replace 'key.first' -> 'key.second'
						}

						exprtk::symbol_table<float> symbol_table;
						exprtk::expression<float> expression;
						exprtk::parser<float> parser;

						expression.register_symbol_table(symbol_table);

						parser.compile(arr, expression);
						arrSize = expression.value();
					}
					else
					{
						arrSize = defConsts.find(arr) != defConsts.end() ? defConsts[arr] : std::stoi(arr);
					}

					if (structTMaps.find(type) == structTMaps.end())
					{
						auto vecArr = std::vector<GLint>();
						for (auto i = 0; i < arrSize; ++i)
						{
							char locBuff[100] = { '\0' };

							snprintf(locBuff, sizeof(locBuff), (varName + "[%zd]").c_str(), i);
							vecArr.push_back(glGetUniformLocation(m_ShaderProgramID, locBuff));
						}
						m_ShaderInputs->push_back(ShaderInputVariable{ type, varName, SLDataTypeArr{vecArr} });
					}
					else
					{
						auto vecArr = std::vector<SLStruct>();
						for (auto i = 0; i < arrSize; ++i)
						{
							auto vec = std::vector<SLStructMember>();
							for (auto j = 0; j < structTMaps[type].size(); ++j)
							{
								char locBuff[100] = { '\0' };

								snprintf(locBuff, sizeof(locBuff), (varName + "[%zd]." + structTMaps[type][j].memberName).c_str(), i);
								vec.push_back(SLStructMember{ structTMaps[type][j].type, structTMaps[type][j].memberName, glGetUniformLocation(m_ShaderProgramID, locBuff) });
							}
							vecArr.push_back(SLStruct{ vec });
						}
						m_ShaderInputs->push_back(ShaderInputVariable{ type, varName, SLStructTypeArr{vecArr} });
					}
				}
				else
				{
					if (structTMaps.find(type) == structTMaps.end())
					{
						m_ShaderInputs->push_back(ShaderInputVariable{ type, fullVarName, glGetUniformLocation(m_ShaderProgramID, fullVarName.c_str()) });
					}
					else
					{
						auto vec = std::vector<SLStructMember>();
						for (auto i = 0; i < structTMaps[type].size(); ++i)
						{
							vec.push_back(SLStructMember{ structTMaps[type][i].type,structTMaps[type][i].memberName, glGetUniformLocation(m_ShaderProgramID, (fullVarName + "." + structTMaps[type][i].memberName).c_str()) });
						}
						m_ShaderInputs->push_back(ShaderInputVariable{ type, fullVarName, SLStruct{vec} });
					}
				}
			}
		}
	}

	void Validate() const
	{
		GLint result = 0;
		GLchar eLog[1024] = { 0 };

		glValidateProgram(m_ShaderProgramID);
		glGetProgramiv(m_ShaderProgramID, GL_VALIDATE_STATUS, &result);
		if (!result) {
			glGetShaderInfoLog(m_ShaderProgramID, sizeof(eLog), NULL, eLog);
			printf("Error validating shader program: '%s'\n", eLog);
			return;
		}
	}

	template <typename T>
	const T* CheckInputDataType(const std::any& data) const
	{
		return std::any_cast<T>(&data);
	}

	void SetShaderData(const std::string& type, const GLint& location, const std::any& value) const
	{
		if (type == "float")
		{
			if (auto val = CheckInputDataType<GLfloat>(value))
			{
				glUniform1f(location, *val);
			}
		}
		else if (type == "vec2")
		{
			if (auto val = CheckInputDataType<glm::vec2>(value))
			{
				glUniform2f(location, val->x, val->y);
			}
		}
		else if (type == "vec3")
		{
			if (auto val = CheckInputDataType<glm::vec3>(value))
			{
				glUniform3f(location, val->x, val->y, val->z);
			}
			else if(auto val = CheckInputDataType<std::tuple<GLint, glm::vec3*>>(value))
			{
				auto& size = std::get<0>(*val);
				auto& dataPtr = std::get<1>(*val);
				glUniform3fv(location, size, (const GLfloat*)dataPtr);
			}
		}
		else if (type == "vec4")
		{
			if (auto val = CheckInputDataType<glm::vec4>(value))
			{
				glUniform4f(location, val->x, val->y, val->z, val->w);
			}
		}
		else if (type == "bool" || type == "int" || type.find("sample") != std::string::npos)
		{
			if (auto val = CheckInputDataType<GLint>(value))
			{
				glUniform1i(location, *val);
			}
			else if(auto val = CheckInputDataType<bool>(value))
			{
				glUniform1i(location, *val);
			}
		}
		else if (type == "ivec2")
		{
			if (auto val = CheckInputDataType<glm::ivec2>(value))
			{
				glUniform2i(location, val->x, val->y);
			}
		}
		else if (type == "ivec3")
		{
			if (auto val = CheckInputDataType<glm::ivec3>(value))
			{
				glUniform3i(location, val->x, val->y, val->z);
			}
		}
		else if (type == "ivec4")
		{
			if (auto val = CheckInputDataType<glm::ivec4>(value))
			{
				glUniform4i(location, val->x, val->y, val->z, val->w);
			}
		}
		else if (type == "uint" || type == "bool")
		{
			if (auto val = CheckInputDataType<GLuint>(value))
			{
				glUniform1ui(location, *val);
			}
			else if (auto val = CheckInputDataType<bool>(value))
			{
				glUniform1ui(location, *val);
			}
		}
		else if (type == "uvec2")
		{
			if (auto val = CheckInputDataType<glm::uvec2>(value))
			{
				glUniform2ui(location, val->x, val->y);
			}
		}
		else if (type == "uvec3")
		{
			if (auto val = CheckInputDataType<glm::uvec3>(value))
			{
				glUniform3ui(location, val->x, val->y, val->z);
			}
		}
		else if (type == "uvec4")
		{
			if (auto val = CheckInputDataType<glm::uvec4>(value))
			{
				glUniform4ui(location, val->x, val->y, val->z, val->w);
			}
		}
		else if (type == "mat2")
		{
			if (auto val = CheckInputDataType<glm::mat2>(value))
			{
				glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(*val));
			}
		}
		else if (type == "mat3")
		{
			if (auto val = CheckInputDataType<glm::mat3>(value))
			{
				glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(*val));
			}
		}
		else if (type == "mat4")
		{
			if (auto val = CheckInputDataType<glm::mat4>(value))
			{
				glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(*val));
			}
			else if(auto val = CheckInputDataType<aiMatrix4x4>(value))
			{
				glUniformMatrix4fv(location, 1, GL_TRUE, (const GLfloat*)&val);
			}
		}
		else
		{
			printf("Error Parsing Shader: ToDo: Shader Data Type not supported yet. '%s'\n", type.c_str());
		}
	}

	void SetVariable(std::string&& varName, const std::any& value, const GLuint& index = 0, std::string&& memName = "") const
	{
		auto it = std::find_if(m_ShaderInputs->begin(), m_ShaderInputs->end(), [&](ShaderInputVariable& var) {return var.VarName == varName; });

		if (it != m_ShaderInputs->end())
		{
			if (auto data = CheckInputDataType<GLint>(it->VarData))
			{			
				SetShaderData(it->VarType, *data, value);
			}
			else if (auto data = CheckInputDataType<SLDataTypeArr>(it->VarData))
			{
				SetShaderData(it->VarType, data->VarLocArray[index], value);			
			}
			else if (auto data = CheckInputDataType<SLStruct>(it->VarData))
			{
				auto begin = data->StructMemArray.begin();
				auto end = data->StructMemArray.end();
				auto dat = std::find_if(begin, end, [&](const SLStructMember& m) {return m.VarName == memName; });

				if (dat != end) 
				{
					SetShaderData(dat->VarType, dat->VarLocation, value);
				}
				else
				{
					printf("Error Parsing Shader: Can't find member '%s'.'%s'\n", varName.c_str(), memName.c_str());
				}
			}
			else if (auto data = CheckInputDataType<SLStructTypeArr>(it->VarData))
			{	
				auto begin = data->StructArray[index].StructMemArray.begin();
				auto end = data->StructArray[index].StructMemArray.end();
				auto dat = std::find_if(begin, end, [&](const SLStructMember& m) {return m.VarName == memName; });

				if (dat != end)
				{
					SetShaderData(dat->VarType, dat->VarLocation, value);
				}
				else
				{
					printf("Error Parsing Shader: Can't find member '%s'['%d'].'%s'\n", varName.c_str(), index, memName.c_str());
				}
			}
			else		
			{
				printf("Error Parsing Shader: Error parsing Data '%s'\n", it->VarData.type().name());
			}
		}
		else
		{
			printf("Error Parsing Shader: Variable doesn't exist \n");
		}
	}

	~Impl() 
	{
		if (m_ShaderProgramID!= 0) {
			glDeleteProgram(m_ShaderProgramID);
			m_ShaderProgramID = 0;
		}
	}
};

Shader_Object::Shader_Object(const std::vector<std::string>& shaders) : m_pImpl{new Impl(shaders)} 
{
}

void Shader_Object::ValidateShaderObject() const
{
	Pimpl()->Validate();
}

const GLuint& Shader_Object::GetShaderObjectProgramID() const
{
	return Pimpl()->m_ShaderProgramID;
}

const GLuint Shader_Object::SetTextureUnit(std::string&& textureName)
{
	GLint& texUnit = Pimpl()->m_TextureUnit;
	SetVariable(std::move(textureName), texUnit);
	return texUnit++;
}

const GLuint Shader_Object::SetTextureUnit(std::string&& varName, const GLuint& index, std::string&& texName)
{
	GLint& texUnit = Pimpl()->m_TextureUnit;
	SetVariable(std::move(varName), texUnit, index, std::move(texName));
	return texUnit++;
}

const GLuint Shader_Object::GetTextureUnit() const
{
	return Pimpl()->m_TextureUnit;
}

const void Shader_Object::ResetTextureUnit(GLuint&& resetToUnit)
{
	Pimpl()->m_TextureUnit = resetToUnit;
}

void Shader_Object::UseShaderObject() const
{
	glUseProgram(Pimpl()->m_ShaderProgramID);
}

void Shader_Object::DispatchShaderObject(const glm::uvec3& threadGroupCnt) const
{
	if (Pimpl()->m_ShaderType == GL_COMPUTE_SHADER) 
	{
		glDispatchCompute(threadGroupCnt.x, threadGroupCnt.y, threadGroupCnt.z);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}
}

void Shader_Object::SetVariable(std::string&& varName, const std::any& value, const GLuint& index, std::string&& memName) const
{
	Pimpl()->SetVariable(std::move(varName), value, index, std::move(memName));
}

Shader_Object::~Shader_Object() = default;
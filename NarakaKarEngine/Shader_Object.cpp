#include "pch.h"
#include "Shader_Object.h"
#include "RenderingCommonValues.h"
#include <sstream>
#include <regex>
#include "exprtk.hpp"

using namespace NarakaKarEngine;
using namespace RenderEngine;

struct Shader_Object::Impl
{	
	struct ShaderCodeType
	{
		const char* Code;
		GLenum Type;
	};

	GLuint m_ShaderProgramID;
	std::unique_ptr<std::vector<ShaderInputVariable>> m_ShaderInputs;

	Impl(const std::vector<std::string>& shaderLocs)
		: m_ShaderProgramID { 0 }
		, m_ShaderInputs{ std::make_unique<std::vector<ShaderInputVariable>>()}
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

		return ShaderCodeType{ content.c_str(), type };
	}


	//ToDo: Find a way to compile shaders faster
	void AddShader(GLuint theProgram, const ShaderCodeType& codeType)
	{
		auto shader = glCreateShader(codeType.Type);

		const GLchar* theCode[1];
		theCode[0] = codeType.Code;

		GLint codeLength[1];
		codeLength[0] = strlen(codeType.Code);

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

							if (!word.contains(".") && (val = std::stoi(word)) > 0)
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
					if (line.contains("}"))
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
				auto type = defUniforms[var].type;
				auto fullVarName = defUniforms[var].name;

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

							snprintf(locBuff, sizeof(locBuff), (varName + "[% zd]").c_str(), i);
							vecArr.push_back(glGetUniformLocation(m_ShaderProgramID, locBuff));
						}
						m_ShaderInputs->push_back(ShaderInputVariable{ type, varName, SLDataTypeArr{vecArr} });
					}
					else
					{
						auto vecArr = std::vector<SLStructArr>();
						for (auto i = 0; i < arrSize; ++i)
						{
							auto vec = std::vector<SLStructMember>();
							for (auto j = 0; j < structTMaps[type].size(); ++j)
							{
								char locBuff[100] = { '\0' };

								snprintf(locBuff, sizeof(locBuff), (varName + "[% zd]." + structTMaps[type][j].memberName).c_str(), i);
								vec.push_back(SLStructMember{ structTMaps[type][j].type, structTMaps[type][j].memberName, glGetUniformLocation(m_ShaderProgramID, locBuff) });
							}
							vecArr.push_back(SLStructArr{ vec });
						}
						m_ShaderInputs->push_back(ShaderInputVariable{ type, varName, vecArr });
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
						m_ShaderInputs->push_back(ShaderInputVariable{ type, fullVarName, SLStructArr{vec} });
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

const GLuint& Shader_Object::GetShaderObjectID() const
{
	return Pimpl()->m_ShaderProgramID;
}

void Shader_Object::UseShaderObject() const
{
	glUseProgram(Pimpl()->m_ShaderProgramID);
}

Shader_Object::~Shader_Object() = default;
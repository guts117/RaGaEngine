#include "pch.h"
#include "Shader.h"
#include "RenderingCommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include <iterator>
#include <sstream>
#include <regex>
#include <any>
#include <ctype.h>
#include <iostream>
#include "exprtk.hpp"

using namespace NarakaKarEngine;
using namespace RenderEngine;

void Shader::CreateFromString(const char* vertexCode, const char* fragmentCode) {
	CompileShader(vertexCode, fragmentCode);
}

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

struct ShaderInputVariable
{
	std::string VarType;
	std::string VarName;
	std::any VarData;
};

void Shader::CreateFromFiles(const char* vertexLocation, const char* fragmentLocation, bool isYEs) {
	std::string vertexString = ReadFile(vertexLocation);
	std::string fragmentString = ReadFile(fragmentLocation);
	const char* vertexCode = vertexString.c_str();
	const char* fragmentCode = fragmentString.c_str();

	CompileShader(vertexCode, fragmentCode);

	if (isYEs) {
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
		
		std::vector<std::string> defStructs;
		std::vector<slUniformTemp> defUniforms;
		std::map<std::string, int> defConsts;

		std::istringstream ss(fragmentCode);
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

		for(int i = 0; i< defStructs.size(); ++i)
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

				if(lastWord == "struct")
				{
					structTMaps.emplace(word, std::vector<slStructTemp>());
					lastStruct = word;
					lastWord = "";
					continue;
				}

				if(lastWord != lastStruct)
				{
					if (!isType) 
					{
						structTMaps[lastStruct].push_back(slStructTemp{ lastWord, word});
					}
					isType = !isType;
				}

				lastWord = word;
			}
		}

		std::vector<ShaderInputVariable> shaderInputs;

		for(auto var = 0; var < defUniforms.size(); ++var)
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
						vecArr.push_back(glGetUniformLocation(shaderID, locBuff));
					}
					shaderInputs.push_back(ShaderInputVariable{ type, varName, SLDataTypeArr{vecArr} });
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
							vec.push_back(SLStructMember{ structTMaps[type][j].type, structTMaps[type][j].memberName, glGetUniformLocation(shaderID, locBuff) });
						}
						vecArr.push_back(SLStructArr{ vec });
					}
					shaderInputs.push_back(ShaderInputVariable{ type, varName, vecArr });
				}
			}
			else
			{		
				if (structTMaps.find(type) == structTMaps.end()) 
				{
					shaderInputs.push_back(ShaderInputVariable{ type, fullVarName, glGetUniformLocation(shaderID, fullVarName.c_str()) });
				}
				else
				{
					auto vec = std::vector<SLStructMember>();
					for (auto i = 0; i < structTMaps[type].size(); ++i) 
					{
						vec.push_back(SLStructMember{ structTMaps[type][i].type,structTMaps[type][i].memberName, glGetUniformLocation(shaderID, (fullVarName + "." + structTMaps[type][i].memberName).c_str())});
					}
					shaderInputs.push_back(ShaderInputVariable{ type, fullVarName, SLStructArr{vec} });
				}
			}
		}
	}
}

void Shader::CreateFromFiles(const char* vertexLocation, const char* geometryLocation, const char* fragmentLocation)
{
	std::string vertexString = ReadFile(vertexLocation);
	std::string geometryString = ReadFile(geometryLocation);
	std::string fragmentString = ReadFile(fragmentLocation);
	const char* vertexCode = vertexString.c_str();
	const char* geometryCode = geometryString.c_str();
	const char* fragmentCode = fragmentString.c_str();

	CompileShader(vertexCode, geometryCode, fragmentCode);
}

void Shader::CreateFromFiles(const char* vertexLocation, const char* tesscontrolLocation, const char* tessevaluationLocation, const char* fragmentLocation)
{
	std::string vertexString = ReadFile(vertexLocation);
	std::string tesscontrolString = ReadFile(tesscontrolLocation);
	std::string tessevaluationString = ReadFile(tessevaluationLocation);
	std::string fragmentString = ReadFile(fragmentLocation);
	const char* vertexCode = vertexString.c_str();
	const char* tesscontrolCode = tesscontrolString.c_str();
	const char* tessevaluationCode = tessevaluationString.c_str();
	const char* fragmentCode = fragmentString.c_str();

	CompileShader(vertexCode, tesscontrolCode, tessevaluationCode, fragmentCode);
}

void Shader::CreateFromFiles(const char* vertexLocation, const char* tesscontrolLocation, const char* tessevaluationLocation, const char* geometryLocation, const char* fragmentLocation)
{
	std::string vertexString = ReadFile(vertexLocation);
	std::string tesscontrolString = ReadFile(tesscontrolLocation);
	std::string tessevaluationString = ReadFile(tessevaluationLocation);
	std::string geometryString = ReadFile(geometryLocation);
	std::string fragmentString = ReadFile(fragmentLocation);
	const char* vertexCode = vertexString.c_str();
	const char* tesscontrolCode = tesscontrolString.c_str();
	const char* tessevaluationCode = tessevaluationString.c_str();
	const char* geometryCode = geometryString.c_str();
	const char* fragmentCode = fragmentString.c_str();

	CompileShader(vertexCode, tesscontrolCode, tessevaluationCode,geometryCode, fragmentCode);
}

void Shader::CreateFromFiles(const char* computeLocation) {
	std::string computeString = ReadFile(computeLocation);
	const char* computeCode = computeString.c_str();

	CompileShader(computeCode);
}

std::string Shader::ReadFile(const char* fileLocation) {
	std::string content;
	std::ifstream fileStream(fileLocation, std::ios::in);

	if (!fileStream.is_open()) {
		printf("Failed to read %s! File doesn't exist.", fileLocation);
		return "";
	}
	std::string line = "";
	while (!fileStream.eof()) {
		std::getline(fileStream, line);
		content.append(line + "\n");
	}
	fileStream.close();
	return content;
}

void Shader::AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType) 
{
	theShader = glCreateShader(shaderType);

	const GLchar* theCode[1];
	theCode[0] = shaderCode;

	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);

	glShaderSource(theShader, 1, theCode, codeLength);
	glCompileShader(theShader);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result) {
		glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
		printf("Error compiling the %d program: '%s'\n", shaderType, eLog);
		return;
	}

	glAttachShader(theProgram, theShader);
}

void Shader::CompileShader(const char* vertexCode, const char* fragmentCode) 
{
	shaderID = glCreateProgram();

	if (!shaderID) {
		printf("Error creating shader program!\n");
		glfwTerminate();
		return;
	}
	AddShader(shaderID, vertexCode, GL_VERTEX_SHADER);
	AddShader(shaderID, fragmentCode, GL_FRAGMENT_SHADER);

	CompileProgram();
	glDeleteShader(theShader);
}

void Shader::CompileShader(const char* vertexCode, const char* geometryCode, const char* fragmentCode)
{
	shaderID = glCreateProgram();

	if (!shaderID) {
		printf("Error creating shader program!\n");
		glfwTerminate();
		return;
	}
	AddShader(shaderID, vertexCode, GL_VERTEX_SHADER);
	AddShader(shaderID, geometryCode, GL_GEOMETRY_SHADER);
	AddShader(shaderID, fragmentCode, GL_FRAGMENT_SHADER);

	CompileProgram();
	glDeleteShader(theShader);
}

void Shader::CompileShader(const char* vertexCode, const char* tessellationControlCode, const char* tessellationEvaluationCode, const char* fragmentCode)
{
	shaderID = glCreateProgram();

	if (!shaderID) {
		printf("Error creating shader program!\n");
		glfwTerminate();
		return;
	}
	AddShader(shaderID, vertexCode, GL_VERTEX_SHADER);
	AddShader(shaderID, tessellationControlCode, GL_TESS_CONTROL_SHADER);
	AddShader(shaderID, tessellationEvaluationCode, GL_TESS_EVALUATION_SHADER);
	AddShader(shaderID, fragmentCode, GL_FRAGMENT_SHADER);

	CompileProgram();
	glDeleteShader(theShader);
}

void Shader::CompileShader(const char* vertexCode, const char* tessellationControlCode, const char* tessellationEvaluationCode, const char* geometryCode, const char* fragmentCode)
{
	shaderID = glCreateProgram();

	if (!shaderID) {
		printf("Error creating shader program!\n");
		glfwTerminate();
		return;
	}
	AddShader(shaderID, vertexCode, GL_VERTEX_SHADER);
	AddShader(shaderID, tessellationControlCode, GL_TESS_CONTROL_SHADER);
	AddShader(shaderID, tessellationEvaluationCode, GL_TESS_EVALUATION_SHADER);
	AddShader(shaderID, geometryCode, GL_GEOMETRY_SHADER);
	AddShader(shaderID, fragmentCode, GL_FRAGMENT_SHADER);

	CompileProgram();
	glDeleteShader(theShader);
}

void Shader::CompileShader(const char* computeCode)
{
	shaderID = glCreateProgram();

	if (!shaderID) {
		printf("Error creating shader program!\n");
		glfwTerminate();
		return;
	}
	AddShader(shaderID, computeCode, GL_COMPUTE_SHADER);

	CompileProgram();
	glDeleteShader(theShader);
}

void Shader::Validate()
{
	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glValidateProgram(shaderID);
	glGetProgramiv(shaderID, GL_VALIDATE_STATUS, &result);
	if (!result) {
		glGetShaderInfoLog(shaderID, sizeof(eLog), NULL, eLog);
		printf("Error validating program: '%s'\n", eLog);
		return;
	}
}

void Shader::CompileProgram()
{
}

GLuint Shader::GetProjectionLocation() {
	return uniformProjection;
}

GLuint Shader::GetModelLocation() {
	return uniformModel;
}
GLuint Shader::GetViewLocation() {
	return uniformView;
}
GLuint Shader::GetPrevPVMLocation()
{
	return uniformPrevPVM;
}

GLuint Shader::GetAmbientColorLocation() {
	return uniformDirectionalLight.uniformColor;
}


GLuint  Shader::GetDirectionLocation() {
	return uniformDirectionalLight.uniformDirection;
}

GLuint Shader::GetEyePositionLocation() {
	return uniformEyePosition;
}

GLuint Shader::GetHeightScaleLocation()
{
	return uniformHeightScale;
}

GLuint Shader::GetAlbedoLocation()
{
	return uniformAlbedo;
}

GLuint Shader::GetMetallicLocation()
{
	return uniformMetallic;
}

GLuint Shader::GetNormalLocation()
{
	return uniformNormal;
}

GLuint Shader::GetRoughnessLocation()
{
	return uniformRoughness;
}

GLuint Shader::GetParallaxLocation()
{
	return uniformParallax;
}

GLuint Shader::GetGlowLocation()
{
	return uniformGlow;
}

GLuint Shader::GetOmniLightPosLocation()
{
	return uniformOmniLightPos;
}

GLuint Shader::GetFarPlaneLocation()
{
	return uniformFarPlane;
}

GLuint Shader::GetShaderID()
{
	return shaderID;
}


void Shader::SetDirectionalLight(DirectionalLight* dLight) {

}

void Shader::SetPointLight(PointLight* pLight, unsigned int lightCount, unsigned int textureUnit, unsigned int offset) {

}

void Shader::SetSpotLight(SpotLight* sLight, unsigned int lightCount, unsigned int textureUnit, unsigned int offset) {


}

void Shader::SetTexture(GLuint textureUnit)
{

}
void Shader::SetSkybox(GLuint textureUnit)
{

}

void Shader::SetIrradianceMap(GLuint textureUnit)
{
}

void Shader::SetPrefilterMap(GLuint textureUnit)
{
}

void Shader::SetBRDFLUT(GLuint textureUnit)
{
}

void Shader::SetDirectionalShadowMap(GLuint textureUnit)
{

}

void Shader::SetAOMap(GLuint textureUnit)
{
}

void Shader::SetDepthMap(GLuint textureUnit)
{
}

void Shader::SetDirectionalLightTransform(glm::mat4* lTransform)
{

}

void Shader::SetLightMatrices(std::vector<glm::mat4> lightMatrices)
{

}

void Shader::UseShader() 
{
	glUseProgram(shaderID);
}

void Shader::ClearShader() {
	if (shaderID != 0) {
		glDeleteProgram(shaderID);
		shaderID = 0;
	}
}

Shader::~Shader() {
	ClearShader();
}
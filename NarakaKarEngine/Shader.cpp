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

using namespace NarakaKarEngine;
using namespace RenderEngine;

void Shader::CreateFromString(const char* vertexCode, const char* fragmentCode) {
	CompileShader(vertexCode, fragmentCode);
}

struct structT
{
	std::string type;
	std::string memberName;
};

struct ShaderInputVariable
{
	std::string VarType;
	std::string VarName;
	GLint VarLocID;
};

void Shader::CreateFromFiles(const char* vertexLocation, const char* fragmentLocation, bool isYEs) {
	std::string vertexString = ReadFile(vertexLocation);
	std::string fragmentString = ReadFile(fragmentLocation);
	const char* vertexCode = vertexString.c_str();
	const char* fragmentCode = fragmentString.c_str();

	CompileShader(vertexCode, fragmentCode);

	if (isYEs) {
		std::vector<std::string> defStructs;
		std::vector<std::string> defUniforms;
		std::vector<std::string> defConst;

		std::istringstream ss(fragmentString);
		std::string line;
		bool isStructStart = false;
		std::string structStr;

		while (std::getline(ss, line))
		{
			if (line.starts_with("//")) continue;

			if (line.starts_with("const") && line.ends_with(";"))
			{
				defConst.push_back(line);
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

			if (line.starts_with("uniform"))
			{
				defUniforms.push_back(line);
			}
		}
		
		std::map<std::string, std::vector<structT>> structTMaps;

		for(int i = 0; i< defStructs.size(); ++i)
		{
			std::string lastWord = "";
			std::string word;
			std::string lastStruct = "";
			std::stringstream iss(defStructs[i]);
			bool isType = true;
			while (iss >> word) 		
			{
				word.erase(remove(word.begin(), word.end(), '{'), word.end());
				word.erase(remove(word.begin(), word.end(), '}'), word.end());
				word.erase(remove(word.begin(), word.end(), ';'), word.end());

				if(lastWord == "struct")
				{
					structTMaps.emplace(word, std::vector<structT>());
					lastStruct = word;
					lastWord = "";
					continue;
				}

				if(lastWord != lastStruct)
				{
					if (!isType) 
					{
						structTMaps[lastStruct].push_back(structT{ lastWord, word});
					}
					isType = !isType;
				}

				lastWord = word;
			}
		}

		std::vector<ShaderInputVariable> shaderInputs;

		for(auto var = 0; var < defUniforms.size(); ++var)
		{
			std::string lastWord = "";
			std::string word;
			std::stringstream iss(defUniforms[var]);

			std::string type = "";	

			std::vector<std::string> arraySizeVec;

			int size = 1;

			if (word.contains("["))
			{
				const std::regex pattern("\\[(.*?)\\]");

				std::string s_size;
				for (std::sregex_iterator it = std::sregex_iterator(
					defUniforms[var].begin(), defUniforms[var].end(), pattern);
					it != std::sregex_iterator(); it++)
				{

					std::smatch match;
					match = *it;

					s_size = match.str(1);
					break;
				}

				//if (std::stoi(s_size))
				//{
				//		
				//}
				//std::stringstream sizess(s_size);
				//while (iss >> word)
				//{

				//}
				
			}

			while (iss >> word)
			{
				word.erase(remove(word.begin(), word.end(), ';'), word.end());

				if (lastWord == "uniform")
				{
					type = word;
					lastWord = "";
					continue;
				}
				if (type != "")
				{
					for (auto i = 0; i < size; ++i)
					{
						shaderInputs.push_back(ShaderInputVariable{ type, word + "#" + std::to_string(i), glGetUniformLocation(shaderID, word.c_str())});
					}
				}

				lastWord = word;
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
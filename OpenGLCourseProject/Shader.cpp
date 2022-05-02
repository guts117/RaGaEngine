#include "Shader.h"
#include "Model.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

void Shader::CreateFromString(const char* vertexCode, const char* fragmentCode) {
	CompileShader(vertexCode, fragmentCode);
}

void Shader::CreateFromFiles(const char* vertexLocation, const char* fragmentLocation) {
	std::string vertexString = ReadFile(vertexLocation);
	std::string fragmentString = ReadFile(fragmentLocation);
	const char* vertexCode = vertexString.c_str();
	const char* fragmentCode = fragmentString.c_str();

	CompileShader(vertexCode, fragmentCode);
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
#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <stdio.h>
#include <string>
#include <iostream>

class Debug
{
public:
	Debug() = default;
	void static DebugPrintReferenceTBN(std::string header, GLfloat* vertices, int offset, glm::vec3 nm);
	void static DebugPrintTBN(std::string header, GLfloat* vertices, int offsetN, int offsetT, int offsetB = -1);
private:
	~Debug() = default;
};

#include "pch.h"
#include "Debug.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

void Debug::DebugPrintReferenceTBN(std::string header, GLfloat* vertices, int offset, glm::vec3 nm) {

	//Ref
	glm::vec3 pos1(-1.0f, 0.0f, 1.0f);
	glm::vec3 pos2(-1.0f, 0.0f, -1.0f);
	glm::vec3 pos3(1.0f, 0.0f, -1.0f);
	glm::vec3 pos4(1.0f, 0.0f, 1.0f);

	// positions
	//glm::vec3 pos1(vertices[0], vertices[1], vertices[2]);
	//glm::vec3 pos2(vertices[offset + 0], vertices[offset + 1], vertices[offset + 2]);
	//glm::vec3 pos3(vertices[offset * 2 + 0], vertices[offset * 2 + 1], vertices[offset * 2 + 2]);
	//glm::vec3 pos4(vertices[offset * 3 + 0], vertices[offset * 3 + 1], vertices[offset * 3 + 2]);

	// texture coordinates
	glm::vec2 uv1(0.0f, 1.0f);
	glm::vec2 uv2(0.0f, 0.0f);
	glm::vec2 uv3(1.0f, 0.0f);
	glm::vec2 uv4(1.0f, 1.0f);

	// calculate tangent/bitangent vectors of both triangles
	glm::vec3 tangent1, bitangent1;
	glm::vec3 tangent2, bitangent2;
	// triangle 1
	// ----------
	glm::vec3 edge1 = pos2 - pos1;
	glm::vec3 edge2 = pos3 - pos1;
	glm::vec2 deltaUV1 = uv2 - uv1;
	glm::vec2 deltaUV2 = uv3 - uv1;

	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

	bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

	// triangle 2
	// ----------
	edge1 = pos3 - pos1;
	edge2 = pos4 - pos1;
	deltaUV1 = uv3 - uv1;
	deltaUV2 = uv4 - uv1;

	f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);


	bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);


	float quadVertices[] = {
		// positions            // normal         // texcoords  // tangent                          // bitangent
		pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
		pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
		pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

		pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
		pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
		pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
	};
	int vtxCnt = 0;
	std::cout << header << "\n";
	for (int i = 0; i < 84; i += 14)
	{
		std::cout << "	" << "Vertex:" << vtxCnt++ << std::endl; ;
		glm::vec3 normal = glm::vec3(quadVertices[3 + i], quadVertices[4 + i], quadVertices[5 + i]);
		glm::vec3 bitangent = glm::vec3(quadVertices[11 + i], quadVertices[12 + i], quadVertices[13 + i]);
		glm::vec3 tangent = glm::vec3(quadVertices[8 + i], quadVertices[9 + i], quadVertices[10 + i]);

		tangent = glm::normalize(tangent);
		bitangent = glm::normalize(bitangent);
		std::cout << "		" << "Normal: " << normal.x << "," << normal.y << "," << normal.z << "\n";
		std::cout << "		" << "Tangent: " << tangent.x << "," << tangent.y << "," << tangent.z << "\n";
		std::cout << "		" << "BiTangent: " << bitangent.x << "," << bitangent.y << "," << bitangent.z << "\n";
	}
	std::cout << std::endl;

}

void Debug::DebugPrintTBN(std::string header, GLfloat* vertices, int offsetN, int offsetT, int offsetB)
{
	int vtxCnt = 0;
	std::cout << header << "\n";
	for (int i = 0; i < 44; i += 11)
	{
		std::cout << "	" << "Vertex:" << vtxCnt++ << std::endl; ;

		glm::vec3 normal;
		glm::vec3 tangent;
		glm::vec3 bitangent;
		normal = glm::vec3(vertices[offsetN + i], vertices[offsetN + 1 + i], vertices[offsetN + 2 + i]);
		tangent = glm::vec3(vertices[offsetT + i], vertices[offsetT + 1 + i], vertices[offsetT + 2 + i]);

		tangent = glm::normalize(tangent);
		tangent = glm::normalize(tangent - glm::dot(tangent, normal) * normal);
		if (offsetB == -1)
		{
			bitangent = glm::cross(tangent, normal);
		}
		else
		{
			bitangent = glm::vec3(vertices[offsetB + i], vertices[offsetB + 1 + i], vertices[offsetB + 2 + i]);
		}
		std::cout << "		" << "Normal: " << normal.x << "," << normal.y << "," << normal.z << "\n";
		std::cout << "		" << "Tangent: " << tangent.x << "," << tangent.y << "," << tangent.z << "\n";
		std::cout << "		" << "BiTangent: " << bitangent.x << "," << bitangent.y << "," << bitangent.z << "\n";
	}
	std::cout << std::endl;
}
#ifndef UTILITY
#define UTILITY

#include "render_pch.h"

struct MathUtil
{
	static void CalcAverageNormals(std::vector<GLuint> indices, std::vector<std::vector<GLfloat>>& vertices, unsigned int normalOffset)
	{
		auto vLength = vertices[0].size();
		auto verticesCount = vLength * vertices.size();
		auto indicesCount = indices.size();

		for (size_t i = 0; i < indicesCount; i += 3) 
		{
			auto vertexId0 = indices[i];
			auto vertexId1 = indices[i + 1];
			auto vertexId2 = indices[i + 2];
			glm::vec3 v1(vertices[vertexId1][0] - vertices[vertexId0][0], vertices[vertexId1][1] - vertices[vertexId0][1], vertices[vertexId1][2] - vertices[vertexId0][2]);
			glm::vec3 v2(vertices[vertexId2][0] - vertices[vertexId0][0], vertices[vertexId2][1] - vertices[vertexId0][1], vertices[vertexId2][2] - vertices[vertexId0][2]);
			glm::vec3 normal = glm::cross(v1, v2);
			normal = glm::normalize(normal);

			auto normalId0 = normalOffset; 
			auto normalId1 = normalOffset + 1; 
			auto normalId2 = normalOffset + 2;

			vertices[vertexId0][normalId0] += normal.x; vertices[vertexId0][normalId1] += normal.y; vertices[vertexId0][normalId2] += normal.z;
			vertices[vertexId1][normalId0] += normal.x; vertices[vertexId1][normalId1] += normal.y; vertices[vertexId1][normalId2] += normal.z;
			vertices[vertexId2][normalId0] += normal.x; vertices[vertexId2][normalId1] += normal.y; vertices[vertexId2][normalId2] += normal.z;

		}

		for (auto& vertex : vertices) 
		{
			glm::vec3 vec(vertex[normalOffset + 0], vertex[normalOffset + 1], vertex[normalOffset + 2]);
			vec = glm::normalize(vec);
			vertex[normalOffset + 0] = vec.x; vertex[normalOffset + 1] = vec.y; vertex[normalOffset + 2] = vec.z;
		}
	}

	static void CalcAverageTangents(std::vector<GLuint> indices, std::vector<std::vector<GLfloat>>& vertices, unsigned int tangentOffset)
	{
		auto vLength = vertices[0].size();
		auto verticesCount = vLength * vertices.size();
		auto indicesCount = indices.size();

		for (size_t i = 0; i < indicesCount; i += 3) 
		{
			auto vertexId0 = indices[i];
			auto vertexId1 = indices[i + 1];
			auto vertexId2 = indices[i + 2] ;

			glm::vec3 Edge1(vertices[vertexId1][0] - vertices[vertexId0][0], vertices[vertexId1][1] - vertices[vertexId0][1], vertices[vertexId1][2] - vertices[vertexId0][2]);
			glm::vec3 Edge2(vertices[vertexId2][0] - vertices[vertexId0][0], vertices[vertexId2][1] - vertices[vertexId0][1], vertices[vertexId2][2] - vertices[vertexId0][2]);

			float DeltaU1 = vertices[vertexId1][3] - vertices[vertexId0][3];                //for the uv coordinates add 3
			float DeltaV1 = vertices[vertexId1][3 + 1] - vertices[vertexId0][3 + 1];
			float DeltaU2 = vertices[vertexId2][3] - vertices[vertexId0][3];
			float DeltaV2 = vertices[vertexId2][3 + 1] - vertices[vertexId0][3 + 1];

			float f = 1.0f / (DeltaU1 * DeltaV2 - DeltaU2 * DeltaV1);

			glm::vec3 Tangent, Bitangent;

			Tangent.x = f * (DeltaV2 * Edge1.x - DeltaV1 * Edge2.x);
			Tangent.y = f * (DeltaV2 * Edge1.y - DeltaV1 * Edge2.y);
			Tangent.z = f * (DeltaV2 * Edge1.z - DeltaV1 * Edge2.z);

			/*Bitangent.x = f * (-DeltaU2 * Edge1.x - DeltaU1 * Edge2.x);
			Bitangent.y = f * (-DeltaU2 * Edge1.y - DeltaU1 * Edge2.y);
			Bitangent.z = f * (-DeltaU2 * Edge1.z - DeltaU1 * Edge2.z);*/

			auto tangentId0 = tangentOffset;
			auto tangentId1 = tangentOffset + 1;
			auto tangentId2 = tangentOffset + 2;

			vertices[vertexId0][tangentId0] += Tangent.x; vertices[vertexId0][tangentId1] += Tangent.y; vertices[vertexId0][tangentId2] += Tangent.z;
			vertices[vertexId1][tangentId0] += Tangent.x; vertices[vertexId1][tangentId1] += Tangent.y; vertices[vertexId1][tangentId2] += Tangent.z;
			vertices[vertexId2][tangentId0] += Tangent.x; vertices[vertexId2][tangentId1] += Tangent.y; vertices[vertexId2][tangentId2] += Tangent.z;
		}

		for (auto& vertex : vertices)
		{
			glm::vec3 vec(vertex[tangentOffset + 0], vertex[tangentOffset + 1], vertex[tangentOffset + 2]);
			vec = glm::normalize(vec);
			vertex[tangentOffset + 0] = vec.x; vertex[tangentOffset + 1] = vec.y; vertex[tangentOffset + 2] = vec.z;
		}
	}

	static int Mini(int x, int y)
	{
		return x < y ? x : y;
	}

	static int Maxi(int x, int y)
	{
		return x > y ? x : y;
	}

	static int GetGreatestCommonDenominator(int a, int b)
	{
		int result = std::min(a, b); // Find Minimum of a nd b
		while (result > 0) {
			if (a % result == 0 && b % result == 0) {
				break;
			}
			result--;
		}
		return result; // return gcd of a nd b
	}

	GLFWmonitor* Get_current_monitor(GLFWwindow* window)
	{
		int nmonitors, i;
		int wx, wy, ww, wh;
		int mx, my, mw, mh;
		int overlap, bestoverlap;
		GLFWmonitor* bestmonitor;
		GLFWmonitor** monitors;
		const GLFWvidmode* mode;

		bestoverlap = 0;
		bestmonitor = NULL;

		glfwGetWindowPos(window, &wx, &wy);
		glfwGetWindowSize(window, &ww, &wh);
		monitors = glfwGetMonitors(&nmonitors);

		for (i = 0; i < nmonitors; i++) {
			mode = glfwGetVideoMode(monitors[i]);
			glfwGetMonitorPos(monitors[i], &mx, &my);
			mw = mode->width;
			mh = mode->height;

			overlap =
				Maxi(0, Mini(wx + ww, mx + mw) - Maxi(wx, mx)) *
				Maxi(0, Mini(wy + wh, my + mh) - Maxi(wy, my));

			if (bestoverlap < overlap) {
				bestoverlap = overlap;
				bestmonitor = monitors[i];
			}
		}

		return bestmonitor;
	}
};

#endif
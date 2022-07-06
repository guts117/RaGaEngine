#ifndef UTILITY
#define UTILITY

#include "pch.h"

struct MathUtil
{
	static void CalcAverageNormals(unsigned int* indices, unsigned int indicesCount,
		GLfloat* vertices, unsigned int verticesCount,
		unsigned int vLength, unsigned int normalOffset)
	{
		for (size_t i = 0; i < indicesCount; i += 3) {
			unsigned int in0 = indices[i] * vLength;
			unsigned int in1 = indices[i + 1] * vLength;
			unsigned int in2 = indices[i + 2] * vLength;
			glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
			glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
			glm::vec3 normal = glm::cross(v1, v2);
			normal = glm::normalize(normal);

			in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;

			vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
			vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
			vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;

		}

		for (size_t i = 0; i < verticesCount / vLength; i++) {
			unsigned int nOffset = i * vLength + normalOffset;
			glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
			vec = glm::normalize(vec);
			vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
		}
	}

	static void CalcAverageTangents(unsigned int* indices, unsigned int indicesCount,
		GLfloat* vertices, unsigned int verticesCount,
		unsigned int vLength, unsigned int tangentOffset)
	{
		for (size_t i = 0; i < indicesCount; i += 3) {
			unsigned int in0 = indices[i] * vLength;
			unsigned int in1 = indices[i + 1] * vLength;
			unsigned int in2 = indices[i + 2] * vLength;

			glm::vec3 Edge1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
			glm::vec3 Edge2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);

			float DeltaU1 = vertices[in1 + 3] - vertices[in0 + 3];                //for the uv coordinates add 3
			float DeltaV1 = vertices[in1 + 3 + 1] - vertices[in0 + 3 + 1];
			float DeltaU2 = vertices[in2 + 3] - vertices[in0 + 3];
			float DeltaV2 = vertices[in2 + 3 + 1] - vertices[in0 + 3 + 1];

			float f = 1.0f / (DeltaU1 * DeltaV2 - DeltaU2 * DeltaV1);

			glm::vec3 Tangent, Bitangent;

			Tangent.x = f * (DeltaV2 * Edge1.x - DeltaV1 * Edge2.x);
			Tangent.y = f * (DeltaV2 * Edge1.y - DeltaV1 * Edge2.y);
			Tangent.z = f * (DeltaV2 * Edge1.z - DeltaV1 * Edge2.z);

			/*Bitangent.x = f * (-DeltaU2 * Edge1.x - DeltaU1 * Edge2.x);
			Bitangent.y = f * (-DeltaU2 * Edge1.y - DeltaU1 * Edge2.y);
			Bitangent.z = f * (-DeltaU2 * Edge1.z - DeltaU1 * Edge2.z);*/

			in0 += tangentOffset; in1 += tangentOffset; in2 += tangentOffset;
			vertices[in0] += Tangent.x; vertices[in0 + 1] += Tangent.y; vertices[in0 + 2] += Tangent.z;
			vertices[in1] += Tangent.x; vertices[in1 + 1] += Tangent.y; vertices[in1 + 2] += Tangent.z;
			vertices[in2] += Tangent.x; vertices[in2 + 1] += Tangent.y; vertices[in2 + 2] += Tangent.z;
		}

		for (size_t i = 0; i < verticesCount / vLength; i++) {
			unsigned int nOffset = i * vLength + tangentOffset;
			glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
			vec = glm::normalize(vec);
			vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
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
#include "Animated_Mesh.h"

#include <iostream>
Animated_Mesh::Animated_Mesh() :Mesh()
{
}

void Animated_Mesh::CreateAnimatedMesh(GLfloat Vertices[], unsigned int Indices[], GLuint numOfVertices, GLuint numOfIndices, std::vector<VertexBoneData>& Bones)
{
	indexCount = numOfIndices;
	vertexCount = numOfVertices;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * numOfIndices, Indices, GL_STATIC_DRAW);


	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices[0]) * numOfVertices, Vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertices[0]) * 11, 0);    //vertex position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertices[0]) * 11, (void*)(sizeof(Vertices[0]) * 3));  //texture co-ordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertices[0]) * 11, (void*)(sizeof(Vertices[0]) * 5));  //normal co-ordinates
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertices[0]) * 11, (void*)(sizeof(Vertices[0]) * 8));  //tangent co-ordinates
	glEnableVertexAttribArray(3);

	glGenBuffers(1, &VBO_bones);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_bones);
	glBufferData(GL_ARRAY_BUFFER, Bones.size() * sizeof(Bones[0]), &Bones[0], GL_STATIC_DRAW);

	glVertexAttribIPointer(4, 4, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)16);
	glEnableVertexAttribArray(5);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


void Animated_Mesh::RenderAnimatedMesh() {
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Animated_Mesh::ClearMesh() {
	if (VBO != 0) {
		glDeleteBuffers(1, &VBO);
		VBO = 0;
	}
	if (VBO_bones != 0)
	{
		glDeleteBuffers(1, &VBO_bones);
		VBO_bones = 0;
	}
	if (VAO != 0) {
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}
	if (IBO != 0) {
		glDeleteBuffers(1, &IBO);
		IBO = 0;
	}

	indexCount = 0;
	vertexCount = 0;
}
Animated_Mesh::~Animated_Mesh() {
	ClearMesh();
}
#include "pch.h"
#include "Static_Mesh.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

void Static_Mesh::CreateMeshWithNormal(GLfloat Vertices[], unsigned int Indices[], GLuint numOfVertices, GLuint numOfIndices) 
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

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertices[0]) * 8, 0);    //vertex position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertices[0]) * 8, (void*)(sizeof(Vertices[0]) * 3));  //texture co-ordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertices[0]) * 8, (void*)(sizeof(Vertices[0]) * 5));  //normal co-ordinates
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Static_Mesh::CreateMeshWithTangentNormal(GLfloat Vertices[], unsigned int Indices[], GLuint numOfVertices, GLuint numOfIndices) 
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

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Static_Mesh::RenderMesh() 
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Static_Mesh::RenderTessellatedMesh() 
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glDrawElements(GL_PATCHES, indexCount, GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Static_Mesh::RenderQuad() 
{
	if (VAO == 0)
	{
		float quadVertices[] = {
			// positions			// texture Coords
			-1.0f,  1.0f, 0.0f,		 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f,		 0.0f, 0.0f,
			1.0f,  1.0f, 0.0f,		 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f,		 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void Static_Mesh::RenderCube() 
{
	if (VAO == 0) 
	{
		unsigned int Indices[] = {
			//front
			0,1,2,
			2,1,3,
			//right
			2,3,5,
			5,3,7,
			//back
			5,7,4,
			4,7,6,
			//left
			4,6,0,
			0,6,1,
			//top
			4,0,5,
			5,0,2,
			//bottom
			1,6,3,
			3,6,7
		};

		float Vertices[] = {
			-1.0f, 1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			-1.0f, -1.0f, -1.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, 1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,

			-1.0f, 1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			-1.0f, -1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f

		};

		CreateMeshWithNormal(Vertices, Indices, 64, 36);
	}
	RenderMesh();
}


void Static_Mesh::RenderCCWCube()
{
	if (VAO == 0)
	{
		unsigned int Indices[] = {
			//back face (CCW)
			3, 1, 0, 
			0, 2, 3,
			//front face (CCW)
			6, 7, 5, 
			5, 4, 6,
			//left face (CCW)
			1, 6, 4, 
			4, 0, 1,
			//right face (CCW)
			7, 3, 2, 
			2, 5, 7,
			//bottom face (CCW)
			1, 3, 7, 
			7, 6, 1,
			// top face (CCW)
			4, 5, 2, 
			2, 0, 4
		};

		float Vertices[] = {
			-0.5f, 0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,	//0 -+-
			-0.5f, -0.5f, -0.5f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,	//1 ---
			0.5f, 0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,	//2 ++-
			0.5f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,	//3 +--

			-0.5f, 0.5f, 0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,	//4 -++
			0.5f, 0.5f, 0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,	//5	+++
			-0.5f, -0.5f, 0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,	//6 --+
			0.5f, -0.5f, 0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f	//7 +-+

		};

		CreateMeshWithNormal(Vertices, Indices, 64, 36);
	}
	RenderMesh();
}

//CCW==============================================================================================
/*
		// back face (CCW winding)
		0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // bottom-left
		-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, // bottom-right
		-0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right
		-0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right
		0.5f, 0.5f, -0.5f, 0.0f, 1.0f, // top-left
		0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // bottom-left
		// front face (CCW winding)
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, // bottom-right
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, // top-right
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, // top-right
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, // top-left
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
		// left face (CCW)
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // bottom-left
		-0.5f, -0.5f, 0.5f, 1.0f, 0.0f, // bottom-right
		-0.5f, 0.5f, 0.5f, 1.0f, 1.0f, // top-right
		-0.5f, 0.5f, 0.5f, 1.0f, 1.0f, // top-right
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, // top-left
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // bottom-left
		// right face (CCW)
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, // bottom-right
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, // top-left
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
		// bottom face (CCW)      
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // bottom-left
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, // bottom-right
		0.5f, -0.5f, 0.5f, 1.0f, 1.0f, // top-right
		0.5f, -0.5f, 0.5f, 1.0f, 1.0f, // top-right
		-0.5f, -0.5f, 0.5f, 0.0f, 1.0f, // top-left
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // bottom-left
		// top face (CCW)
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // bottom-right
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, // top-left
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, // bottom-left

\*/

//CW=======================================================================
/*
		// Back face
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // Bottom-left
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, // bottom-right    
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right              
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, // top-left
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // bottom-left                
		// Front face
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, // top-right
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, // bottom-right        
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, // top-right
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, // top-left        
		// Left face
		-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // top-right
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-left
		-0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-left       
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-left
		-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // top-right
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-right
		// Right face
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // top-left
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right      
		0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-right          
		0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-right
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // top-left
		// Bottom face          
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // top-right
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, // bottom-left
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, // top-left        
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, // bottom-left
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // top-right
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-right
		// Top face
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, // top-left
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // bottom-right                 
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // bottom-right
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, // bottom-left  
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f  // top-left    
*/
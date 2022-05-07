#pragma once
#include <cstdio>
#include <vector>
#include <string>
#include <map>

#include<GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Static_Mesh.h"
#include "Animated_Mesh.h"
#include "Texture.h"

class Static_Mesh;
class Animated_Mesh;
class VertexBoneData;

struct BoneMatrix
{
	aiMatrix4x4 offset_matrix;
	aiMatrix4x4 final_world_transform;
};


class Model
{
public:
	Model() = default;

	virtual void LoadModel(const std::string& fileName);
	virtual void RenderModel();
	virtual ~Model() = 0;

	Model(const Model&) = delete;
	Model& operator = (Model&) = delete;
	Model(const Model&&) = delete;
	Model& operator= (Model&&) = delete;

	glm::mat4 prevModel = glm::mat4(1.0f);

protected:
	virtual void LoadNode(aiNode* node, const aiScene* scene);
	virtual void LoadMesh(aiMesh* mesh, const aiScene* scene);
	virtual void LoadMaterials(const aiScene* scene);
	void ClearModel();

	std::vector<Static_Mesh*> static_MeshList;
	std::vector<Animated_Mesh*> anim_MeshList;
	std::vector<Texture*> textureList;
	std::vector<Texture*> metalTextureList;
	std::vector<Texture*> normalTextureList;
	std::vector<Texture*> roughTextureList;
	std::vector<Texture*> parallaxTextureList;
	std::vector<Texture*> glowTextureList;
	Texture* billboardTexture = nullptr;
	std::vector<unsigned int> meshToTex;

	const aiScene *scene = nullptr;
};


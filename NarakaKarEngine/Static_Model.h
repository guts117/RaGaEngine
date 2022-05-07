#pragma once
#include "Model.h"
class Static_Model :
	public Model
{

public:
	Static_Model() = default;
	void LoadModel(const std::string& fileName) final;
	void RenderModel() final;
	~Static_Model();
private:
	void ClearModel();
	void LoadMesh(aiMesh* mesh, const aiScene* scene) final;
	void LoadNode(aiNode* node, const aiScene* scene) final;
	void LoadMaterials(const aiScene* scene) final;
};


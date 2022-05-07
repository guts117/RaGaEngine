#pragma once
#include "Model.h"

class Animated_Model :public Model
{
public:
	Animated_Model() = default;

	void initShaders(GLuint shaderID);
	void RenderModel()final;
	void LoadModel(const std::string& fileName)final;
	//void RenderModel() final;
	void showNodeName(aiNode* node);
	glm::mat4 aiToGlm(aiMatrix4x4 ai_matr);
	aiQuaternion nlerp(aiQuaternion a, aiQuaternion b, float blend);

	~Animated_Model();

	void boneTransform(double time_in_sec, std::vector<aiMatrix4x4>& transforms);
	std::vector<aiMatrix4x4> transforms = std::vector<aiMatrix4x4>();
	std::vector<aiMatrix4x4> prevTransforms = std::vector<aiMatrix4x4>();
private:

	void ClearModel();

	aiMatrix4x4 m_GlobalInverseTransform;
	std::string directory;

	std::map<std::string, unsigned int> m_BoneMapping;
	unsigned int m_NumBones = 0;
	GLuint m_bone_location[MAX_BONES]{0};
	GLuint m_prev_bone_location[MAX_BONES]{ 0 };
	std::vector<BoneMatrix> m_bone_matrices;
	float ticks_per_second = 0.0f;

	void LoadNode(aiNode* node, const aiScene* scene) final;
	void LoadMesh(aiMesh* mesh, const aiScene* scene)final;
	void LoadMaterials(const aiScene* scene) final;

	unsigned int findPosition(float p_animation_time, const aiNodeAnim* p_node_anim);
	unsigned int findRotation(float p_animation_time, const aiNodeAnim* p_node_anim);
	unsigned int findScaling(float p_animation_time, const aiNodeAnim* p_node_anim);
	const aiNodeAnim* findNodeAnim(const aiAnimation* p_animation, const std::string p_node_name);

	aiVector3D calcInterpolatedPosition(float p_animation_time, const aiNodeAnim* p_node_anim);
	aiQuaternion calcInterpolatedRotation(float p_animation_time, const aiNodeAnim* p_node_anim);
	aiVector3D calcInterpolatedScaling(float p_animation_time, const aiNodeAnim* p_node_anim);


	void readNodeHierarchy(float p_animation_time, const aiNode* p_node, const aiMatrix4x4 parent_transform);


	glm::quat rotate_head_xz = glm::quat(cos(glm::radians(0.0f)), sin(glm::radians(0.0f)) * glm::vec3(1.0f, 0.0f, 0.0f));
};

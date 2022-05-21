#include "Animated_Model.h"
#include<iostream>


void Animated_Model::initShaders(GLuint shaderID)
{
	
	for (size_t i = 0; i < MAX_BONES; i++)
	{
		char locBuff[100] = { '\0' };

		snprintf(locBuff, sizeof(locBuff), "gBones[%d]", i);
		m_bone_location[i] = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "gPrevBones[%d]", i);
		m_prev_bone_location[i] = glGetUniformLocation(shaderID, locBuff);
	}
}


void Animated_Model::RenderModel() {

	boneTransform((double)glfwGetTime(), transforms);
	for (unsigned int i = 0; i < transforms.size(); i++) // move all matrices for actual model position to shader
	{
		glUniformMatrix4fv(m_bone_location[i], 1, GL_TRUE, (const GLfloat*)&transforms[i]);
		glUniformMatrix4fv(m_prev_bone_location[i], 1, GL_TRUE, (const GLfloat*)&prevTransforms[i]);
	}

	for (size_t i = 0; i < anim_MeshList.size(); i++) {
		unsigned int materialIndex = meshToTex[i];

		if (materialIndex < textureList.size() && textureList[materialIndex]) {
			textureList[materialIndex]->UseTexture(0);
			metalTextureList[materialIndex]->UseTexture(5);
			normalTextureList[materialIndex]->UseTexture(6);
			roughTextureList[materialIndex]->UseTexture(10);
			parallaxTextureList[materialIndex]->UseTexture(11);
			glowTextureList[materialIndex]->UseTexture(12);
		}

		anim_MeshList[i]->RenderMesh();
	}

	prevTransforms = transforms;
}

void Animated_Model::LoadModel(const std::string& fileName)
{
	static Assimp::Importer importer;
	scene = new aiScene(*importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_OptimizeMeshes | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace));
	importer.GetOrphanedScene();
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "error assimp : " << importer.GetErrorString() << std::endl;
		return;
	}

	m_GlobalInverseTransform = scene->mRootNode->mTransformation;
	m_GlobalInverseTransform.Inverse();

	if (scene->mAnimations[0]->mTicksPerSecond != 0.0)
	{
		ticks_per_second = static_cast<float> (scene->mAnimations[0]->mTicksPerSecond);
	}
	else
	{
		ticks_per_second = 25.0f;
	}
	directory = fileName.substr(0, fileName.find_last_of('/'));

	std::cout << "scene->HasAnimations() 1: " << scene->HasAnimations() << std::endl;
	std::cout << "scene->mNumMeshes 1: " << scene->mNumMeshes << std::endl;
	std::cout << "scene->mAnimations[0]->mNumChannels 1: " << scene->mAnimations[0]->mNumChannels << std::endl;
	std::cout << "scene->mAnimations[0]->mDuration 1: " << scene->mAnimations[0]->mDuration << std::endl;
	std::cout << "scene->mAnimations[0]->mTicksPerSecond 1: " << scene->mAnimations[0]->mTicksPerSecond << std::endl << std::endl;

	std::cout << "		name nodes : " << std::endl;
	showNodeName(scene->mRootNode);
	std::cout << std::endl;

	std::cout << "		name bones : " << std::endl;
	LoadNode(scene->mRootNode, scene);

	std::cout << "		name nodes animation : " << std::endl;
	for (unsigned int i = 0; i < scene->mAnimations[0]->mNumChannels; i++)
	{
		std::cout << scene->mAnimations[0]->mChannels[i]->mNodeName.C_Str() << std::endl;
	}
	std::cout << std::endl;


}

void Animated_Model::showNodeName(aiNode* node)
{
	std::cout << node->mName.data << std::endl;
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		showNodeName(node->mChildren[i]);
	}
}

void Animated_Model::LoadNode(aiNode* node, const aiScene* scene)
{
	Animated_Mesh mesh;
	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* ai_mesh = scene->mMeshes[i];
		LoadMesh(ai_mesh, scene);
	}

}

void Animated_Model::LoadMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<GLfloat> vertices;
	std::vector<unsigned int> indices;
	std::vector<VertexBoneData> bones;

	vertices.reserve(mesh->mNumVertices);
	indices.reserve(mesh->mNumVertices);

	bones.resize(mesh->mNumVertices);

	for (size_t i = 0; i < mesh->mNumVertices; i++) {
		vertices.insert(vertices.end(), { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z });

		if (mesh->mTextureCoords[0]) {
			vertices.insert(vertices.end(), { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y });
		}
		else {
			vertices.insert(vertices.end(), { 0.0f, 0.0f });
		}
		if (mesh->mNormals != NULL)
		{
			vertices.insert(vertices.end(), { mesh->mNormals[i].x, mesh->mNormals[i].y,  mesh->mNormals[i].z });
		}
		else
		{
			vertices.insert(vertices.end(), { 0.0f, 0.0f, 0.0f });
		}
		if (mesh->mTangents != NULL) {
			vertices.insert(vertices.end(), { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z });
		}
		else {
			vertices.insert(vertices.end(), { 0.0f, 0.0f, 0.0f });
		}
	}

	for (size_t i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (size_t j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	for (unsigned int i = 0; i < mesh->mNumBones; i++)
	{
		unsigned int bone_index = 0;
		std::string bone_name(mesh->mBones[i]->mName.data);

		if (m_BoneMapping.find(bone_name) == m_BoneMapping.end())
		{

			bone_index = m_NumBones;
			m_NumBones++;
			BoneMatrix bi;
			m_bone_matrices.push_back(bi);
			m_bone_matrices[bone_index].offset_matrix = mesh->mBones[i]->mOffsetMatrix;
			m_BoneMapping[bone_name] = bone_index;
		}
		else
		{
			bone_index = m_BoneMapping[bone_name];
		}

		for (unsigned int j = 0; j < mesh->mBones[i]->mNumWeights; j++)
		{
			std::size_t vertex_id = mesh->mBones[i]->mWeights[j].mVertexId;
			float weight = mesh->mBones[i]->mWeights[j].mWeight;
			bones[vertex_id].AddBoneData(bone_index, weight);
		}
	}

	Animated_Mesh* newMesh = new Animated_Mesh();
	newMesh->CreateMeshWithBones(&vertices[0], &indices[0], vertices.size(), indices.size(), bones);
	anim_MeshList.push_back(newMesh);
	meshToTex.push_back(mesh->mMaterialIndex);

	newMesh = nullptr;
	LoadMaterials(scene);

	boneTransform(0.0f, prevTransforms);
}

void Animated_Model::LoadMaterials(const aiScene* scene)
{
	textureList.resize(scene->mNumMaterials);
	metalTextureList.resize(scene->mNumMaterials);
	normalTextureList.resize(scene->mNumMaterials);
	roughTextureList.resize(scene->mNumMaterials);
	parallaxTextureList.resize(scene->mNumMaterials);
	glowTextureList.resize(scene->mNumMaterials);

	for (size_t i = 0; i < scene->mNumMaterials; i++) {
		aiMaterial* material = scene->mMaterials[i];

		textureList[i] = nullptr;
		metalTextureList[i] = nullptr;
		normalTextureList[i] = nullptr;
		roughTextureList[i] = nullptr;
		parallaxTextureList[i] = nullptr;
		glowTextureList[i] = nullptr;

		if (material->GetTextureCount(aiTextureType_DIFFUSE)) {
			aiString path;
			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
				int idx = std::string(path.data).rfind("\\");
				std::string filename = std::string(path.data).substr(idx + 1);

				std::string texPath = std::string("Textures/") + filename;

				textureList[i] = new Texture(texPath, true);

				if (!textureList[i]->LoadTextureNoAlpha()) {
					printf("Failed to load texture at: %s\n", texPath.c_str());
					delete textureList[i];
					textureList[i] = nullptr;
				}
			}
		}

		if (material->GetTextureCount(aiTextureType_SPECULAR)) {
			aiString path;
			if (material->GetTexture(aiTextureType_SPECULAR, 0, &path) == AI_SUCCESS) {
				int idx = std::string(path.data).rfind("\\");
				std::string filename = std::string(path.data).substr(idx + 1);

				std::string texPath = std::string("Textures/Metallic/") + filename;

				metalTextureList[i] = new Texture(texPath.c_str());

				if (!metalTextureList[i]->LoadTextureNoAlpha()) {
					printf("Failed to load texture at: %s\n", texPath.c_str());
					delete metalTextureList[i];
					metalTextureList[i] = nullptr;
				}
			}
		}
		else {
			aiString path;
			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
				int idx = std::string(path.data).rfind("\\");
				std::string filename = std::string(path.data).substr(idx + 1);

				std::string texPath = std::string("Textures/Metallic/") + filename;

				metalTextureList[i] = new Texture(texPath.c_str());

				if (!metalTextureList[i]->LoadTextureNoAlpha()) {
					printf("Failed to load texture at: %s\n", texPath.c_str());
					delete metalTextureList[i];
					metalTextureList[i] = nullptr;
				}
			}

		}

		if (material->GetTextureCount(aiTextureType_HEIGHT)) {
			aiString path;
			if (material->GetTexture(aiTextureType_HEIGHT, 0, &path) == AI_SUCCESS) {
				int idx = std::string(path.data).rfind("\\");
				std::string filename = std::string(path.data).substr(idx + 1);

				std::string texPath = std::string("Textures/Normal/") + filename;

				normalTextureList[i] = new Texture(texPath.c_str());

				if (!normalTextureList[i]->LoadTextureNoAlpha()) {
					printf("Failed to load texture at: %s\n", texPath.c_str());
					delete normalTextureList[i];
					normalTextureList[i] = nullptr;
				}
			}
		}
		else {
			aiString path;
			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
				int idx = std::string(path.data).rfind("\\");
				std::string filename = std::string(path.data).substr(idx + 1);

				std::string texPath = std::string("Textures/Normal/") + filename;

				normalTextureList[i] = new Texture(texPath.c_str());

				if (!normalTextureList[i]->LoadTextureNoAlpha()) {
					printf("Failed to load texture at: %s\n", texPath.c_str());
					delete normalTextureList[i];
					normalTextureList[i] = nullptr;
				}
			}
		}

		if (material->GetTextureCount(aiTextureType_REFLECTION)) {
			aiString path;
			if (material->GetTexture(aiTextureType_REFLECTION, 0, &path) == AI_SUCCESS) {
				int idx = std::string(path.data).rfind("\\");
				std::string filename = std::string(path.data).substr(idx + 1);

				std::string texPath = std::string("Textures/Roughness/") + filename;

				roughTextureList[i] = new Texture(texPath.c_str());

				if (!roughTextureList[i]->LoadTextureNoAlpha()) {
					printf("Failed to load texture for at: %s\n", texPath.c_str());
					delete roughTextureList[i];
					roughTextureList[i] = nullptr;
				}
			}
		}
		else {
			aiString path;
			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
				int idx = std::string(path.data).rfind("\\");
				std::string filename = std::string(path.data).substr(idx + 1);

				std::string texPath = std::string("Textures/Roughness/") + filename;

				roughTextureList[i] = new Texture(texPath.c_str());

				if (!roughTextureList[i]->LoadTextureNoAlpha()) {
					printf("Failed to load texture at: %s\n", texPath.c_str());
					delete roughTextureList[i];
					roughTextureList[i] = nullptr;
				}
			}
		}

		if (material->GetTextureCount(aiTextureType_EMISSIVE)) {

			aiString path;
			if (material->GetTexture(aiTextureType_EMISSIVE, 0, &path) == AI_SUCCESS) {
				int idx = std::string(path.data).rfind("\\");
				std::string filename = std::string(path.data).substr(idx + 1);

				std::string texPath = std::string("Textures/Glow/") + filename;

				glowTextureList[i] = new Texture(texPath.c_str());

				if (!glowTextureList[i]->LoadTextureNoAlpha()) {
					printf("Failed to load reflec texture for static model at: %s\n", texPath.c_str());
					delete glowTextureList[i];
					glowTextureList[i] = nullptr;
				}
			}
		}
		else {
			aiString path;
			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
				int idx = std::string(path.data).rfind("\\");
				std::string filename = std::string(path.data).substr(idx + 1);

				std::string texPath = std::string("Textures/Glow/") + filename;

				glowTextureList[i] = new Texture(texPath.c_str());

				if (!glowTextureList[i]->LoadTextureNoAlpha()) {
					printf("Failed to load texture for static model at: %s\n", texPath.c_str());
					delete glowTextureList[i];
					glowTextureList[i] = nullptr;
				}
			}
		}

		if (!textureList[i]) {
			textureList[i] = new Texture("Textures/plain.jpg", true);
			textureList[i]->LoadTextureNoAlpha();
		}

		if (!metalTextureList[i]) {
			metalTextureList[i] = new Texture("Textures/plain.jpg");
			metalTextureList[i]->LoadTextureNoAlpha();
		}

		if (!normalTextureList[i]) {
			normalTextureList[i] = new Texture("Textures/plain.jpg");
			normalTextureList[i]->LoadTextureNoAlpha();
		}
		if (!roughTextureList[i]) {
			roughTextureList[i] = new Texture("Textures/plain_norm.jpg");
			roughTextureList[i]->LoadTextureNoAlpha();
		}
		if (!parallaxTextureList[i]) {
			parallaxTextureList[i] = new Texture("Textures/plain_dark.jpg");
			parallaxTextureList[i]->LoadTextureNoAlpha();
		}
		if (!glowTextureList[i]) {
			glowTextureList[i] = new Texture("Textures/plain_dark.jpg");
			glowTextureList[i]->LoadTextureNoAlpha();
		}
	}
}

unsigned int Animated_Model::findPosition(float p_animation_time, const aiNodeAnim* p_node_anim)
{
	for (unsigned int i = 0; i < p_node_anim->mNumPositionKeys - 1; i++)
	{
		if (p_animation_time < (float)p_node_anim->mPositionKeys[i + 1].mTime)
		{
			return i;
		}
	}

	//assert(0);
	return 0;
}

unsigned int Animated_Model::findRotation(float p_animation_time, const aiNodeAnim* p_node_anim)
{

	for (unsigned int i = 0; i < p_node_anim->mNumRotationKeys - 1; i++)
	{
		if (p_animation_time < (float)p_node_anim->mRotationKeys[i + 1].mTime)
		{
			return i;
		}
	}

	//assert(0);
	return 0;
}

unsigned int Animated_Model::findScaling(float p_animation_time, const aiNodeAnim* p_node_anim)
{
	for (unsigned int i = 0; i < p_node_anim->mNumScalingKeys - 1; i++)
	{
		if (p_animation_time < (float)p_node_anim->mScalingKeys[i + 1].mTime)
		{
			return i;
		}
	}

	//assert(0);
	return 0;
}

aiVector3D Animated_Model::calcInterpolatedPosition(float p_animation_time, const aiNodeAnim* p_node_anim)
{
	if (p_node_anim->mNumPositionKeys == 1)
	{
		return p_node_anim->mPositionKeys[0].mValue;
	}

	unsigned int position_index = findPosition(p_animation_time, p_node_anim);
	unsigned int next_position_index = position_index + 1;
	//assert(next_position_index < p_node_anim->mNumPositionKeys);
	float delta_time = (float)(p_node_anim->mPositionKeys[next_position_index].mTime - p_node_anim->mPositionKeys[position_index].mTime);
	float factor = (p_animation_time - (float)p_node_anim->mPositionKeys[position_index].mTime) / delta_time;
	//assert(factor >= 0.0f && factor <= 1.0f);
	aiVector3D start = p_node_anim->mPositionKeys[position_index].mValue;
	aiVector3D end = p_node_anim->mPositionKeys[next_position_index].mValue;
	aiVector3D delta = end - start;

	return start + factor * delta;
}

aiQuaternion Animated_Model::calcInterpolatedRotation(float p_animation_time, const aiNodeAnim* p_node_anim)
{
	if (p_node_anim->mNumRotationKeys == 1)
	{
		return p_node_anim->mRotationKeys[0].mValue;
	}

	unsigned int rotation_index = findRotation(p_animation_time, p_node_anim);
	unsigned int next_rotation_index = rotation_index + 1;
	assert(next_rotation_index < p_node_anim->mNumRotationKeys);

	float delta_time = (float)(p_node_anim->mRotationKeys[next_rotation_index].mTime - p_node_anim->mRotationKeys[rotation_index].mTime);

	float factor = (p_animation_time - (float)p_node_anim->mRotationKeys[rotation_index].mTime) / delta_time;

	//assert(factor >= 0.0f && factor <= 1.0f);
	aiQuaternion start_quat = p_node_anim->mRotationKeys[rotation_index].mValue;
	aiQuaternion end_quat = p_node_anim->mRotationKeys[next_rotation_index].mValue;

	return nlerp(start_quat, end_quat, factor);
}

aiVector3D Animated_Model::calcInterpolatedScaling(float p_animation_time, const aiNodeAnim* p_node_anim)
{
	if (p_node_anim->mNumScalingKeys == 1)
	{
		return p_node_anim->mScalingKeys[0].mValue;
	}

	unsigned int scaling_index = findScaling(p_animation_time, p_node_anim);
	unsigned int next_scaling_index = scaling_index + 1;
	assert(next_scaling_index < p_node_anim->mNumScalingKeys);

	float delta_time = (float)(p_node_anim->mScalingKeys[next_scaling_index].mTime - p_node_anim->mScalingKeys[scaling_index].mTime);

	float  factor = (p_animation_time - (float)p_node_anim->mScalingKeys[scaling_index].mTime) / delta_time;
	//assert(factor >= 0.0f && factor <= 1.0f);
	aiVector3D start = p_node_anim->mScalingKeys[scaling_index].mValue;
	aiVector3D end = p_node_anim->mScalingKeys[next_scaling_index].mValue;
	aiVector3D delta = end - start;

	return start + factor * delta;
}

const aiNodeAnim* Animated_Model::findNodeAnim(const aiAnimation* p_animation, const std::string p_node_name)
{

	for (unsigned int i = 0; i < p_animation->mNumChannels; i++)
	{
		const aiNodeAnim* node_anim = p_animation->mChannels[i];
		if (std::string(node_anim->mNodeName.data) == p_node_name)
		{
			return node_anim;
		}
	}

	return nullptr;
}

void Animated_Model::readNodeHierarchy(float p_animation_time, const aiNode* p_node, const aiMatrix4x4 parent_transform)
{

	std::string node_name(p_node->mName.data);


	const aiAnimation* animation = scene->mAnimations[0];
	aiMatrix4x4 node_transform = p_node->mTransformation;

	const aiNodeAnim* node_anim = findNodeAnim(animation, node_name);

	if (node_anim)
	{
		aiVector3D scaling_vector = calcInterpolatedScaling(p_animation_time, node_anim);
		aiMatrix4x4 scaling_matr;
		aiMatrix4x4::Scaling(scaling_vector, scaling_matr);

		aiQuaternion rotate_quat = calcInterpolatedRotation(p_animation_time, node_anim);
		aiMatrix4x4 rotate_matr = aiMatrix4x4(rotate_quat.GetMatrix());

		aiVector3D translate_vector = calcInterpolatedPosition(p_animation_time, node_anim);
		aiMatrix4x4 translate_matr;
		aiMatrix4x4::Translation(translate_vector, translate_matr);

		if (std::string(node_anim->mNodeName.data) == std::string("Head"))
		{
			aiQuaternion rotate_head = aiQuaternion(rotate_head_xz.w, rotate_head_xz.x, rotate_head_xz.y, rotate_head_xz.z);

			node_transform = translate_matr * (rotate_matr * aiMatrix4x4(rotate_head.GetMatrix())) * scaling_matr;
		}
		else
		{
			node_transform = translate_matr * rotate_matr * scaling_matr;
		}

	}

	aiMatrix4x4 global_transform = parent_transform * node_transform;


	if (m_BoneMapping.find(node_name) != m_BoneMapping.end())
	{
		unsigned int bone_index = m_BoneMapping[node_name];
		m_bone_matrices[bone_index].final_world_transform = m_GlobalInverseTransform * global_transform * m_bone_matrices[bone_index].offset_matrix;
	}

	for (unsigned int i = 0; i < p_node->mNumChildren; i++)
	{
		readNodeHierarchy(p_animation_time, p_node->mChildren[i], global_transform);
	}

}

void Animated_Model::boneTransform(double time_in_sec, std::vector<aiMatrix4x4>& transforms)
{
	aiMatrix4x4 identity_matrix;

	double time_in_ticks = time_in_sec * ticks_per_second;
	float animation_time = static_cast<float>(fmod(time_in_ticks, scene->mAnimations[0]->mDuration));

	readNodeHierarchy(animation_time, scene->mRootNode, identity_matrix);

	transforms.resize(m_NumBones);

	for (unsigned int i = 0; i < m_NumBones; i++)
	{
		transforms[i] = m_bone_matrices[i].final_world_transform;
	}
}

glm::mat4 Animated_Model::aiToGlm(aiMatrix4x4 ai_matr)
{
	glm::mat4 result;
	result[0].x = ai_matr.a1; result[0].y = ai_matr.b1; result[0].z = ai_matr.c1; result[0].w = ai_matr.d1;
	result[1].x = ai_matr.a2; result[1].y = ai_matr.b2; result[1].z = ai_matr.c2; result[1].w = ai_matr.d2;
	result[2].x = ai_matr.a3; result[2].y = ai_matr.b3; result[2].z = ai_matr.c3; result[2].w = ai_matr.d3;
	result[3].x = ai_matr.a4; result[3].y = ai_matr.b4; result[3].z = ai_matr.c4; result[3].w = ai_matr.d4;

	return result;
}

aiQuaternion Animated_Model::nlerp(aiQuaternion a, aiQuaternion b, float blend)
{
	a.Normalize();
	b.Normalize();

	aiQuaternion result;
	float dot_product = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
	float one_minus_blend = 1.0f - blend;

	if (dot_product < 0.0f)
	{
		result.x = a.x * one_minus_blend + blend * -b.x;
		result.y = a.y * one_minus_blend + blend * -b.y;
		result.z = a.z * one_minus_blend + blend * -b.z;
		result.w = a.w * one_minus_blend + blend * -b.w;
	}
	else
	{
		result.x = a.x * one_minus_blend + blend * b.x;
		result.y = a.y * one_minus_blend + blend * b.y;
		result.z = a.z * one_minus_blend + blend * b.z;
		result.w = a.w * one_minus_blend + blend * b.w;
	}

	return result.Normalize();
}

void Animated_Model::ClearModel()
{

	for (size_t i = 0; i < anim_MeshList.size(); i++) {
		if (anim_MeshList[i]) {
			delete anim_MeshList[i];
			anim_MeshList[i] = nullptr;
		}
	}
	for (size_t i = 0; i < textureList.size(); i++) {
		if (textureList[i]) {
			delete textureList[i];
			textureList[i] = nullptr;
		}
	}
	for (size_t i = 0; i < metalTextureList.size(); i++) {
		if (metalTextureList[i]) {
			delete metalTextureList[i];
			metalTextureList[i] = nullptr;
		}
	}
	for (size_t i = 0; i < roughTextureList.size(); i++) {
		if (roughTextureList[i]) {
			delete roughTextureList[i];
			roughTextureList[i] = nullptr;
		}
	}
	for (size_t i = 0; i < parallaxTextureList.size(); i++) {
		if (parallaxTextureList[i]) {
			delete parallaxTextureList[i];
			parallaxTextureList[i] = nullptr;
		}
	}
	for (size_t i = 0; i < glowTextureList.size(); i++) {
		if (glowTextureList[i]) {
			delete glowTextureList[i];
			glowTextureList[i] = nullptr;
		}
	}
}

Animated_Model::~Animated_Model()
{
	delete scene;

	//if(prevTransforms!= nullptr)
	//{
	//	delete prevTransforms;
	//	prevTransforms = nullptr;
	//}

	ClearModel();
}

#include "pch.h"
#include "Static_Model.h"
#include "Texture.h"
#include "Static_Mesh.h"

void Static_Model::RenderModel()
{
	for (size_t i = 0; i < static_MeshList.size(); i++) {
		unsigned int materialIndex = meshToTex[i];

		if (materialIndex < textureList.size() && textureList[materialIndex]) {
			textureList[materialIndex]->UseTexture(0);
			metalTextureList[materialIndex]->UseTexture(5);
			normalTextureList[materialIndex]->UseTexture(6);
			roughTextureList[materialIndex]->UseTexture(10);
			parallaxTextureList[materialIndex]->UseTexture(11);
			glowTextureList[materialIndex]->UseTexture(12);
		}

		static_MeshList[i]->RenderMesh();
	}
}

void Static_Model::LoadModel(const std::string& fileName)
{
	Assimp::Importer importer;
	scene = importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_OptimizeMeshes | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices | aiProcess_CalcTangentSpace);

	if (!scene) {
		printf("Model (%s) failed to load: %s", fileName.c_str(), importer.GetErrorString());
		return;
	}

	LoadNode(scene->mRootNode, scene);

	LoadMaterials(scene);
}

void Static_Model::LoadNode(aiNode* node, const aiScene* scene)
{
	for (size_t i = 0; i < node->mNumMeshes; i++) {
		LoadMesh(scene->mMeshes[node->mMeshes[i]], scene);
	}

	for (size_t i = 0; i < node->mNumChildren; i++) {
		LoadNode(node->mChildren[i], scene);
	}
}

void Static_Model::LoadMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<GLfloat> vertices;
	std::vector<unsigned int> indices;

	for (size_t i = 0; i < mesh->mNumVertices; i++) {
		vertices.insert(vertices.end(), { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z });
		if (mesh->mTextureCoords[0]) {
			vertices.insert(vertices.end(), { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y });
		}
		else {
			vertices.insert(vertices.end(), { 0.0f, 0.0f });
		}
		vertices.insert(vertices.end(), { mesh->mNormals[i].x, mesh->mNormals[i].y,  mesh->mNormals[i].z });
		vertices.insert(vertices.end(), { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z });
		//vertices.insert(vertices.end(), { -mesh->mBitangents[i].x, -mesh->mBitangents[i].y, -mesh->mBitangents[i].z });
	}

	for (size_t i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (size_t j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}
	Static_Mesh* newMesh = new Static_Mesh();
	newMesh->CreateMeshWithTangentNormal(&vertices[0], &indices[0], vertices.size(), indices.size());
	static_MeshList.push_back(newMesh);
	meshToTex.push_back(mesh->mMaterialIndex);

	newMesh = nullptr;
}

void Static_Model::LoadMaterials(const aiScene* scene)
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

		//printf("%d\n",material->GetTextureCount(aiTextureType_DIFFUSE));

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

				metalTextureList[i] = new Texture(texPath);

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

					if (metalTextureList[i] == nullptr)
					{
						metalTextureList[i] = new Texture(texPath);

						if (!metalTextureList[i]->LoadTextureNoAlpha()) {
							printf("Failed to load texture for static model at: %s\n", texPath.c_str());
							delete metalTextureList[i];
							metalTextureList[i] = nullptr;
						}
					}
				}
			
		}

		if (material->GetTextureCount(aiTextureType_HEIGHT)) {
			aiString path;
			if (material->GetTexture(aiTextureType_HEIGHT, 0, &path) == AI_SUCCESS) {
				int idx = std::string(path.data).rfind("\\");
				std::string filename = std::string(path.data).substr(idx + 1);

				std::string texPath = std::string("Textures/Normal/") + filename;

				normalTextureList[i] = new Texture(texPath);

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

				normalTextureList[i] = new Texture(texPath);

				if (!normalTextureList[i]->LoadTextureNoAlpha()) {
					printf("Failed to load texture for static model at: %s\n", texPath.c_str());
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

				roughTextureList[i] = new Texture(texPath);

				if (!roughTextureList[i]->LoadTextureNoAlpha()) {
					printf("Failed to load reflec texture for static model at: %s\n", texPath.c_str());
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

				roughTextureList[i] = new Texture(texPath);

				if (!roughTextureList[i]->LoadTextureNoAlpha()) {
					printf("Failed to load texture for static model at: %s\n", texPath.c_str());
					delete roughTextureList[i];
					roughTextureList[i] = nullptr;
				}
			}
		}

		if (material->GetTextureCount(aiTextureType_DISPLACEMENT)) {

			aiString path;
			if (material->GetTexture(aiTextureType_DISPLACEMENT, 0, &path) == AI_SUCCESS) {
				int idx = std::string(path.data).rfind("\\");
				std::string filename = std::string(path.data).substr(idx + 1);

				std::string texPath = std::string("Textures/Parallax/") + filename;

				parallaxTextureList[i] = new Texture(texPath);

				if (!parallaxTextureList[i]->LoadTextureNoAlpha()) {
					printf("Failed to load texture for static model at: %s\n", texPath.c_str());
					delete parallaxTextureList[i];
					parallaxTextureList[i] = nullptr;
				}
			}
		}
		else {
			aiString path;
			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
				int idx = std::string(path.data).rfind("\\");
				std::string filename = std::string(path.data).substr(idx + 1);

				std::string texPath = std::string("Textures/Parallax/") + filename;

				parallaxTextureList[i] = new Texture(texPath);

				if (!parallaxTextureList[i]->LoadTextureNoAlpha()) {
					printf("Failed to load texture for static model at: %s\n", texPath.c_str());
					delete parallaxTextureList[i];
					parallaxTextureList[i] = nullptr;
				}
			}
		}

		if (material->GetTextureCount(aiTextureType_EMISSIVE)) {

			aiString path;
			if (material->GetTexture(aiTextureType_EMISSIVE, 0, &path) == AI_SUCCESS) {
				int idx = std::string(path.data).rfind("\\");
				std::string filename = std::string(path.data).substr(idx + 1);

				std::string texPath = std::string("Textures/Glow/") + filename;

				glowTextureList[i] = new Texture(texPath);

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

				glowTextureList[i] = new Texture(texPath);

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

void Static_Model::ClearModel()
{
	for (size_t i = 0; i < static_MeshList.size(); i++) {
		if (static_MeshList[i]) {
			delete static_MeshList[i];
			static_MeshList[i] = nullptr;
		}
	}
}

Static_Model::~Static_Model()
{
	ClearModel();

}

#ifndef MODEL
#define MODEL

#include "pch.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class Mesh;
		class VertexBoneData;
		class Texture;

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
			std::shared_ptr<std::vector<std::shared_ptr<Mesh>>> MeshList;

		protected:
			virtual void LoadNode(aiNode* node, const aiScene* scene);
			virtual void LoadMesh(aiMesh* mesh, const aiScene* scene);
			virtual void LoadMaterials(const aiScene* scene);
			void ClearModel();

			std::vector<Texture*> textureList;
			std::vector<Texture*> metalTextureList;
			std::vector<Texture*> normalTextureList;
			std::vector<Texture*> roughTextureList;
			std::vector<Texture*> parallaxTextureList;
			std::vector<Texture*> glowTextureList;
			Texture* billboardTexture = nullptr;
			std::vector<unsigned int> meshToTex;

			const aiScene* scene = nullptr;
		};
	}
}
#endif
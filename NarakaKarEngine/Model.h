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

			std::unique_ptr<std::vector<std::weak_ptr<Mesh>>> MeshList;
			std::unique_ptr<std::map<TexType, std::vector<std::weak_ptr<Texture>>>> TextureMap;

		protected:
			virtual void LoadNode(aiNode* node, const aiScene* scene);
			virtual void LoadMesh(aiMesh* mesh, const aiScene* scene);
			virtual void LoadMaterials(const aiScene* scene);
			void ClearModel();

			Texture* billboardTexture = nullptr;
			std::vector<unsigned int> meshToTex;

			const aiScene* scene = nullptr;
		};
	}
}
#endif
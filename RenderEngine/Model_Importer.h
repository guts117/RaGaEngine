#ifndef MODEL_IMPORTER
#define MODEL_IMPORTER

#include "render_pch.h"

//ToDo:
//Expand This on a dedicated issue #60
namespace NarakaRenderEngine
{
	namespace RenderEngine
	{
		struct BoneMatrix
		{
			aiMatrix4x4 offset_matrix;
			aiMatrix4x4 final_world_transform;
		};

		struct KeyFrames
		{
			GLfloat value;
			GLfloat time;
		};

		//struct AnimationNode
		//{
		//	std::string NodeName;
		//	std::string KeyFrames;
		//};

		class Mesh;
		class Texture;
		enum TexType;

		class Model_Importer final
		{
		public:
			explicit Model_Importer() = delete;

			void LoadModel(std::string&& fileName);

			~Model_Importer();

			std::unique_ptr<std::vector<std::weak_ptr<Mesh>>> MeshList;
			std::unique_ptr<std::map<TexType, std::vector<std::weak_ptr<Texture>>>> TextureMap;
			std::unique_ptr<std::vector<aiAnimation>> AnimationList;
			std::unique_ptr< std::vector<aiNode>> NodeHierarchy;
		private:
			struct Impl;

			const Impl* Pimpl() const { return m_pImpl.get(); }
			Impl* Pimpl() { return m_pImpl.get(); }

			std::unique_ptr<Impl> m_pImpl;
		};
	}
}
#endif
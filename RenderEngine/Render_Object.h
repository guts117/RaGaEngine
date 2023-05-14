#ifndef RENDER_OBJECT
#define RENDER_OBJECT

#include "render_pch.h"

namespace NarakaRenderEngine
{
	namespace RenderEngine
	{
		class Mesh;
		enum TexType;
		class Texture;
		class Shader_Object;

		struct Transform
		{
		private:
			glm::mat4 modelMatrix;
		
		public:
			void SetModelMatrtix(glm::mat4 modelMat)	{ modelMatrix = modelMat; }
			glm::mat4 GetModelMatrix() const			{ return modelMatrix; }
		};

		struct RenderObjectParams
		{
			const bool&& useWorldSpaceTransform = false;
			const bool&& useTextures = false;
			const glm::mat4* prevViewProjection = nullptr;	//has motion blur
		};

		struct BoneTransform
		{
			std::shared_ptr<const aiMatrix4x4> FinalWorldTransform;
			std::unique_ptr<aiMatrix4x4> PrevFinalWorldTransfrom;
		};

		class Render_Object
		{
		public:
			explicit Render_Object() = delete;

			explicit Render_Object(std::vector<rw_clustering_ptr<Mesh>>&& meshes
								, std::map<TexType, std::vector<rw_clustering_ptr<Texture>>>&& textureMap = std::map<TexType, std::vector<rw_clustering_ptr<Texture>>>()
								, rw_clustering_ptr<Transform>&& modelMatrix = rw_clustering_ptr<Transform>()
								, rw_clustering_ptr<Transform>&& prevModelMatrix = rw_clustering_ptr<Transform>()
								, std::vector<BoneTransform*>&& boneMatrices = std::vector<BoneTransform*>());

			void SetTextures(std::map<TexType, std::vector<rw_clustering_ptr<Texture>>>&& textureMap);

			void ResetTextures();
			glm::mat4 GetModelMatrix() const;

			Render_Object(Render_Object&& rhs) noexcept;
			Render_Object& operator=(Render_Object&& rhs) noexcept;

			Render_Object(const Render_Object& rhs) noexcept = delete;
			Render_Object& operator=(const Render_Object& rhs) noexcept = delete;

			void RenderObject(rw_clustering_ptr<Shader_Object>& shader, RenderObjectParams&& params);

			bool IsTesselated();

			~Render_Object() noexcept;
		private:
			struct Impl;

			const Impl& Pimpl() const { return m_pImpl.Get(); }
			Impl& Pimpl() { return m_pImpl.Get(); }

#ifdef NDEBUG //size of map debug(24), release(16) and size of vector debug(32), release(24)
			ForwardDeclaredPimpl<Impl, alignof(void*) * 12, alignof(void*)> m_pImpl;
#else
			ForwardDeclaredPimpl<Impl, alignof(void*) * 15, alignof(void*)> m_pImpl;
#endif;
		};
	}
}

#endif
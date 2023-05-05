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

			explicit Render_Object(std::vector<Mesh*>&& meshes
								, std::map<TexType, std::vector<Texture*>>&& textureMap = std::map<TexType, std::vector<Texture*>>()
								, glm::mat4* modelMatrix = nullptr
								, glm::mat4* prevModelMatrix = nullptr
								, std::vector<BoneTransform*>&& boneMatrices = std::vector<BoneTransform*>());

			void SetTextures(std::map<TexType, std::vector<Texture*>>&& textureMap);

			void ResetTextures();
			const glm::mat4* GetModelMatrix() const ;

			Render_Object(Render_Object&& rhs) noexcept;
			Render_Object& operator=(Render_Object&& rhs) noexcept;

			Render_Object(const Render_Object& rhs) noexcept = delete;
			Render_Object& operator=(const Render_Object& rhs) noexcept = delete;

			void RenderObject(Shader_Object& shader, RenderObjectParams&& params) const;

			const bool IsTesselated() const;

			~Render_Object() noexcept;
		private:
			struct Impl;

			const Impl& Pimpl() const { return m_pImpl.Get(); }
			Impl& Pimpl() { return m_pImpl.Get(); }

#ifdef NDEBUG //size of map debug(24), release(16) and size of vector debug(32), release(24)
			ForwardDeclaredPimpl<Impl, alignof(void*) * 10, alignof(void*)> m_pImpl;
#else
			ForwardDeclaredPimpl<Impl, alignof(void*) * 13, alignof(void*)> m_pImpl;
#endif;
		};
	}
}

#endif
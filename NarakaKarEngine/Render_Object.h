#ifndef RENDER_OBJECT
#define RENDER_OBJECT

#include "pch.h"

namespace NarakaKarEngine
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

			explicit Render_Object(std::shared_ptr<std::vector<Mesh>> meshes
								, std::shared_ptr<std::map<TexType, std::vector<Texture>>> textureMap = nullptr
								, std::shared_ptr<glm::mat4> modelMatrix = nullptr
								, std::shared_ptr<glm::mat4> prevModelMatrix = nullptr
								, std::shared_ptr<std::vector<BoneTransform>> boneMatrices = nullptr);

			void SetTextures(std::map<TexType, std::vector<Texture>>&& textureMap);

			Render_Object(Render_Object&& rhs) noexcept = default;
			Render_Object& operator=(Render_Object&& rhs) noexcept = default;

			Render_Object(const Render_Object& rhs) noexcept = delete;
			Render_Object& operator=(const Render_Object& rhs) noexcept = delete;

			void RenderObject(Shader_Object& shader, const RenderObjectParams&& params);

			const bool IsTesselated() const;

			~Render_Object();
		private:
			struct Impl;

			const Impl* Pimpl() const { return m_pImpl.get(); }
			Impl* Pimpl() { return m_pImpl.get(); }

			std::unique_ptr<Impl> m_pImpl;
		};
	}
}

#endif
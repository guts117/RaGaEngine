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

		struct BoneTransform
		{
			std::shared_ptr<const aiMatrix4x4> FinalWorldTransform;
			std::unique_ptr<aiMatrix4x4> PrevFinalWorldTransfrom;
		};

		class Render_Object
		{
		public:
			explicit Render_Object() = delete;

			explicit Render_Object(std::shared_ptr<std::vector<std::shared_ptr<Mesh>>> meshes
								, std::shared_ptr<std::map<TexType, std::vector<std::shared_ptr<Texture>>>> textureMap = nullptr
								, std::shared_ptr<std::vector<BoneTransform>> boneMatrices = nullptr);

			Render_Object(Render_Object&& rhs) noexcept = default;
			Render_Object& operator=(Render_Object&& rhs) noexcept = default;

			Render_Object(const Render_Object& rhs) noexcept = delete;
			Render_Object& operator=(const Render_Object& rhs) noexcept = delete;

			void RenderObject(std::shared_ptr<Shader_Object> shader, const glm::mat4& prevPV, bool&& hasModelMatrix = true);
			
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
#ifndef MESH
#define MESH

#include "render_pch.h"
#include "VertexBoneData.h"
#include "ForwardDeclaredPimpl.h"

namespace NarakaRenderEngine
{
	namespace RenderEngine
	{	
		struct MeshGenParams
		{
			bool&& HasNormal = false;
			bool&& HasTangent = false;
			bool&& IsTessellated = false;
			GLuint&& InstanceCount = 0;
			std::vector<VertexBoneData>&& BoneData = std::vector<VertexBoneData>();
		};

		class Mesh
		{
		public:
			explicit Mesh() = delete;
			explicit Mesh(GLuint materialIndex, std::vector<std::vector<GLfloat>>&& vertices, std::vector<GLuint>&& indices, MeshGenParams&& meshGenParams);

			Mesh(Mesh&& rhs) noexcept;
			Mesh& operator= (Mesh&& rhs) noexcept;

			Mesh(const Mesh& rhs) = delete;
			Mesh& operator= (const Mesh& rhs) = delete;

			void RenderMesh();
			const GLuint& GetMaterialIndex() const;
			const bool& IsTessellated() const;

			~Mesh() noexcept;
		private:
			struct Impl;

			const Impl& Pimpl() const { return m_pImpl.Get(); }
			Impl& Pimpl() { return m_pImpl.Get(); }

			ForwardDeclaredPimpl<Impl, alignof(GLuint) * 8, alignof(GLuint)> m_pImpl;
		};
	}
}
#endif
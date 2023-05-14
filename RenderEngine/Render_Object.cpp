#include "render_pch.h"
#include "Render_Object.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader_Object.h"

using namespace NarakaRenderEngine;
using namespace RenderEngine;

struct alignas(alignof(void*)) Render_Object::Impl
{
	std::vector<rw_clustering_ptr<Mesh>> m_Meshes;
	std::vector<BoneTransform*> m_BoneMatrices;
	std::map<TexType, std::vector<rw_clustering_ptr<Texture>>> m_TextureMap;
	rw_clustering_ptr<Transform> m_ModelMatrix;
	rw_clustering_ptr<Transform> m_PrevModelMatrix;

	Impl() = delete;

	Impl(std::vector<rw_clustering_ptr<Mesh>>&& meshes
		, std::map<TexType, std::vector<rw_clustering_ptr<Texture>>>&& textureMap
		, rw_clustering_ptr<Transform>&& modelMatrix
		, rw_clustering_ptr<Transform>&& prevModelMatrix
		, std::vector<BoneTransform*>&& boneMatrices)
		: m_Meshes{ std::move(meshes) }
		, m_BoneMatrices{ std::move(boneMatrices) }
		, m_TextureMap{ std::move(textureMap) }	
		, m_ModelMatrix{ std::move(modelMatrix) }
		, m_PrevModelMatrix{ std::move(prevModelMatrix)}
	{
	}

	Impl(Impl&& rhs) noexcept 
		: m_TextureMap{ std::move(rhs.m_TextureMap) }	
		, m_Meshes {std::move(rhs.m_Meshes)}	
		, m_BoneMatrices{ std::move(rhs.m_BoneMatrices) }
		, m_ModelMatrix {std::exchange(rhs.m_ModelMatrix, rw_clustering_ptr<Transform>())}
		, m_PrevModelMatrix {std::exchange(rhs.m_PrevModelMatrix, rw_clustering_ptr<Transform>())}
	{
	};
	Impl& operator=(Impl&& rhs) noexcept
	{
		m_TextureMap = std::move(rhs.m_TextureMap);
		m_Meshes = std::move(rhs.m_Meshes);
		m_BoneMatrices = std::move(rhs.m_BoneMatrices);
		m_ModelMatrix = std::exchange(rhs.m_ModelMatrix, rw_clustering_ptr<Transform>());
		m_PrevModelMatrix = std::exchange(rhs.m_PrevModelMatrix, rw_clustering_ptr<Transform>());
		
		return *this;
	};

	Impl(const Impl& rhs) noexcept = delete;
	Impl& operator=(const Impl& rhs) noexcept = delete;

	void RenderObject(rw_clustering_ptr<Shader_Object>& shader, RenderObjectParams&& params)
	{
		if (params.useWorldSpaceTransform && m_ModelMatrix.isValid())
		{
			shader->SetVariable("Model", *(m_ModelMatrix.get()));

			if (params.prevViewProjection != nullptr && m_PrevModelMatrix.isValid())
			{
				shader->SetVariable("prevPVM", *params.prevViewProjection * (m_PrevModelMatrix.get())->GetModelMatrix());
				m_PrevModelMatrix.write(std::mem_fn(&Transform::SetModelMatrtix), m_ModelMatrix.get()->GetModelMatrix());
			}
		}

		if (!m_Meshes.size()) { return; }

		auto resetToTexUnit = shader->GetTextureUnit();

		for (auto i = 0; i < m_BoneMatrices.size(); i++) // move all matrices for actual model position to shader
		{
			auto& boneMatrix = m_BoneMatrices[i];
			shader->SetVariable("gBones", *boneMatrix->FinalWorldTransform);
			shader->SetVariable("gPrevBones", *boneMatrix->PrevFinalWorldTransfrom);
			*boneMatrix->PrevFinalWorldTransfrom = *boneMatrix->FinalWorldTransform;
		}

		for (size_t i = 0; i < m_Meshes.size(); ++i)
		{
			if(auto mesh = m_Meshes[i].get())
			{
				auto materialIndex = mesh->GetMaterialIndex();

				if (params.useTextures)
				{
					for (auto texType = 0; texType < Max; ++texType)
					{
						if (m_TextureMap.contains((TexType)texType))
						{
							if (materialIndex >= m_TextureMap.at((TexType)texType).size()) { continue; }

							if (auto texMap = m_TextureMap.at((TexType)texType)[materialIndex].get()) 
							{
								//Very Important:
								//ToDo: How do you handle thread accesing two different memory pools (here: Shader_Object.SetTexture && Texture.UseTextureTemp)
								switch ((TexType)texType)
								{
								case Albedo:
									shader.write(std::mem_fn<void(std::string&&, const GLuint&, std::string&&)>(&Shader_Object::SetTextureUnit), "material", 0, "albedoMap");
									break;
								case Metallic:
									shader.write(std::mem_fn<void(std::string&&, const GLuint&, std::string&&)>(&Shader_Object::SetTextureUnit), "material", 0, "metallicMap");
									break;
								case Roughness:
									shader.write(std::mem_fn<void(std::string&&, const GLuint&, std::string&&)>(&Shader_Object::SetTextureUnit), "material", 0, "roughnessMap");
									break;
								case Normal:
									shader.write(std::mem_fn<void(std::string&&, const GLuint&, std::string&&)>(&Shader_Object::SetTextureUnit), "material", 0, "normalMap");
									break;
								case Parallax:
									shader.write(std::mem_fn<void(std::string&&, const GLuint&, std::string&&)>(&Shader_Object::SetTextureUnit), "material", 0, "parallaxMap");
									break;
								case Glow:
									shader.write(std::mem_fn<void(std::string&&, const GLuint&, std::string&&)>(&Shader_Object::SetTextureUnit), "material", 0, "glowMap");
									break;
								case Displacement:
									shader.write(std::mem_fn<void(std::string&&)>(&Shader_Object::SetTextureUnit), "displacementMap");
									//ToDo: Add dispFactor through material or something
									shader->SetVariable("dispFactor", 0.2f);
									break;
								case Noise:
									shader.write(std::mem_fn<void(std::string&&)>(&Shader_Object::SetTextureUnit), "noise");
									//ToDo: Add noiseFactor through material or something
									//shader.SetVariable("noiseFactor", 0.2f);
									break;
								case HDR:
								default:
									shader.write(std::mem_fn<void(std::string&&)>(&Shader_Object::SetTextureUnit), "theTexture");
									break;
								}

								texMap->UseTextureTemp(shader->GetTextureUnit());
							}
						}
					}
				}

				mesh->RenderMesh();
			}
		}
		//ToDo: This should go in OnExitStage of RenderObject;
		shader.write(std::mem_fn(&Shader_Object::ResetTextureUnit), resetToTexUnit);
	}

	bool IsTesselated()
	{
		auto check = [&](rw_clustering_ptr<Mesh>& mesh)->bool 
		{
			if (mesh.get())
			{
				return mesh->IsTessellated();
			}
			return false;
		};
		return std::find_if(m_Meshes.begin(), m_Meshes.end(), check) != m_Meshes.end();
	}

	~Impl() noexcept = default;
};

Render_Object::Render_Object(std::vector<rw_clustering_ptr<Mesh>>&& meshes
			, std::map<TexType, std::vector<rw_clustering_ptr<Texture>>>&& textureMap
			, rw_clustering_ptr<Transform>&& modelMatrix
			, rw_clustering_ptr<Transform>&& prevModelMatrix
			, std::vector<BoneTransform*>&& boneMatrices)
			: m_pImpl{Impl(std::move(meshes), std::move(textureMap), std::move(modelMatrix), std::move(prevModelMatrix), std::move(boneMatrices))}
{
}

void Render_Object::SetTextures(std::map<TexType, std::vector<rw_clustering_ptr<Texture>>>&& textureMap)
{
	Pimpl().m_TextureMap = std::map<TexType, std::vector<rw_clustering_ptr<Texture>>>(std::move(textureMap));
}

void Render_Object::ResetTextures()
{
	Pimpl().m_TextureMap.clear();
}

glm::mat4 Render_Object::GetModelMatrix() const
{
	return Pimpl().m_ModelMatrix.get()->GetModelMatrix();
}

Render_Object::Render_Object(Render_Object&& rhs) noexcept = default;
Render_Object& Render_Object::operator=(Render_Object&& rhs) noexcept = default;

void Render_Object::RenderObject(rw_clustering_ptr<Shader_Object>& shader, RenderObjectParams&& params)
{
	Pimpl().RenderObject(shader, std::move(params));
}

bool Render_Object::IsTesselated()
{
	return Pimpl().IsTesselated();
}

Render_Object::~Render_Object() noexcept = default;

#include "render_pch.h"
#include "Render_Object.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader_Object.h"

using namespace NarakaRenderEngine;
using namespace RenderEngine;

struct alignas(alignof(void*)) Render_Object::Impl
{
	std::vector<clustering_ptr<Mesh>> m_Meshes;
	std::vector<BoneTransform*> m_BoneMatrices;
	std::map<TexType, std::vector<clustering_ptr<Texture>>> m_TextureMap;
	clustering_ptr<glm::mat4> m_ModelMatrix;
	clustering_ptr<glm::mat4> m_PrevModelMatrix;

	Impl() = delete;

	Impl(std::vector<clustering_ptr<Mesh>>&& meshes
		, std::map<TexType, std::vector<clustering_ptr<Texture>>>&& textureMap
		, clustering_ptr<glm::mat4>&& modelMatrix
		, clustering_ptr<glm::mat4>&& prevModelMatrix
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
		, m_ModelMatrix {std::exchange(rhs.m_ModelMatrix, clustering_ptr<glm::mat4>())}
		, m_PrevModelMatrix {std::exchange(rhs.m_PrevModelMatrix, clustering_ptr<glm::mat4>())}
	{
	};
	Impl& operator=(Impl&& rhs) noexcept
	{
		m_TextureMap = std::move(rhs.m_TextureMap);
		m_Meshes = std::move(rhs.m_Meshes);
		m_BoneMatrices = std::move(rhs.m_BoneMatrices);
		m_ModelMatrix = std::exchange(rhs.m_ModelMatrix, clustering_ptr<glm::mat4>());
		m_PrevModelMatrix = std::exchange(rhs.m_PrevModelMatrix, clustering_ptr<glm::mat4>());
		
		return *this;
	};

	Impl(const Impl& rhs) noexcept = delete;
	Impl& operator=(const Impl& rhs) noexcept = delete;

	void RenderObject(clustering_ptr<Shader_Object>& shader, RenderObjectParams&& params)
	{
		if (params.useWorldSpaceTransform && m_ModelMatrix.poolHeadPtr != nullptr)
		{
			shader->SetVariable("Model", *(m_ModelMatrix.get()));

			if (params.prevViewProjection != nullptr && m_PrevModelMatrix.poolHeadPtr != nullptr)
			{
				shader->SetVariable("prevPVM", *params.prevViewProjection * *(m_PrevModelMatrix.get()));
				*(m_PrevModelMatrix.get()) = *(m_ModelMatrix.get());
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
								switch ((TexType)texType)
								{
								case Albedo:
									texMap->UseTextureTemp(shader->SetTextureUnit("material", 0, "albedoMap"));
									break;
								case Metallic:
									texMap->UseTextureTemp(shader->SetTextureUnit("material", 0, "metallicMap"));
									break;
								case Roughness:
									texMap->UseTextureTemp(shader->SetTextureUnit("material", 0, "roughnessMap"));
									break;
								case Normal:
									texMap->UseTextureTemp(shader->SetTextureUnit("material", 0, "normalMap"));
									break;
								case Parallax:
									texMap->UseTextureTemp(shader->SetTextureUnit("material", 0, "parallaxMap"));
									break;
								case Glow:
									texMap->UseTextureTemp(shader->SetTextureUnit("material", 0, "glowMap"));
									break;
								case Displacement:
									texMap->UseTextureTemp(shader->SetTextureUnit("displacementMap"));
									//ToDo: Add dispFactor through material or something
									shader->SetVariable("dispFactor", 0.2f);
									break;
								case Noise:
									texMap->UseTextureTemp(shader->SetTextureUnit("noise"));
									//ToDo: Add noiseFactor through material or something
									//shader.SetVariable("noiseFactor", 0.2f);
									break;
								case HDR:
								default:
									texMap->UseTextureTemp(shader->SetTextureUnit("theTexture"));
									break;
								}
							}
						}
					}
				}

				mesh->RenderMesh();
			}
		}
		shader->ResetTextureUnit(std::move(resetToTexUnit));
	}

	bool IsTesselated()
	{
		auto check = [&](clustering_ptr<Mesh>& mesh)->bool 
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

Render_Object::Render_Object(std::vector<clustering_ptr<Mesh>>&& meshes
			, std::map<TexType, std::vector<clustering_ptr<Texture>>>&& textureMap
			, clustering_ptr<glm::mat4>&& modelMatrix
			, clustering_ptr<glm::mat4>&& prevModelMatrix
			, std::vector<BoneTransform*>&& boneMatrices)
			: m_pImpl{Impl(std::move(meshes), std::move(textureMap), std::move(modelMatrix), std::move(prevModelMatrix), std::move(boneMatrices))}
{
}

void Render_Object::SetTextures(std::map<TexType, std::vector<clustering_ptr<Texture>>>&& textureMap)
{
	Pimpl().m_TextureMap = std::map<TexType, std::vector<clustering_ptr<Texture>>>(std::move(textureMap));
}

void Render_Object::ResetTextures()
{
	Pimpl().m_TextureMap.clear();
}

const clustering_ptr<glm::mat4> Render_Object::GetModelMatrix() const
{
	return Pimpl().m_ModelMatrix;
}

Render_Object::Render_Object(Render_Object&& rhs) noexcept = default;
Render_Object& Render_Object::operator=(Render_Object&& rhs) noexcept = default;

void Render_Object::RenderObject(clustering_ptr<Shader_Object>& shader, RenderObjectParams&& params)
{
	Pimpl().RenderObject(shader, std::move(params));
}

bool Render_Object::IsTesselated()
{
	return Pimpl().IsTesselated();
}

Render_Object::~Render_Object() noexcept = default;

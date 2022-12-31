#include "pch.h"
#include "Render_Object.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader_Object.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

struct Render_Object::Impl
{
	std::unique_ptr<std::vector<std::weak_ptr<Mesh>>> m_Meshes;
	std::unique_ptr<std::map<TexType, std::vector<std::weak_ptr<Texture>>>> m_TextureMap;
	std::weak_ptr<glm::mat4> m_ModelMatrix;
	std::weak_ptr<glm::mat4> m_PrevModelMatrix;
	std::unique_ptr<std::vector<std::weak_ptr<BoneTransform>>> m_BoneMatrices;

	Impl() = delete;

	Impl(std::unique_ptr<std::vector<std::weak_ptr<Mesh>>>&& meshes
		, std::unique_ptr<std::map<TexType, std::vector<std::weak_ptr<Texture>>>>&& textureMap
		, std::shared_ptr<glm::mat4> modelMatrix
		, std::shared_ptr<glm::mat4> prevModelMatrix
		, std::unique_ptr<std::vector<std::weak_ptr<BoneTransform>>>&& boneMatrices)
		: m_Meshes{ std::move(meshes)}
		, m_TextureMap{ std::move(textureMap) }
		, m_ModelMatrix{ modelMatrix }
		, m_PrevModelMatrix{ prevModelMatrix }
		, m_BoneMatrices{ std::move(boneMatrices) }
	{
	}

	Impl(Impl&& rhs) noexcept = default;
	Impl& operator=(Impl&& rhs) noexcept = default;

	Impl(const Impl& rhs) noexcept = delete;
	Impl& operator=(const Impl& rhs) noexcept = delete;

	~Impl() = default;

	void RenderObject(Shader_Object& shader, RenderObjectParams&& params)
	{
		if (params.useWorldSpaceTransform && !m_ModelMatrix.expired())
		{
			auto modelMatrix = m_ModelMatrix.lock();
			if(modelMatrix)
			{
				shader.SetVariable("Model", *modelMatrix);

				if (params.prevViewProjection != nullptr && !m_PrevModelMatrix.expired())
				{
					auto prevModMat = m_PrevModelMatrix.lock();
					shader.SetVariable("PrevPMV", *params.prevViewProjection * *(prevModMat));
					*prevModMat = *modelMatrix;
				}
			}
		}


		if (!m_Meshes) { return; }

		auto resetToTexUnit = shader.GetTextureUnit();

		if (m_BoneMatrices)
		{
			for (auto i = 0; i < m_BoneMatrices->size(); i++) // move all matrices for actual model position to shader
			{
				auto boneMatrix = m_BoneMatrices->at(i).lock();
				shader.SetVariable("gBones", *boneMatrix->FinalWorldTransform);
				shader.SetVariable("gPrevBones", *boneMatrix->PrevFinalWorldTransfrom);
				*boneMatrix->PrevFinalWorldTransfrom = *boneMatrix->FinalWorldTransform;
			}
		}

		for (size_t i = 0; i < m_Meshes->size(); ++i)
		{
			if(auto mesh = m_Meshes->at(i).lock())
			{
				auto materialIndex = mesh->GetMaterialIndex();

				if (params.useTextures && m_TextureMap)
				{
					for (auto texType = 0; texType < Max; ++texType)
					{
						if (m_TextureMap->contains((TexType)texType))
						{
							if (materialIndex >= m_TextureMap->at((TexType)texType).size()) { continue; }

							if (auto texMap = m_TextureMap->at((TexType)texType)[materialIndex].lock()) 
							{
								switch ((TexType)texType)
								{
								case Albedo:
									texMap->UseTextureTemp(shader.SetTextureUnit("material.albedoMap"));
									break;
								case Metallic:
									texMap->UseTextureTemp(shader.SetTextureUnit("material.metallicMap"));
									break;
								case Roughness:
									texMap->UseTextureTemp(shader.SetTextureUnit("material.roughnessMap"));
									break;
								case Normal:
									texMap->UseTextureTemp(shader.SetTextureUnit("material.normalMap"));
									break;
								case Parallax:
									texMap->UseTextureTemp(shader.SetTextureUnit("material.parallaxMap"));
									break;
								case Glow:
									texMap->UseTextureTemp(shader.SetTextureUnit("material.glowMap"));
									break;
								case Displacement:
									texMap->UseTextureTemp(shader.SetTextureUnit("displacementMap"));
									//ToDo: Add dispFactor through material or something
									shader.SetVariable("dispFactor", 0.2f);
									break;
								case Noise:
									texMap->UseTextureTemp(shader.SetTextureUnit("noise"));
									//ToDo: Add noiseFactor through material or something
									//shader.SetVariable("noiseFactor", 0.2f);
									break;
								default:
									texMap->UseTextureTemp(shader.SetTextureUnit("theTexture"));
									break;
								}
							}
						}
					}
				}

				mesh->RenderMesh();
			}
		}
		shader.ResetTextureUnit(std::move(resetToTexUnit));
	}

	bool IsTesselated() const
	{
		auto check = [&](std::weak_ptr<Mesh> mesh)->bool 
		{
			if (auto m = mesh.lock())
			{
				return m->IsTessellated();
			}
			return false;
		};
		return std::find_if(m_Meshes->begin(), m_Meshes->end(), check) != m_Meshes->end();
	}
};

Render_Object::Render_Object(std::unique_ptr<std::vector<std::weak_ptr<Mesh>>>&& meshes
			, std::unique_ptr<std::map<TexType, std::vector<std::weak_ptr<Texture>>>> textureMap
			, std::shared_ptr<glm::mat4> modelMatrix
			, std::shared_ptr<glm::mat4> prevModelMatrix
			, std::unique_ptr<std::vector<std::weak_ptr<BoneTransform>>>&& boneMatrices)
			: m_pImpl{std::make_unique<Impl>(std::move(meshes), std::move(textureMap), modelMatrix, prevModelMatrix, std::move(boneMatrices))}
{
}

void Render_Object::SetTextures(std::map<TexType, std::vector<std::weak_ptr<Texture>>>&& textureMap)
{
	Pimpl()->m_TextureMap = std::make_unique<std::map<TexType, std::vector<std::weak_ptr<Texture>>>>(std::move(textureMap));
}

void Render_Object::ResetTextures()
{
	Pimpl()->m_TextureMap = nullptr;
}

const std::weak_ptr<glm::mat4>& Render_Object::GetModelMatrix() const
{
	return Pimpl()->m_ModelMatrix;
}

void Render_Object::RenderObject(Shader_Object& shader, RenderObjectParams&& params)
{
	Pimpl()->RenderObject(shader, std::move(params));
}

const bool Render_Object::IsTesselated() const
{
	return Pimpl()->IsTesselated();
}

Render_Object::~Render_Object() = default;

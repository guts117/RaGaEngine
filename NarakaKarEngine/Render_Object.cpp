#include "pch.h"
#include "Render_Object.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader_Object.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

struct Render_Object::Impl
{
	std::weak_ptr<std::vector<Mesh>> m_Meshes;
	std::weak_ptr<std::map<TexType, std::vector<Texture>>> m_TextureMap;
	std::weak_ptr<glm::mat4> m_ModelMatrix;
	std::weak_ptr<glm::mat4> m_PrevModelMatrix;
	std::weak_ptr<std::vector<BoneTransform>> m_BoneMatrices;

	Impl() = delete;

	Impl(std::shared_ptr<std::vector<Mesh>> meshes
		, std::shared_ptr<std::map<TexType, std::vector<Texture>>> textureMap
		, std::shared_ptr<glm::mat4> modelMatrix
		, std::shared_ptr<glm::mat4> prevModelMatrix
		, std::shared_ptr<std::vector<BoneTransform>> boneMatrices)
		: m_Meshes{ meshes }
		, m_TextureMap{ textureMap }
		, m_ModelMatrix{ modelMatrix }
		, m_PrevModelMatrix{ prevModelMatrix }
		, m_BoneMatrices{ boneMatrices }
	{
	}

	Impl(Impl&& rhs) noexcept = default;
	Impl& operator=(Impl&& rhs) noexcept = default;

	Impl(const Impl& rhs) noexcept = delete;
	Impl& operator=(const Impl& rhs) noexcept = delete;

	~Impl() = default;

	void RenderObject(Shader_Object& shader, const RenderObjectParams&& params)
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

		if (auto boneMatrices = m_BoneMatrices.lock()) 
		{
			for (auto i = 0; i < boneMatrices->size(); i++) // move all matrices for actual model position to shader
			{
				shader.SetVariable("gBones", *boneMatrices->at(i).FinalWorldTransform);
				shader.SetVariable("gPrevBones", *boneMatrices->at(i).PrevFinalWorldTransfrom);
				*boneMatrices->at(i).PrevFinalWorldTransfrom = *boneMatrices->at(i).FinalWorldTransform;
			}
		}

		if (auto meshes = m_Meshes.lock()) 
		{
			auto resetToTexUnit = shader.GetTextureUnit();

			auto textureMap = m_TextureMap.lock();

			for (size_t i = 0; i < meshes->size(); ++i)
			{
				auto materialIndex = meshes->at(i)->GetMaterialIndex();

				if (params.useTextures && textureMap)
				{
					for (auto texType = 0; texType < Max; ++texType)
					{
						if (textureMap->contains((TexType)texType))
						{
							if (materialIndex >= textureMap->at((TexType)texType).size()) { continue; }

							switch ((TexType)texType)
							{
							case Albedo:
								textureMap->at(Albedo)[materialIndex]->UseTextureTemp(shader.SetTextureUnit("material.albedoMap"));
								break;
							case Metallic:
								textureMap->at(Metallic)[materialIndex]->UseTextureTemp(shader.SetTextureUnit("material.metallicMap"));
								break;
							case Roughness:
								textureMap->at(Roughness)[materialIndex]->UseTextureTemp(shader.SetTextureUnit("material.roughnessMap"));
								break;
							case Normal:
								textureMap->at(Normal)[materialIndex]->UseTextureTemp(shader.SetTextureUnit("material.normalMap"));
								break;
							case Parallax:
								textureMap->at(Parallax)[materialIndex]->UseTextureTemp(shader.SetTextureUnit("material.parallaxMap"));
								break;
							case Glow:
								textureMap->at(Glow)[materialIndex]->UseTextureTemp(shader.SetTextureUnit("material.glowMap"));
								break;
							case Displacement:
								textureMap->at(Displacement)[materialIndex]->UseTextureTemp(shader.SetTextureUnit("displacementMap"));
								//ToDo: Add dispFactor through material or something
								shader.SetVariable("dispFactor", 0.2f);
								break;
							default:
								textureMap->at(Default)[materialIndex]->UseTextureTemp(shader.SetTextureUnit("theTexture"));
								break;
							}
						}
					}
				}

				meshes->at(i)->RenderMesh();
			}
			shader.ResetTextureUnit(std::move(resetToTexUnit));
		}
	}

	bool IsTesselated() const
	{
		auto check = [&](std::shared_ptr<Mesh> mesh)->bool {return mesh->IsTessellated(); };
		if (auto meshes = m_Meshes.lock()) 
		{
			return std::find_if(meshes->begin(), meshes->end(), check) != meshes->end();
		}
		return false;
	}
};

Render_Object::Render_Object(std::shared_ptr<std::vector<Mesh>> meshes
			, std::shared_ptr<std::map<TexType, std::vector<Texture>>> textureMap
			, std::shared_ptr<glm::mat4> modelMatrix
			, std::shared_ptr<glm::mat4> prevModelMatrix
			, std::shared_ptr<std::vector<BoneTransform>> boneMatrices)
			: m_pImpl{std::make_unique<Impl>(meshes, textureMap, modelMatrix, prevModelMatrix, boneMatrices)}
{
}

void Render_Object::SetTextures(std::map<TexType, std::vector<Texture>>&& textureMap)
{
	Pimpl()->m_TextureMap = std::make_shared<std::map<TexType, std::vector<Texture>>>(std::move(textureMap));
}

void Render_Object::RenderObject(Shader_Object& shader, const RenderObjectParams&& params)
{
	Pimpl()->RenderObject(shader, std::move(params));
}

const bool Render_Object::IsTesselated() const
{
	return Pimpl()->IsTesselated();
}

Render_Object::~Render_Object() = default;

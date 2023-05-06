#include "render_pch.h"
#include "Render_Object.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader_Object.h"

using namespace NarakaRenderEngine;
using namespace RenderEngine;

struct alignas(alignof(void*)) Render_Object::Impl
{
	std::vector<Mesh*> m_Meshes;
	std::vector<BoneTransform*> m_BoneMatrices;
	std::map<TexType, std::vector<Texture*>> m_TextureMap;
	glm::mat4* m_ModelMatrix;
	glm::mat4* m_PrevModelMatrix;

	Impl() = delete;

	Impl(std::vector<Mesh*>&& meshes
		, std::map<TexType, std::vector<Texture*>>&& textureMap
		, glm::mat4* modelMatrix
		, glm::mat4* prevModelMatrix
		, std::vector<BoneTransform*>&& boneMatrices)
		: m_Meshes{ std::move(meshes) }
		, m_BoneMatrices{ std::move(boneMatrices) }
		, m_TextureMap{ std::move(textureMap) }	
		, m_ModelMatrix{ modelMatrix }
		, m_PrevModelMatrix{ prevModelMatrix }
	{
	}

	Impl(Impl&& rhs) noexcept 
		: m_TextureMap{ std::move(rhs.m_TextureMap) }	
		, m_Meshes {std::move(rhs.m_Meshes)}	
		, m_BoneMatrices{ std::move(rhs.m_BoneMatrices) }
		, m_ModelMatrix {std::exchange(rhs.m_ModelMatrix, nullptr)}
		, m_PrevModelMatrix {std::exchange(rhs.m_PrevModelMatrix, nullptr)}	
	{
	};
	Impl& operator=(Impl&& rhs) noexcept
	{
		m_TextureMap = std::move(rhs.m_TextureMap);
		m_Meshes = std::move(rhs.m_Meshes);
		m_BoneMatrices = std::move(rhs.m_BoneMatrices);
		m_ModelMatrix = std::exchange(rhs.m_ModelMatrix, nullptr);
		m_PrevModelMatrix = std::exchange(rhs.m_PrevModelMatrix, nullptr);
		
		return *this;
	};

	Impl(const Impl& rhs) noexcept = delete;
	Impl& operator=(const Impl& rhs) noexcept = delete;

	void RenderObject(Shader_Object& shader, RenderObjectParams&& params) const
	{
		if (params.useWorldSpaceTransform && m_ModelMatrix != nullptr)
		{
			shader.SetVariable("Model", *m_ModelMatrix);

			if (params.prevViewProjection != nullptr && m_PrevModelMatrix != nullptr)
			{
				shader.SetVariable("prevPVM", *params.prevViewProjection * *(m_PrevModelMatrix));
				*m_PrevModelMatrix = *m_ModelMatrix;
			}
		}

		if (!m_Meshes.size()) { return; }

		auto resetToTexUnit = shader.GetTextureUnit();

		for (auto i = 0; i < m_BoneMatrices.size(); i++) // move all matrices for actual model position to shader
		{
			auto& boneMatrix = m_BoneMatrices[i];
			shader.SetVariable("gBones", *boneMatrix->FinalWorldTransform);
			shader.SetVariable("gPrevBones", *boneMatrix->PrevFinalWorldTransfrom);
			*boneMatrix->PrevFinalWorldTransfrom = *boneMatrix->FinalWorldTransform;
		}

		for (size_t i = 0; i < m_Meshes.size(); ++i)
		{
			if(auto mesh = m_Meshes[i])
			{
				auto materialIndex = mesh->GetMaterialIndex();

				if (params.useTextures)
				{
					for (auto texType = 0; texType < Max; ++texType)
					{
						if (m_TextureMap.contains((TexType)texType))
						{
							if (materialIndex >= m_TextureMap.at((TexType)texType).size()) { continue; }

							if (auto texMap = m_TextureMap.at((TexType)texType)[materialIndex]) 
							{
								switch ((TexType)texType)
								{
								case Albedo:
									texMap->UseTextureTemp(shader.SetTextureUnit("material", 0, "albedoMap"));
									break;
								case Metallic:
									texMap->UseTextureTemp(shader.SetTextureUnit("material", 0, "metallicMap"));
									break;
								case Roughness:
									texMap->UseTextureTemp(shader.SetTextureUnit("material", 0, "roughnessMap"));
									break;
								case Normal:
									texMap->UseTextureTemp(shader.SetTextureUnit("material", 0, "normalMap"));
									break;
								case Parallax:
									texMap->UseTextureTemp(shader.SetTextureUnit("material", 0, "parallaxMap"));
									break;
								case Glow:
									texMap->UseTextureTemp(shader.SetTextureUnit("material", 0, "glowMap"));
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
								case HDR:
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
		auto check = [&](Mesh* mesh)->bool 
		{
			if (mesh)
			{
				return mesh->IsTessellated();
			}
			return false;
		};
		return std::find_if(m_Meshes.begin(), m_Meshes.end(), check) != m_Meshes.end();
	}

	~Impl() noexcept = default;
};

Render_Object::Render_Object(std::vector<Mesh*>&& meshes
			, std::map<TexType, std::vector<Texture*>>&& textureMap
			, glm::mat4* modelMatrix
			, glm::mat4* prevModelMatrix
			, std::vector<BoneTransform*>&& boneMatrices)
			: m_pImpl{Impl(std::move(meshes), std::move(textureMap), modelMatrix, prevModelMatrix, std::move(boneMatrices))}
{
}

void Render_Object::SetTextures(std::map<TexType, std::vector<Texture*>>&& textureMap)
{
	Pimpl().m_TextureMap = std::map<TexType, std::vector<Texture*>>(std::move(textureMap));
}

void Render_Object::ResetTextures()
{
	Pimpl().m_TextureMap.clear();
}

const glm::mat4* Render_Object::GetModelMatrix() const
{
	return Pimpl().m_ModelMatrix;
}

Render_Object::Render_Object(Render_Object&& rhs) noexcept = default;
Render_Object& Render_Object::operator=(Render_Object&& rhs) noexcept = default;

void Render_Object::RenderObject(Shader_Object& shader, RenderObjectParams&& params) const
{
	Pimpl().RenderObject(shader, std::move(params));
}

const bool Render_Object::IsTesselated() const
{
	return Pimpl().IsTesselated();
}

Render_Object::~Render_Object() noexcept = default;

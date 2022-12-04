#include "pch.h"
#include "Render_Object.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader_Object.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

struct Render_Object::Impl
{
	std::shared_ptr<std::vector<std::shared_ptr<Mesh>>> m_Meshes;
	std::shared_ptr<std::map<TexType, std::vector<std::shared_ptr<Texture>>>> m_TextureMap;
	std::shared_ptr<glm::mat4> m_ModelMatrix;
	std::shared_ptr<glm::mat4> m_PrevModelMatrix;
	std::shared_ptr<std::vector<BoneTransform>> m_BoneMatrices;

	Impl() = delete;

	Impl(std::shared_ptr<std::vector<std::shared_ptr<Mesh>>> meshes
		, std::shared_ptr<std::map<TexType, std::vector<std::shared_ptr<Texture>>>> textureMap
		, std::shared_ptr<std::vector<BoneTransform>> boneMatrices)
		: m_Meshes{ meshes }
		, m_TextureMap{ textureMap }
		, m_ModelMatrix{ std::make_shared<glm::mat4>(1.0f) }
		, m_PrevModelMatrix{ std::make_shared<glm::mat4>(1.0f) }
		, m_BoneMatrices{ boneMatrices }
	{
	}

	Impl(Impl&& rhs) noexcept = default;
	Impl& operator=(Impl&& rhs) noexcept = default;

	Impl(const Impl& rhs) noexcept = delete;
	Impl& operator=(const Impl& rhs) noexcept = delete;

	~Impl() = default;

	void RenderObject(std::shared_ptr<Shader_Object> shader, const glm::mat4& prevVP, bool&& hasModelMatrix)
	{
		if (hasModelMatrix) 
		{
			shader->SetVariable("Model", *m_ModelMatrix);
			shader->SetVariable("PrevPMV", prevVP * *(m_PrevModelMatrix));
			*m_PrevModelMatrix = *m_ModelMatrix;
		}

		if (m_BoneMatrices != nullptr) 
		{
			for (auto i = 0; i < m_BoneMatrices->size(); i++) // move all matrices for actual model position to shader
			{
				shader->SetVariable("gBones", *m_BoneMatrices->at(i).FinalWorldTransform);
				shader->SetVariable("gPrevBones", *m_BoneMatrices->at(i).PrevFinalWorldTransfrom);
				*m_BoneMatrices->at(i).PrevFinalWorldTransfrom = *m_BoneMatrices->at(i).FinalWorldTransform;
			}
		}

		auto resetToTexUnit = shader->GetTextureUnit();

		for (size_t i = 0; i < m_Meshes->size(); ++i) {
			auto materialIndex = m_Meshes->at(i)->GetMaterialIndex();

			if (m_TextureMap != nullptr && materialIndex < m_TextureMap->size() && m_TextureMap->at(Albedo)[materialIndex]) {
				m_TextureMap->at(Albedo)[materialIndex]->UseTexture(shader->SetTextureUnit("material.albedoMap"));
				m_TextureMap->at(Metallic)[materialIndex]->UseTexture(shader->SetTextureUnit("material.metallicMap"));
				m_TextureMap->at(Roughness)[materialIndex]->UseTexture(shader->SetTextureUnit("material.roughnessMap"));
				m_TextureMap->at(Normal)[materialIndex]->UseTexture(shader->SetTextureUnit("material.normalMap"));
				m_TextureMap->at(Parallax)[materialIndex]->UseTexture(shader->SetTextureUnit("material.parallaxMap"));
				m_TextureMap->at(Glow)[materialIndex]->UseTexture(shader->SetTextureUnit("material.glowMap"));
				//m_TextureMap->at(Displacement)[materialIndex]->UseTexture(shader->SetTextureUnit("displacementMap"));
				//shader->SetVariable("dispFactor", );
			}

			m_Meshes->at(i)->RenderMesh();
		}
		shader->ResetTextureUnit(std::move(resetToTexUnit));
	}
};

Render_Object::Render_Object(std::shared_ptr<std::vector<std::shared_ptr<Mesh>>> meshes
			, std::shared_ptr<std::map<TexType, std::vector<std::shared_ptr<Texture>>>> textureMap
			, std::shared_ptr<std::vector<BoneTransform>> boneMatrices)
			: m_pImpl{std::make_unique<Impl>(meshes, textureMap, boneMatrices)}
{
}

void Render_Object::RenderObject(std::shared_ptr<Shader_Object> shader, const glm::mat4& prevPV, bool&& hasModelMatrix)
{
	Pimpl()->RenderObject(shader, prevPV, std::move(hasModelMatrix));
}

Render_Object::~Render_Object() = default;

#include "pch.h"
#include "Scene_Render_Pass_Handler.h"
#include "Fbo_Handler.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

struct Scene_Render_Pass_Handler::Impl
{
	std::shared_ptr<Fbo_Handler> m_fboHandler;
	std::unique_ptr<std::vector<std::shared_ptr<Shader_Object>>> m_shaderVec;
	std::vector<std::shared_ptr<std::any>> m_inputs;
	std::vector<std::shared_ptr<std::any>> m_outputs;

	Impl() = delete;

	Impl(Impl&& rhs) noexcept = default;
	Impl& operator=(Impl&& rhs) noexcept = default;

	Impl(const Impl& rhs) noexcept = delete;
	Impl& operator=(const Impl& rhs) noexcept = delete;

	~Impl() = default;
};

Scene_Render_Pass_Handler::~Scene_Render_Pass_Handler() = default;

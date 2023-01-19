#include "render_pch.h"
#include "Scene_Handler.h"
#include "Render_Pass_Handler.h"

using namespace NarakaRenderEngine;
using namespace RenderEngine;

struct Scene_Handler::Impl
{
	std::unique_ptr<std::vector<std::shared_ptr<Render_Pass_Handler>>> m_renderPasses;

	Impl() = delete;

	Impl(Impl&& rhs) noexcept = default;
	Impl& operator=(Impl&& rhs) noexcept = default;

	Impl(const Impl& rhs) noexcept = delete;
	Impl& operator=(const Impl& rhs) noexcept = delete;

	~Impl() = default;
};

Scene_Handler::~Scene_Handler() = default;
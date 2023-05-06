#ifndef SCENE_RENDER_PASS_HANDLER
#define SCENE_RENDER_PASS_HANDLER

#include "Render_Pass_Handler.h"

namespace NarakaRenderEngine
{
	namespace RenderEngine
	{
		class Render_Object;
		class Shader_Object;
		class Fbo_Handler;

		class Scene_Render_Pass_Handler final :
			public Render_Pass_Handler
		{
		public:
			explicit Scene_Render_Pass_Handler() = delete;
			explicit Scene_Render_Pass_Handler(Fbo_Handler* fboHandlr
				, std::vector<Shader_Object*>&& shaderVec
				, std::shared_ptr<std::vector<std::shared_ptr<std::any>>> inputs = nullptr);

			virtual void Update(const std::vector<std::vector<Render_Object>>& renderObj, const CamParam* camParam = nullptr, const LightParam* lightParam = nullptr) override;

			Scene_Render_Pass_Handler(Scene_Render_Pass_Handler&& rhs) noexcept = default;
			Scene_Render_Pass_Handler& operator=(Scene_Render_Pass_Handler&& rhs) noexcept = default;

			Scene_Render_Pass_Handler(const Scene_Render_Pass_Handler& rhs) noexcept = delete;
			Scene_Render_Pass_Handler& operator=(const Scene_Render_Pass_Handler& rhs) noexcept = delete;

			~Scene_Render_Pass_Handler();
		};
	}
}
#endif

#ifndef EXPOSURE_RENDER_PASS_HANDLER
#define EXPOSURE_RENDER_PASS_HANDLER

#include "Render_Pass_Handler.h"

namespace NarakaRenderEngine
{
	namespace RenderEngine
	{
		class Render_Object;
		class Shader_Object;
		class Fbo_Handler;

		class Exposure_Render_Pass_Handler final :
			public Render_Pass_Handler
		{
		public:
			explicit Exposure_Render_Pass_Handler() = delete;
			explicit Exposure_Render_Pass_Handler(Fbo_Handler* fboHandlr
				, const std::vector<std::shared_ptr<Shader_Object>>& shaderVec
				, std::shared_ptr<std::vector<std::shared_ptr<std::any>>> inputs = nullptr);

			virtual void Update(const std::vector<std::vector<std::shared_ptr<Render_Object>>>& renderObj, const CamParam* camParam = nullptr, const LightParam* lightParam = nullptr) override;

			Exposure_Render_Pass_Handler(Exposure_Render_Pass_Handler&& rhs) noexcept = default;
			Exposure_Render_Pass_Handler& operator=(Exposure_Render_Pass_Handler&& rhs) noexcept = default;

			Exposure_Render_Pass_Handler(const Exposure_Render_Pass_Handler& rhs) noexcept = delete;
			Exposure_Render_Pass_Handler& operator=(const Exposure_Render_Pass_Handler& rhs) noexcept = delete;

			~Exposure_Render_Pass_Handler();
		};
	}
}

#endif


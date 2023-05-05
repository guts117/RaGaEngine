#ifndef MOTION_BLUR_RENDER_PASS_HANDLER
#define MOTION_BLUR_RENDER_PASS_HANDLER

#include "Render_Pass_Handler.h"

namespace NarakaRenderEngine
{
	namespace RenderEngine
	{
		class Render_Object;
		class Shader_Object;
		class Fbo_Handler;

		class Motion_Blur_Render_Pass_Handler final :
			public Render_Pass_Handler
		{
		public:
			explicit Motion_Blur_Render_Pass_Handler() = delete;
			explicit Motion_Blur_Render_Pass_Handler(Fbo_Handler* fboHandlr
				, const std::vector<std::shared_ptr<Shader_Object>>& shaderVec
				, std::shared_ptr<std::vector<std::shared_ptr<std::any>>> inputs = nullptr);

			virtual void Update(const std::vector<std::vector<Render_Object>>& renderObj, const CamParam* camParam = nullptr, const LightParam* lightParam = nullptr) override;

			Motion_Blur_Render_Pass_Handler(Motion_Blur_Render_Pass_Handler&& rhs) noexcept = default;
			Motion_Blur_Render_Pass_Handler& operator=(Motion_Blur_Render_Pass_Handler&& rhs) noexcept = default;

			Motion_Blur_Render_Pass_Handler(const Motion_Blur_Render_Pass_Handler& rhs) noexcept = delete;
			Motion_Blur_Render_Pass_Handler& operator=(const Motion_Blur_Render_Pass_Handler& rhs) noexcept = delete;

			~Motion_Blur_Render_Pass_Handler();
		};
	}
}

#endif


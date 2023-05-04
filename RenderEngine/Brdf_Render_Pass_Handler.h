#ifndef BRDF_SHADER_PASS
#define BRDF_SHADER_PASS

#include "Render_Pass_Handler.h"

namespace NarakaRenderEngine
{
	namespace RenderEngine
	{
		class Render_Object;
		class Shader_Object;
		class Fbo_Handler;

		class Brdf_Render_Pass_Handler final :
			public Render_Pass_Handler
		{
		public:
			explicit Brdf_Render_Pass_Handler() = delete;
			explicit Brdf_Render_Pass_Handler(Fbo_Handler* fboHandlr
				, const std::vector<std::shared_ptr<Shader_Object>>& shaderVec
				, std::shared_ptr<std::vector<std::shared_ptr<std::any>>> inputs = nullptr);

			virtual void Update(const std::vector<std::vector<std::shared_ptr<Render_Object>>>& renderObj, const CamParam* camParam = nullptr, const LightParam* lightParam = nullptr) override;

			Brdf_Render_Pass_Handler(Brdf_Render_Pass_Handler&& rhs) noexcept = default;
			Brdf_Render_Pass_Handler& operator=(Brdf_Render_Pass_Handler&& rhs) noexcept = default;

			Brdf_Render_Pass_Handler(const Brdf_Render_Pass_Handler& rhs) noexcept = delete;
			Brdf_Render_Pass_Handler& operator=(const Brdf_Render_Pass_Handler& rhs) noexcept = delete;

			~Brdf_Render_Pass_Handler();
		};
	}
}
#endif
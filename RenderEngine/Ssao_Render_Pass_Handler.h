#ifndef SSAO_RENDER_PASS_HANDLER
#define SSAO_RENDER_PASS_HANDLER

#include "Render_Pass_Handler.h"

namespace NarakaRenderEngine
{
	namespace RenderEngine
	{
		class Render_Object;
		class Shader_Object;
		class Fbo_Handler;

		class Ssao_Render_Pass_Handler final :
			public Render_Pass_Handler
		{
		public:
			explicit Ssao_Render_Pass_Handler() = delete;
			explicit Ssao_Render_Pass_Handler(Fbo_Handler* fboHandlr
				, std::vector<rw_clustering_ptr<Shader_Object>>&& shaderVec
				, std::shared_ptr<std::vector<std::shared_ptr<std::any>>> inputs = nullptr);

			virtual void Init() override;
			virtual void Update(std::vector<std::vector<Render_Object>>& renderObj, const CamParam* camParam = nullptr, const LightParam* lightParam = nullptr) override;

			Ssao_Render_Pass_Handler(Ssao_Render_Pass_Handler&& rhs) noexcept = default;
			Ssao_Render_Pass_Handler& operator=(Ssao_Render_Pass_Handler&& rhs) noexcept = default;

			Ssao_Render_Pass_Handler(const Ssao_Render_Pass_Handler& rhs) noexcept = delete;
			Ssao_Render_Pass_Handler& operator=(const Ssao_Render_Pass_Handler& rhs) noexcept = delete;

			~Ssao_Render_Pass_Handler();
		};
	}
}

#endif


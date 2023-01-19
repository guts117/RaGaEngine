#ifndef BLOOM_RENDER_PASS_HANDLER
#define BLOOM_RENDER_PASS_HANDLER

#include "Render_Pass_Handler.h"

namespace NarakaRenderEngine
{
	namespace RenderEngine
	{
		class Render_Object;
		class Shader_Object;
		class Fbo_Handler;

		class Bloom_Render_Pass_Handler final :
			public Render_Pass_Handler
		{
		public:
			explicit Bloom_Render_Pass_Handler() = delete;
			explicit Bloom_Render_Pass_Handler(std::shared_ptr<Fbo_Handler> fboHandlr
				, const std::vector<std::shared_ptr<Shader_Object>>& shaderVec
				, std::shared_ptr<std::vector<std::shared_ptr<std::any>>> inputs = nullptr);

			virtual void Update(const std::vector<std::vector<std::shared_ptr<Render_Object>>>& renderObj, const CamParam* camParam = nullptr, const LightParam* lightParam = nullptr) override;

			Bloom_Render_Pass_Handler(Bloom_Render_Pass_Handler&& rhs) noexcept = default;
			Bloom_Render_Pass_Handler& operator=(Bloom_Render_Pass_Handler&& rhs) noexcept = default;

			Bloom_Render_Pass_Handler(const Bloom_Render_Pass_Handler& rhs) noexcept = delete;
			Bloom_Render_Pass_Handler& operator=(const Bloom_Render_Pass_Handler& rhs) noexcept = delete;

			~Bloom_Render_Pass_Handler();
		};
	}
}

#endif

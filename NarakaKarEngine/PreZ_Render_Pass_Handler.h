#ifndef PREZ_RENDER_PASS_HANDLER
#define PREZ_RENDER_PASS_HANDLER

#include "Render_Pass_Handler.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class Render_Object;
		class Shader_Object;
		class Fbo_Handler;

		class PreZ_Render_Pass_Handler final:
			public Render_Pass_Handler
		{
		public:
			explicit PreZ_Render_Pass_Handler() = delete;
			explicit PreZ_Render_Pass_Handler(std::shared_ptr<Fbo_Handler> fboHandlr
												, const std::vector<std::shared_ptr<Shader_Object>>& shaderVec
												, std::shared_ptr<std::vector<std::shared_ptr<std::any>>> inputs = nullptr);

			virtual void Update(const std::vector<std::vector<std::shared_ptr<Render_Object>>>& renderObj, const CamParam* camParam = nullptr, const LightParam* lightParam = nullptr) override;

			PreZ_Render_Pass_Handler(PreZ_Render_Pass_Handler&& rhs) noexcept = default;
			PreZ_Render_Pass_Handler& operator=(PreZ_Render_Pass_Handler&& rhs) noexcept = default;

			PreZ_Render_Pass_Handler(const PreZ_Render_Pass_Handler& rhs) noexcept = delete;
			PreZ_Render_Pass_Handler& operator=(const PreZ_Render_Pass_Handler& rhs) noexcept = delete;

			~PreZ_Render_Pass_Handler();
		};
	}
}

#endif
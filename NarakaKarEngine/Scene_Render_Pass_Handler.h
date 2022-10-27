#ifndef SCENE_RENDER_PASS_HANDLER
#define SCENE_RENDER_PASS_HANDLER

#include "pch.h"

//Handles rendering 

namespace NarakaKarEngine
{
	namespace RenderEngine
	{	
		class Render_Object;
		class Shader_Object;
		class Fbo_Handler;

		struct CamParam
		{
			const glm::vec3& Position;
			const glm::mat4& Projection;
			const glm::mat4& View;
			const glm::mat4& PrevProjView;
		};

		struct ShaderParam
		{
			std::shared_ptr<Shader_Object> shader;
			std::shared_ptr<std::vector<std::any>> shaderInputs;
		};

		class Scene_Render_Pass_Handler
		{
		public:
			explicit Scene_Render_Pass_Handler() = delete;

			explicit Scene_Render_Pass_Handler(std::shared_ptr<Fbo_Handler> fboHandlr
											, std::shared_ptr<ShaderParam> shaderParam);

			Scene_Render_Pass_Handler(Scene_Render_Pass_Handler&& rhs) noexcept = default;
			Scene_Render_Pass_Handler& operator=(Scene_Render_Pass_Handler&& rhs) noexcept = default;

			Scene_Render_Pass_Handler(const Scene_Render_Pass_Handler& rhs) noexcept = delete;
			Scene_Render_Pass_Handler& operator=(const Scene_Render_Pass_Handler& rhs) noexcept = delete;

			virtual void Update(std::shared_ptr<std::vector<Render_Object>> renderObj, const CamParam& camParam);

			virtual ~Scene_Render_Pass_Handler() = 0;

		private:
			struct Impl;

			const Impl* Pimpl() const { return m_pImpl.get(); }
			Impl* Pimpl() { return m_pImpl.get(); }

			std::unique_ptr<Impl> m_pImpl;
		};
	}
}

#endif

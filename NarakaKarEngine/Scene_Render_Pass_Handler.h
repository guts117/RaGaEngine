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
												, const std::vector<std::shared_ptr<Shader_Object>>& shaderVec
												, std::shared_ptr<std::vector<std::shared_ptr<std::any>>> inputs = nullptr);

			explicit Scene_Render_Pass_Handler(std::shared_ptr<Fbo_Handler> fboHandlr
											, std::shared_ptr<ShaderParam> shaderParam);

			Scene_Render_Pass_Handler(Scene_Render_Pass_Handler&& rhs) noexcept = default;
			Scene_Render_Pass_Handler& operator=(Scene_Render_Pass_Handler&& rhs) noexcept = default;

			Scene_Render_Pass_Handler(const Scene_Render_Pass_Handler& rhs) noexcept = delete;
			Scene_Render_Pass_Handler& operator=(const Scene_Render_Pass_Handler& rhs) noexcept = delete;

			virtual void Update(std::shared_ptr<std::vector<std::shared_ptr<Render_Object>>> renderObj, const CamParam& camParam) = 0;

			virtual ~Scene_Render_Pass_Handler() = 0;

		protected:
			std::shared_ptr<Fbo_Handler> m_fboHandler;
			std::unique_ptr<std::vector<std::shared_ptr<Shader_Object>>> m_shaderVec;
			std::shared_ptr<std::vector<std::shared_ptr<std::any>>> m_inputs;
			std::shared_ptr<std::vector<std::shared_ptr<GLint>>> m_outputs;
		};
	}
}

#endif

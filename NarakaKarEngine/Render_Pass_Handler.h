#ifndef RENDER_PASS_HANDLER
#define RENDER_PASS_HANDLER

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
			const float fps;
		};

		struct LightParam
		{
			const glm::vec3* Position = nullptr;
			const glm::mat4* Projection = nullptr;
			const glm::mat4* View = nullptr;
			const glm::vec3* Direction = nullptr;
			const GLfloat* FarPlane = nullptr;
			const GLfloat* Edge = nullptr;  //CascadeEnd
			int Count = 0;
			glm::vec4* Color = nullptr;
		};

		struct ShaderParam
		{
			std::shared_ptr<Shader_Object> shader;
			std::shared_ptr<std::vector<std::any>> shaderInputs;
		};

		class Render_Pass_Handler
		{
		public:
			explicit Render_Pass_Handler() = delete;
			explicit Render_Pass_Handler(std::shared_ptr<Fbo_Handler> fboHandlr
												, const std::vector<std::shared_ptr<Shader_Object>>& shaderVec
												, std::shared_ptr<std::vector<std::shared_ptr<std::any>>> inputs = nullptr);

			Render_Pass_Handler(Render_Pass_Handler&& rhs) noexcept = default;
			Render_Pass_Handler& operator=(Render_Pass_Handler&& rhs) noexcept = default;

			Render_Pass_Handler(const Render_Pass_Handler& rhs) noexcept = delete;
			Render_Pass_Handler& operator=(const Render_Pass_Handler& rhs) noexcept = delete;
	
			virtual void Init();
			virtual void Update(const std::vector<std::vector<std::shared_ptr<Render_Object>>>& renderObj, const CamParam* camParam = nullptr, const LightParam* lightParam = nullptr) = 0;

			virtual ~Render_Pass_Handler() = 0;

		protected:

			template <typename T> std::shared_ptr<T> CheckInputDataType(const std::any& data);

			std::shared_ptr<Fbo_Handler> m_fboHandler;
			std::unique_ptr<std::vector<std::shared_ptr<Shader_Object>>> m_shaderVec;
			std::shared_ptr<std::vector<std::shared_ptr<std::any>>> m_inputs;
			std::shared_ptr<std::vector<std::shared_ptr<GLint>>> m_outputs;
		};
	}
}

#endif

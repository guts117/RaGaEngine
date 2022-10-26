#ifndef SCENE_RENDER_PASS_HANDLER
#define SCENE_RENDER_PASS_HANDLER

#include "pch.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class Render_Object;
		class Shader_Object;
		class Camera;

		class CamParam
		{
			const glm::vec3& Position;
			const glm::mat4& Projection;
			const glm::mat4& View;
			const glm::mat4& PrevProjView;
		};

		class Scene_Render_Pass_Handler
		{
		public:
			explicit Scene_Render_Pass_Handler() = delete;

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

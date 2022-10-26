#ifndef SCENE_RENDER_PASS_HANDLER
#define SCENE_RENDER_PASS_HANDLER


namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class Shader_Object;

		class Scene_Render_Pass_Handler
		{
		public:
			explicit Scene_Render_Pass_Handler() = delete;
			explicit Scene_Render_Passs_Handler();

			Scene_Render_Pass_Handler(Scene_Render_Pass_Handler&& rhs) noexcept = default;
			Scene_Render_Pass_Handler& operator=(Scene_Render_Pass_Handler&& rhs) noexcept = default;

			Scene_Render_Pass_Handler(const Scene_Render_Pass_Handler& rhs) noexcept = delete;
			Scene_Render_Pass_Handler& operator=(const Scene_Render_Pass_Handler& rhs) noexcept = delete;

			virtual void Update();

			virtual ~Scene_Render_Pass_Handler() = 0;
		protected:
			struct Impl;

			const Impl* Pimpl() const { return m_pImpl.get(); }
			Impl* Pimpl() { return m_pImpl.get(); }

			std::unique_ptr<Impl> m_pImpl;
		};
	}
}

#endif

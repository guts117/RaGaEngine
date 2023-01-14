#ifndef SCENE_HANDLER
#define SCENE_HANDLER

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class Scene_Handler
		{
		public:
			explicit Scene_Handler() = delete;

			Scene_Handler(Scene_Handler&& rhs) noexcept = default;
			Scene_Handler& operator=(Scene_Handler&& rhs) noexcept = default;

			Scene_Handler(const Scene_Handler& rhs) noexcept = delete;
			Scene_Handler& operator=(const Scene_Handler& rhs) noexcept = delete;

			~Scene_Handler();
		protected:
			struct Impl;

			const Impl* Pimpl() const { return m_pImpl.get(); }
			Impl* Pimpl() { return m_pImpl.get(); }

			std::unique_ptr<Impl> m_pImpl;
		};
	}
}
#endif

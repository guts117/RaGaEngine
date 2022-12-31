#ifndef RENDER_ENGINE_MAIN
#define RENDER_ENGINE_MAIN

#include "pch.h"
//#include "spimpl.h"

namespace NarakaKarEngine
{
	namespace RenderEngine
	{
		class Render_Object;
		class Texture;
		enum TexType;

		struct Transform
		{
			glm::vec3 Position;
			glm::quat Rotation;
			glm::vec3 Scale;
		};

		struct VObject
		{
			std::unique_ptr<Transform> transform;
			std::weak_ptr<Render_Object> render_object;
		};

		struct TexMapData
		{
			TexType&& type;
			std::string&& path;
		};

		class RenderEngineMain
		{
		public:
			explicit RenderEngineMain();

			void Update();
			void EndUpdate();

			GLFWwindow* GetMainWindow();
			void AddViewers();
			//ToDo:
			//static std::unique_ptr<std::map<TexType, std::vector<std::weak_ptr<Texture>>>> CreateTextureMap(std::vector<TexMapData>&& texMapData);

			bool IsEnd();

			~RenderEngineMain();

			RenderEngineMain(RenderEngineMain&& rhs) noexcept = delete;
			RenderEngineMain& operator=(RenderEngineMain&& rhs) noexcept = delete;

			RenderEngineMain(const RenderEngineMain& rhs) noexcept = delete;
			RenderEngineMain& operator=(const RenderEngineMain& rhs) noexcept = delete;
		private:
			struct Impl;

			const Impl* Pimpl() const { return m_pImpl.get(); }
			Impl* Pimpl() { return m_pImpl.get(); }

			std::unique_ptr<Impl> m_pImpl;
		};
	}
}
#endif
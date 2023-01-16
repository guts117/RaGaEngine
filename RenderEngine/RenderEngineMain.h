#ifndef RENDER_ENGINE_MAIN
#define RENDER_ENGINE_MAIN

#include "render_pch.h"
//#include "spimpl.h"

#ifdef RENDER_ENGINE_EXPORTS
#define RENDER_ENGINE_API __declspec(dllexport)
#else
#define RENDER_ENGINE_API __declspec(dllimport)
#endif

namespace NarakaKarEngine
{
	namespace EngineUI
	{
		class EngineUIMain;
	}

	namespace RenderEngine
	{
		class Render_Object;
		class Texture;
		class Scene_Fbo_Handler_Manager;
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

		class RENDER_ENGINE_API RenderEngineMain
		{
		public:
			explicit RenderEngineMain(const glm::ivec2& screenDims);

			void Update(const glm::ivec2& screenDims, const bool& isUpdateBuffers);
			void EndUpdate();

			const GLuint& GetFboBuffer(const std::string& fboHandlerName, const GLuint& fboIndex, const GLuint& bufferIndex)  const;

			//ToDo:
			//static std::unique_ptr<std::map<TexType, std::vector<std::weak_ptr<Texture>>>> CreateTextureMap(std::vector<TexMapData>&& texMapData);

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
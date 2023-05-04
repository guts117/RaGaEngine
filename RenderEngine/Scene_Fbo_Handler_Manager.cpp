#include "render_pch.h"
#include "Scene_Fbo_Handler_Manager.h"
#include "FrameBufferObject.h"
#include "Fbo_Handler.h"
#include "RenderingCommonValues.h"
#include <cereal/access.hpp>

using namespace NarakaRenderEngine;
using namespace RenderEngine;

struct Scene_Fbo_Handler_Manager::Impl
{
	std::vector<Fbo_Handler> m_FboHandlerVec;
	std::string m_SceneName;

	Impl() = delete;

	Impl(const std::string& sceneName, const glm::ivec2& screenDims)
		: m_FboHandlerVec{ std::vector<Fbo_Handler>() }
		, m_SceneName{ sceneName }
	{
		//m_FboHandlerVec.reserve(100);
		GLuint ScreenWidth = screenDims.x;
		GLuint ScreenHeight = screenDims.y;

		//ToDo: Read from json and load all of them 
		//ToDo: read buffer width and height from the json file.
		//ToDo: Use a memory pool instead
		m_FboHandlerVec.reserve(20);

//Brdf Pass
		{
			auto fboTexParams = std::vector<FBOTexParams>();
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_MIN_FILTER,	GL_LINEAR });
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_MAG_FILTER,	GL_LINEAR });
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_WRAP_S,		GL_CLAMP_TO_EDGE });
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_WRAP_T,		GL_CLAMP_TO_EDGE });

			FBOTexGenParams fboTexGenParams{ 1, GL_TEXTURE_2D, 0, GL_RG16F, 0, GL_RG, GL_FLOAT, NULL, std::move(fboTexParams) };
			auto fbo = FrameBufferObject(FBOParams{ false, ScreenWidth, ScreenWidth, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, std::vector<FBOTexGenParams>{std::move(fboTexGenParams)} });
			auto fboVec = std::vector<FrameBufferObject>();
			fboVec.emplace_back(std::move(fbo));
			auto fboHandlr = Fbo_Handler(std::move(fboVec), "Brdf_Pass", false, 0, 0, nullptr, nullptr);
			m_FboHandlerVec.emplace_back(std::move(fboHandlr));
		}

//Environment Map Pass
		{
			auto fboTexParams = std::vector<FBOTexParams>();
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_MIN_FILTER,		GL_LINEAR_MIPMAP_LINEAR });
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_MAG_FILTER,		GL_LINEAR });
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_WRAP_S,			GL_CLAMP_TO_EDGE });
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_WRAP_T,			GL_CLAMP_TO_EDGE });
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_WRAP_R,			GL_CLAMP_TO_EDGE });

			FBOTexGenParams fboTexGenParams{ 1, GL_TEXTURE_CUBE_MAP, 0, GL_RGB16F, 0, GL_RGB, GL_FLOAT, NULL, std::move(fboTexParams) };
			FBORenderBufferParam fboRBParams{ GL_DEPTH_COMPONENT24 , GL_DEPTH_ATTACHMENT };
			auto fbo = FrameBufferObject(FBOParams{ true, ScreenWidth, ScreenWidth, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, std::vector<FBOTexGenParams>{std::move(fboTexGenParams)}, std::vector<FBORenderBufferParam>{ std::move(fboRBParams) } });
			auto fboVec = std::vector<FrameBufferObject>();
			fboVec.emplace_back(std::move(fbo));
			auto fboHandlr = Fbo_Handler(std::move(fboVec), "Environment_Map_Pass", false, 0, 0, nullptr, nullptr);
			m_FboHandlerVec.emplace_back(std::move(fboHandlr));
		}

//Irradiance Map Pass
		{
			auto fboTexParams = std::vector<FBOTexParams>();
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_MIN_FILTER,		GL_LINEAR });
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_MAG_FILTER,		GL_LINEAR });
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_WRAP_S,			GL_CLAMP_TO_EDGE });
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_WRAP_T,			GL_CLAMP_TO_EDGE });
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_WRAP_R,			GL_CLAMP_TO_EDGE });

			FBOTexGenParams fboTexGenParams { 1, GL_TEXTURE_CUBE_MAP, 0, GL_RGB16F, 0, GL_RGB, GL_FLOAT, NULL, std::move(fboTexParams) };
			FBORenderBufferParam fboRBParams { GL_DEPTH_COMPONENT24 , GL_DEPTH_ATTACHMENT };
			auto fbo = FrameBufferObject(FBOParams{ true, 32, 32, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, std::vector<FBOTexGenParams>{std::move(fboTexGenParams)}, std::vector<FBORenderBufferParam>{ std::move(fboRBParams) } });
			auto fboVec = std::vector<FrameBufferObject>();
			fboVec.emplace_back(std::move(fbo));
			auto fboHandlr = Fbo_Handler(std::move(fboVec), "Irradiance_Map_Pass", false, 0, 0, nullptr, nullptr);
			m_FboHandlerVec.emplace_back(std::move(fboHandlr));
		}

//Pre Filter Pass
		{
			auto fboTexParams = std::vector<FBOTexParams>();
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_MIN_FILTER,		GL_LINEAR_MIPMAP_LINEAR });
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_MAG_FILTER,		GL_NEAREST });
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_WRAP_S,			GL_CLAMP_TO_EDGE });
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_WRAP_T,			GL_CLAMP_TO_EDGE });
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_WRAP_R,			GL_CLAMP_TO_EDGE });

			FBOTexGenParams fboTexGenParams { 1, GL_TEXTURE_CUBE_MAP, 0, GL_RGB16F, 0, GL_RGB, GL_FLOAT, NULL, std::move(fboTexParams) };
			FBORenderBufferParam fboRBParams { GL_DEPTH_COMPONENT24 , GL_DEPTH_ATTACHMENT };
			auto fbo = FrameBufferObject(FBOParams{ true, 128, 128, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, std::vector<FBOTexGenParams>{std::move(fboTexGenParams)}, std::vector<FBORenderBufferParam>{ std::move(fboRBParams) } });
			fbo.CreateMipMap(0, 0);
			auto fboVec = std::vector<FrameBufferObject>();
			fboVec.emplace_back(std::move(fbo));
			auto fboHandlr = Fbo_Handler(std::move(fboVec), "Pre_Filter_Pass", false, 0, 0, nullptr, nullptr);
			m_FboHandlerVec.emplace_back(std::move(fboHandlr));
		}

//Depth Pass
		{
			auto fboTexParams = std::vector<FBOTexParams>();
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_MIN_FILTER,		GL_LINEAR });
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_MAG_FILTER,		GL_LINEAR });
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_WRAP_S,			GL_CLAMP_TO_EDGE });
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_WRAP_T,			GL_CLAMP_TO_EDGE });
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_COMPARE_MODE,	GL_NONE });

			FBOTexGenParams fboTexGenParams { 1, GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL, std::move(fboTexParams) };
			auto fbo = FrameBufferObject(FBOParams{ false, ScreenWidth, ScreenHeight, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, std::vector<FBOTexGenParams>{std::move(fboTexGenParams)} });
			auto fboVec = std::vector<FrameBufferObject>();
			fboVec.emplace_back(std::move(fbo));
			auto fboHandlr = Fbo_Handler(std::move(fboVec), "Depth_Pass", true, 0, 0, nullptr, nullptr);
			m_FboHandlerVec.emplace_back(std::move(fboHandlr));
		}

//Shadow Map Pass
		{
			auto fboTexParams = std::vector<FBOTexParams>();
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_MIN_FILTER,		GL_NEAREST });
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_MAG_FILTER,		GL_NEAREST });
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_COMPARE_MODE,	GL_NONE });
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_WRAP_S,			GL_CLAMP_TO_BORDER });
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_WRAP_T,			GL_CLAMP_TO_BORDER });
			float bColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_BORDER_COLOR,	bColor });

			FBOTexGenParams fboTexGenParams{ NUM_CASCADES, GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL, std::move(fboTexParams) };
			auto fbo = FrameBufferObject(FBOParams{ false, 1024, 1024, GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, std::vector<FBOTexGenParams>{std::move(fboTexGenParams)} });
			auto fboVec = std::vector<FrameBufferObject>();
			fboVec.emplace_back(std::move(fbo));
			auto fboHandlr = Fbo_Handler(std::move(fboVec), "Shadow_Map_Pass", false, 0, 0, nullptr, nullptr);
			m_FboHandlerVec.emplace_back(std::move(fboHandlr));
		}

//Omni Shadow Map Pass
		{
			auto fboVec = std::vector<FrameBufferObject>();
			for (unsigned int i = 0; i < 3; ++i)
			{
				auto fboTexParams = std::vector<FBOTexParams>();
				fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_MIN_FILTER,		GL_NEAREST });
				fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_MAG_FILTER,		GL_NEAREST });
				fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_WRAP_S,			GL_CLAMP_TO_EDGE });
				fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_WRAP_T,			GL_CLAMP_TO_EDGE });
				fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_WRAP_R,			GL_CLAMP_TO_EDGE });

				FBOTexGenParams fboTexGenParams{ 1, GL_TEXTURE_CUBE_MAP, 0, GL_DEPTH_COMPONENT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL, std::move(fboTexParams) };
				auto fbo = FrameBufferObject(FBOParams{ false, 512, 512, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, std::vector<FBOTexGenParams>{std::move(fboTexGenParams)} });
				fboVec.emplace_back(std::move(fbo));
			}
			auto fboHandlr = Fbo_Handler(std::move(fboVec), "Omni_Shadow_Map_Pass", false, 0, 0, nullptr, nullptr);
			m_FboHandlerVec.emplace_back(std::move(fboHandlr));
		}

//Ssao Pass
		{
			auto fboTexParams = std::vector<FBOTexParams>();
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_MIN_FILTER,		GL_LINEAR });
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_MAG_FILTER,		GL_LINEAR });
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_WRAP_S,			GL_CLAMP_TO_EDGE });
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_WRAP_T,			GL_CLAMP_TO_EDGE });

			FBOTexGenParams fboTexGenParams { 1, GL_TEXTURE_2D, 0, GL_R32F, 0, GL_RED, GL_FLOAT, NULL, std::move(fboTexParams)};
			auto fbo = FrameBufferObject(FBOParams{ false, ScreenWidth, ScreenHeight, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, std::vector<FBOTexGenParams>{std::move(fboTexGenParams)} });
			auto fboVec = std::vector<FrameBufferObject>();
			fboVec.emplace_back(std::move(fbo));
			auto fboHandlr = Fbo_Handler(std::move(fboVec), "Ssao_Pass", true, 0, 0, nullptr, nullptr);
			m_FboHandlerVec.emplace_back(std::move(fboHandlr));
		}

//Ssao Blur Pass
		{
			auto fboTexParams = std::vector<FBOTexParams>();
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_MIN_FILTER,		GL_LINEAR });
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_MAG_FILTER,		GL_LINEAR });
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_WRAP_S,			GL_CLAMP_TO_EDGE });
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_WRAP_T,			GL_CLAMP_TO_EDGE });

			FBOTexGenParams fboTexGenParams { 1, GL_TEXTURE_2D, 0, GL_R32F, 0, GL_RED, GL_FLOAT, NULL, std::move(fboTexParams) };
			auto fbo = FrameBufferObject(FBOParams{ false, ScreenWidth, ScreenHeight, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, std::vector<FBOTexGenParams>{std::move(fboTexGenParams)} });
			auto fboVec = std::vector<FrameBufferObject>();
			fboVec.emplace_back(std::move(fbo));
			auto fboHandlr = Fbo_Handler(std::move(fboVec), "Ssao_Blur_Pass", true, 0, 0, nullptr, nullptr);
			m_FboHandlerVec.emplace_back(std::move(fboHandlr));
		}

//Shading Pass
		{
			auto colorFboTexParams = std::vector<FBOTexParams>();
			colorFboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_MIN_FILTER,	GL_LINEAR });
			colorFboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_MAG_FILTER,	GL_LINEAR });
			colorFboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_WRAP_S,		GL_CLAMP_TO_EDGE });
			colorFboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_WRAP_T,		GL_CLAMP_TO_EDGE });

			auto motionFboTexParams = std::vector<FBOTexParams>();
			motionFboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_MIN_FILTER,	GL_NEAREST });
			motionFboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_MAG_FILTER,	GL_NEAREST });

			FBOTexGenParams colorFboTexGenParams{ 2, GL_TEXTURE_2D, 0, GL_RGBA16F, 0, GL_RGBA, GL_FLOAT, NULL, std::move(colorFboTexParams) };
			FBOTexGenParams motionfboTexGenParams{ 1, GL_TEXTURE_2D, 0, GL_RG16F, 0, GL_RG, GL_FLOAT, NULL, std::move(motionFboTexParams) };
			FBORenderBufferParam fboRBParams { GL_DEPTH_COMPONENT , GL_DEPTH_ATTACHMENT };
			auto fbo = FrameBufferObject(FBOParams{ false, ScreenWidth, ScreenHeight, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, std::vector<FBOTexGenParams>{colorFboTexGenParams, motionfboTexGenParams }, std::vector<FBORenderBufferParam>{ std::move(fboRBParams) } });
			auto fboVec = std::vector<FrameBufferObject>();
			fboVec.emplace_back(std::move(fbo));
			auto fboHandlr = Fbo_Handler(std::move(fboVec), "Shading_Pass", true, 0, 0, nullptr, nullptr);
			m_FboHandlerVec.emplace_back(std::move(fboHandlr));
		}

//Bloom Pass
		{
			auto fboVec = std::vector<FrameBufferObject>();
			for (unsigned int i = 0; i < 2; ++i)
			{
				auto fboTexParams = std::vector<FBOTexParams>();
				fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_MIN_FILTER,	GL_LINEAR });
				fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_MAG_FILTER,	GL_LINEAR });
				fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_WRAP_S,		GL_CLAMP_TO_EDGE });
				fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_WRAP_T,		GL_CLAMP_TO_EDGE });

				FBOTexGenParams fboTexGenParams { 1, GL_TEXTURE_2D, 0, GL_RGB16F, 0, GL_RGB, GL_FLOAT, NULL, fboTexParams };
				auto fbo = FrameBufferObject(FBOParams{ false, ScreenWidth, ScreenHeight, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, std::vector<FBOTexGenParams>{std::move(fboTexGenParams)} });
				fboVec.emplace_back(std::move(fbo));
			}
			auto fboHandlr = Fbo_Handler(std::move(fboVec), "Bloom_Pass", true, 0, 0, nullptr, nullptr);
			m_FboHandlerVec.emplace_back(std::move(fboHandlr));
		}

//Motion Blur Pass
		{
			auto fboTexParams = std::vector<FBOTexParams>();
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_MIN_FILTER,		GL_LINEAR });
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_MAG_FILTER,		GL_LINEAR });
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_WRAP_S,			GL_CLAMP_TO_EDGE });
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_WRAP_T,			GL_CLAMP_TO_EDGE });

			FBOTexGenParams fboTexGenParams { 1, GL_TEXTURE_2D, 0, GL_RGBA16F, 0, GL_RGBA, GL_FLOAT, NULL, fboTexParams };
			auto fbo = FrameBufferObject(FBOParams{ false, ScreenWidth, ScreenHeight, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, std::vector<FBOTexGenParams>{std::move(fboTexGenParams)} });
			auto fboVec = std::vector<FrameBufferObject>();
			fboVec.emplace_back(std::move(fbo));
			auto fboHandlr = Fbo_Handler(std::move(fboVec), "Motion_Blur_Pass", true, 0, 0, nullptr, nullptr);
			m_FboHandlerVec.emplace_back(std::move(fboHandlr));
		}

//ToDo:
//Final Output
		{
			auto fboTexParams = std::vector<FBOTexParams>();
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_MIN_FILTER,		GL_LINEAR });
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_MAG_FILTER,		GL_LINEAR });
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_WRAP_S,			GL_CLAMP_TO_EDGE });
			fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_WRAP_T,			GL_CLAMP_TO_EDGE });

			FBOTexGenParams fboTexGenParams { 1, GL_TEXTURE_2D, 0, GL_RGBA16F, 0, GL_RGBA, GL_FLOAT, NULL, std::move(fboTexParams) };
			auto fbo = FrameBufferObject(FBOParams{ false, ScreenWidth, ScreenHeight, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, std::vector<FBOTexGenParams>{std::move(fboTexGenParams)} });
			auto fboVec = std::vector<FrameBufferObject>();
			fboVec.emplace_back(std::move(fbo));
			auto fboHandlr = Fbo_Handler(std::move(fboVec), "Final_Output_Pass", true, 0, 0, nullptr, nullptr);
			m_FboHandlerVec.emplace_back(std::move(fboHandlr));
		}
	}

	Fbo_Handler* FindFboHandler(const std::string& handlerName)
	{
		for (int i = 0; i < m_FboHandlerVec.size(); ++i)
		{
			Fbo_Handler* hndlr = &(m_FboHandlerVec[i]);
			if (hndlr->GetHandlerName() == handlerName)
			{
				return hndlr;
			}
		}
		return nullptr;
	}

	void ResizeScreenFboHandlers(const GLuint& width, const GLuint& height) 
	{
		for (auto i = 0; i < m_FboHandlerVec.size(); ++i)
		{
			if (m_FboHandlerVec[i].CanResizeWithWindow())
			{
				m_FboHandlerVec[i].ResizeFBO(width, height);
			}
		}
	}

	Fbo_Handler* AddGameCameraFboHandlers(const int& cameraId, const glm::ivec2& screenDims)
	{
		auto fboTexParams = std::vector<FBOTexParams>();
		fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_MIN_FILTER,		GL_LINEAR });
		fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_MAG_FILTER,		GL_LINEAR });
		fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_WRAP_S,			GL_CLAMP_TO_EDGE });
		fboTexParams.emplace_back(FBOTexParams{ GL_TEXTURE_WRAP_T,			GL_CLAMP_TO_EDGE });

		FBOTexGenParams fboTexGenParams{ 1, GL_TEXTURE_2D, 0, GL_RGBA16F, 0, GL_RGBA, GL_FLOAT, NULL, std::move(fboTexParams) };
		auto fbo = FrameBufferObject(FBOParams{ false, static_cast<GLuint>(screenDims.x), static_cast<GLuint>(screenDims.y), GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, std::vector<FBOTexGenParams>{std::move(fboTexGenParams)} });
		auto fboVec = std::vector<FrameBufferObject>();
		fboVec.emplace_back(std::move(fbo));	
		auto fboHandlr = Fbo_Handler(std::move(fboVec), "CameraPass" + std::to_string(cameraId), true, 0, 0, nullptr, nullptr);
		m_FboHandlerVec.emplace_back(std::move(fboHandlr));
		
		return &m_FboHandlerVec.back();
	}

	Impl(Impl&& rhs) noexcept = default;
	Impl& operator=(Impl&& rhs) noexcept = default;

	Impl(const Impl& rhs) noexcept = delete;
	Impl& operator=(const Impl& rhs) noexcept = delete;

	~Impl() = default;
};

Scene_Fbo_Handler_Manager::Scene_Fbo_Handler_Manager(const std::string& sceneName, const glm::ivec2& screenDims)
	: m_pImpl{ std::make_unique<Impl>(sceneName, screenDims) }
{
}

Fbo_Handler* Scene_Fbo_Handler_Manager::FindFboHandler(const std::string& handlerName)
{
	return Pimpl()->FindFboHandler(handlerName);
}


void Scene_Fbo_Handler_Manager::ResizeScreenFboHandlers(const GLuint& width, const GLuint& height)
{
	Pimpl()->ResizeScreenFboHandlers(width, height);
}

Fbo_Handler* Scene_Fbo_Handler_Manager::AddGameCameraFboHandlers(const int& cameraId, const glm::ivec2& screenDims)
{
	return Pimpl()->AddGameCameraFboHandlers(cameraId, screenDims);
}

Scene_Fbo_Handler_Manager::~Scene_Fbo_Handler_Manager() = default;

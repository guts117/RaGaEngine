#include "pch.h"
#include "Scene_Fbo_Handler_Manager.h"
#include "FrameBufferObject.h"
#include "Fbo_Handler.h"
#include "RenderingCommonValues.h"

using namespace NarakaKarEngine;
using namespace RenderEngine;

extern int ScreenWidth;
extern int ScreenHeight;

struct Scene_Fbo_Handler_Manager::Impl
{
	std::unique_ptr<std::vector<std::shared_ptr<Fbo_Handler>>> m_FboHandlerVec;
	std::string m_SceneName;

	Impl() = delete;

	Impl(const std::string& sceneName)
		: m_FboHandlerVec {std::make_unique<std::vector<std::shared_ptr<Fbo_Handler>>>()}
		, m_SceneName { sceneName }
	{
		//ToDo: Read from json and load all of them 
		//ToDo: read buffer width and height from the json file.

//Brdf Pass
		auto fboTexParams = std::vector<FBOTexParams>();
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MIN_FILTER,	GL_LINEAR });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MAG_FILTER,	GL_LINEAR });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_S,		GL_CLAMP_TO_EDGE });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_T,		GL_CLAMP_TO_EDGE });

		FBOTexGenParams fboTexGenParams{ 1, GL_TEXTURE_2D, 0, GL_RG16F, 0, GL_RG, GL_FLOAT, NULL, fboTexParams };
		auto fboParams = std::make_shared<FBOParams>(FBOParams{ false, static_cast<GLuint>(ScreenWidth), static_cast<GLuint>(ScreenWidth), GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, std::vector<FBOTexGenParams>{fboTexGenParams} });
		auto fbo = std::make_shared<FrameBufferObject>(fboParams);
		auto fboVec = std::make_unique<std::vector<std::shared_ptr<FrameBufferObject>>>();
		fboVec->push_back(fbo);
		auto fboHandlr = std::make_shared<Fbo_Handler>(std::move(fboVec), "Brdf_Pass", false, 0, 0, nullptr, nullptr);
		m_FboHandlerVec->push_back(fboHandlr);

//Environment Map Pass
		fboTexParams.clear();
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MIN_FILTER,		GL_LINEAR_MIPMAP_LINEAR });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MAG_FILTER,		GL_LINEAR });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_S,			GL_CLAMP_TO_EDGE });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_T,			GL_CLAMP_TO_EDGE });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_R,			GL_CLAMP_TO_EDGE });

		fboTexGenParams = FBOTexGenParams{ 1, GL_TEXTURE_CUBE_MAP, 0, GL_RGB16F, 0, GL_RGB, GL_FLOAT, NULL, fboTexParams };
		FBORenderBufferParam fboRBParams{ GL_DEPTH_COMPONENT24 , GL_DEPTH_ATTACHMENT };
		fboParams = std::make_shared<FBOParams>(FBOParams{ true, static_cast<GLuint>(ScreenWidth), static_cast<GLuint>(ScreenWidth), GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, std::vector<FBOTexGenParams>{fboTexGenParams}, std::vector<FBORenderBufferParam>{ fboRBParams } });
		fbo = std::make_unique<FrameBufferObject>(fboParams);
		fboVec = std::make_unique<std::vector<std::shared_ptr<FrameBufferObject>>>();
		fboVec->push_back(fbo);
		fboHandlr = std::make_shared<Fbo_Handler>(std::move(fboVec), "Environment_Map_Pass", false, 0, 0, nullptr, nullptr);
		m_FboHandlerVec->push_back(fboHandlr);

//Irradiance Map Pass
		fboTexParams.clear();
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MIN_FILTER,		GL_LINEAR });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MAG_FILTER,		GL_LINEAR });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_S,			GL_CLAMP_TO_EDGE });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_T,			GL_CLAMP_TO_EDGE });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_R,			GL_CLAMP_TO_EDGE });

		fboTexGenParams = FBOTexGenParams{ 1, GL_TEXTURE_CUBE_MAP, 0, GL_RGB16F, 0, GL_RGB, GL_FLOAT, NULL, fboTexParams };
		fboRBParams = FBORenderBufferParam{ GL_DEPTH_COMPONENT24 , GL_DEPTH_ATTACHMENT };
		fboParams = std::make_shared<FBOParams>(FBOParams{ true, 32, 32, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, std::vector<FBOTexGenParams>{fboTexGenParams}, std::vector<FBORenderBufferParam>{ fboRBParams } });
		fbo = std::make_unique<FrameBufferObject>(fboParams);
		fboVec = std::make_unique<std::vector<std::shared_ptr<FrameBufferObject>>>();
		fboVec->push_back(fbo);
		fboHandlr = std::make_shared<Fbo_Handler>(std::move(fboVec), "Irradiance_Map_Pass", false, 0, 0, nullptr, nullptr);
		m_FboHandlerVec->push_back(fboHandlr);

//Pre Filter Pass
		fboTexParams.clear();
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MIN_FILTER,		GL_LINEAR_MIPMAP_LINEAR });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MAG_FILTER,		GL_NEAREST });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_S,			GL_CLAMP_TO_EDGE });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_T,			GL_CLAMP_TO_EDGE });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_R,			GL_CLAMP_TO_EDGE });

		fboTexGenParams = FBOTexGenParams{ 1, GL_TEXTURE_CUBE_MAP, 0, GL_RGB16F, 0, GL_RGB, GL_FLOAT, NULL, fboTexParams };
		fboRBParams = FBORenderBufferParam{ GL_DEPTH_COMPONENT24 , GL_DEPTH_ATTACHMENT };
		fboParams = std::make_shared<FBOParams>(FBOParams{ true, 128, 128, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, std::vector<FBOTexGenParams>{fboTexGenParams}, std::vector<FBORenderBufferParam>{fboRBParams} });
		fbo = std::make_unique<FrameBufferObject>(fboParams);
		fbo->CreateMipMap(0, 0);
		fboVec = std::make_unique<std::vector<std::shared_ptr<FrameBufferObject>>>();
		fboVec->push_back(fbo);
		fboHandlr = std::make_shared<Fbo_Handler>(std::move(fboVec), "Pre_Filter_Pass", false, 0, 0, nullptr, nullptr);
		m_FboHandlerVec->push_back(fboHandlr);

//Depth Pass
		fboTexParams.clear();
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MIN_FILTER,		GL_LINEAR });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MAG_FILTER,		GL_LINEAR });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_S,			GL_CLAMP_TO_EDGE });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_T,			GL_CLAMP_TO_EDGE });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_COMPARE_MODE,	GL_NONE });

		fboTexGenParams = FBOTexGenParams{ 1, GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL, fboTexParams };
		fboParams = std::make_shared<FBOParams>(FBOParams{ false, static_cast<GLuint>(ScreenWidth), static_cast<GLuint>(ScreenHeight), GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, std::vector<FBOTexGenParams>{fboTexGenParams} });
		fbo = std::make_unique<FrameBufferObject>(fboParams);
		fboVec = std::make_unique<std::vector<std::shared_ptr<FrameBufferObject>>>();
		fboVec->push_back(fbo);
		fboHandlr = std::make_shared<Fbo_Handler>(std::move(fboVec), "Depth_Pass", true, 0, 0, nullptr, nullptr);
		m_FboHandlerVec->push_back(fboHandlr);

//Shadow Map Pass
		fboTexParams.clear();
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MIN_FILTER,		GL_NEAREST });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MAG_FILTER,		GL_NEAREST });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_COMPARE_MODE,	GL_NONE });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_S,			GL_CLAMP_TO_BORDER });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_T,			GL_CLAMP_TO_BORDER });
		float bColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_BORDER_COLOR,	bColor });

		fboTexGenParams = FBOTexGenParams{ NUM_CASCADES, GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL, fboTexParams };
		fboParams = std::make_shared<FBOParams>(FBOParams{ false, 1024, 1024, GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, std::vector<FBOTexGenParams>{fboTexGenParams} });
		fbo = std::make_unique<FrameBufferObject>(fboParams);
		fboVec = std::make_unique<std::vector<std::shared_ptr<FrameBufferObject>>>();
		fboVec->push_back(fbo);
		fboHandlr = std::make_shared<Fbo_Handler>(std::move(fboVec), "Shadow_Map_Pass", false, 0, 0, nullptr, nullptr);
		m_FboHandlerVec->push_back(fboHandlr);

//Omni Shadow Map Pass
		fboTexParams.clear();
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MIN_FILTER,		GL_NEAREST });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MAG_FILTER,		GL_NEAREST });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_S,			GL_CLAMP_TO_EDGE });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_T,			GL_CLAMP_TO_EDGE });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_R,			GL_CLAMP_TO_EDGE });

		fboTexGenParams = FBOTexGenParams{ 1, GL_TEXTURE_CUBE_MAP, 0, GL_DEPTH_COMPONENT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL, fboTexParams };
		fboParams = std::make_shared<FBOParams>(FBOParams{ false, 512, 512, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, std::vector<FBOTexGenParams>{fboTexGenParams} });
		fbo = std::make_unique<FrameBufferObject>(fboParams);
		fboVec = std::make_unique<std::vector<std::shared_ptr<FrameBufferObject>>>();
		fboVec->push_back(fbo);
		fboHandlr = std::make_shared<Fbo_Handler>(std::move(fboVec), "Omni_Shadow_Map_Pass", false, 0, 0, nullptr, nullptr);
		m_FboHandlerVec->push_back(fboHandlr);

//Ssao Pass
		fboTexParams.clear();
		fboTexParams = std::vector<FBOTexParams>();
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MIN_FILTER,		GL_LINEAR });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MAG_FILTER,		GL_LINEAR });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_S,			GL_CLAMP_TO_EDGE });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_T,			GL_CLAMP_TO_EDGE });

		fboTexGenParams = FBOTexGenParams{ 1, GL_TEXTURE_2D, 0, GL_R32F, 0, GL_RED, GL_FLOAT, NULL, fboTexParams };
		fboParams = std::make_shared<FBOParams>(FBOParams{ false, static_cast<GLuint>(ScreenWidth), static_cast<GLuint>(ScreenHeight), GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, std::vector<FBOTexGenParams>{fboTexGenParams} });
		fbo = std::make_unique<FrameBufferObject>(fboParams);
		fboVec = std::make_unique<std::vector<std::shared_ptr<FrameBufferObject>>>();
		fboVec->push_back(fbo);
		fboHandlr = std::make_shared<Fbo_Handler>(std::move(fboVec), "Ssao_Pass", true, 0, 0, nullptr, nullptr);
		m_FboHandlerVec->push_back(fboHandlr);

//Ssao Blur Pass
		fboTexParams.clear();
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MIN_FILTER,		GL_LINEAR });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MAG_FILTER,		GL_LINEAR });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_S,			GL_CLAMP_TO_EDGE });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_T,			GL_CLAMP_TO_EDGE });

		fboTexGenParams = FBOTexGenParams{ 1, GL_TEXTURE_2D, 0, GL_R32F, 0, GL_RED, GL_FLOAT, NULL, fboTexParams };
		fboParams = std::make_shared<FBOParams>(FBOParams{ false, static_cast<GLuint>(ScreenWidth), static_cast<GLuint>(ScreenHeight), GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, std::vector<FBOTexGenParams>{fboTexGenParams} });
		fbo = std::make_unique<FrameBufferObject>(fboParams);
		fboVec = std::make_unique<std::vector<std::shared_ptr<FrameBufferObject>>>();
		fboVec->push_back(fbo);
		fboHandlr = std::make_shared<Fbo_Handler>(std::move(fboVec), "Ssao_Blur_Pass", true, 0, 0, nullptr, nullptr);
		m_FboHandlerVec->push_back(fboHandlr);

//Shading Pass
		auto colorFboTexParams = std::vector<FBOTexParams>();
		colorFboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MIN_FILTER,	GL_LINEAR });
		colorFboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MAG_FILTER,	GL_LINEAR });
		colorFboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_S,		GL_CLAMP_TO_EDGE });
		colorFboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_T,		GL_CLAMP_TO_EDGE });

		auto motionFboTexParams = std::vector<FBOTexParams>();
		motionFboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MIN_FILTER,	GL_NEAREST });
		motionFboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MAG_FILTER,	GL_NEAREST });

		FBOTexGenParams colorFboTexGenParams{ 2, GL_TEXTURE_2D, 0, GL_RGBA16F, 0, GL_RGBA, GL_FLOAT, NULL, colorFboTexParams };
		FBOTexGenParams motionfboTexGenParams{ 1, GL_TEXTURE_2D, 0, GL_RG16F, 0, GL_RG, GL_FLOAT, NULL, motionFboTexParams };
		fboRBParams = FBORenderBufferParam{ GL_DEPTH_COMPONENT , GL_DEPTH_ATTACHMENT };
		fboParams = std::make_shared<FBOParams>(FBOParams{ false, static_cast<GLuint>(ScreenWidth), static_cast<GLuint>(ScreenHeight), GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, std::vector<FBOTexGenParams>{colorFboTexGenParams, motionfboTexGenParams }, std::vector<FBORenderBufferParam>{ fboRBParams } });
		fbo = std::make_unique<FrameBufferObject>(fboParams);
		fboVec = std::make_unique<std::vector<std::shared_ptr<FrameBufferObject>>>();
		fboVec->push_back(fbo);
		fboHandlr = std::make_shared<Fbo_Handler>(std::move(fboVec), "Shading_Pass", true, 0, 0, nullptr, nullptr);
		m_FboHandlerVec->push_back(fboHandlr);

//Bloom Pass
		fboVec = std::make_unique<std::vector<std::shared_ptr<FrameBufferObject>>>();
		for (unsigned int i = 0; i < 2; ++i)
		{
			fboTexParams.clear();
			fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MIN_FILTER,	GL_LINEAR });
			fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MAG_FILTER,	GL_LINEAR });
			fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_S,		GL_CLAMP_TO_EDGE });
			fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_T,		GL_CLAMP_TO_EDGE });

			FBOTexGenParams fboTexGenParams{ 1, GL_TEXTURE_2D, 0, GL_RGB16F, 0, GL_RGB, GL_FLOAT, NULL, fboTexParams };
			fboParams = std::make_shared<FBOParams>(FBOParams{ false, static_cast<GLuint>(ScreenWidth),static_cast<GLuint>(ScreenHeight), GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, std::vector<FBOTexGenParams>{fboTexGenParams} });
			fboVec->push_back(std::make_shared<FrameBufferObject>(fboParams));
		}
		fboHandlr = std::make_shared<Fbo_Handler>(std::move(fboVec), "Bloom_Pass", true, 0, 0, nullptr, nullptr);
		m_FboHandlerVec->push_back(fboHandlr);

//Motion Blur Pass
		fboTexParams.clear();
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MIN_FILTER,		GL_LINEAR });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MAG_FILTER,		GL_LINEAR });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_S,			GL_CLAMP_TO_EDGE });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_T,			GL_CLAMP_TO_EDGE });

		fboTexGenParams = FBOTexGenParams{ 1, GL_TEXTURE_2D, 0, GL_RGBA16F, 0, GL_RGBA, GL_FLOAT, NULL, fboTexParams };
		fboParams = std::make_shared<FBOParams>(FBOParams{ false, static_cast<GLuint>(ScreenWidth), static_cast<GLuint>(ScreenHeight), GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, std::vector<FBOTexGenParams>{fboTexGenParams} });
		fbo = std::make_unique<FrameBufferObject>(fboParams);
		fboVec = std::make_unique<std::vector<std::shared_ptr<FrameBufferObject>>>();
		fboVec->push_back(fbo);
		fboHandlr = std::make_shared<Fbo_Handler>(std::move(fboVec), "Motion_Blur_Pass", true, 0, 0, nullptr, nullptr);
		m_FboHandlerVec->push_back(fboHandlr);

//ToDo:
//Final Output
		fboTexParams.clear();
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MIN_FILTER,		GL_LINEAR });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_MAG_FILTER,		GL_LINEAR });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_S,			GL_CLAMP_TO_EDGE });
		fboTexParams.push_back(FBOTexParams{ GL_TEXTURE_WRAP_T,			GL_CLAMP_TO_EDGE });

		fboTexGenParams = FBOTexGenParams{ 1, GL_TEXTURE_2D, 0, GL_RGBA16F, 0, GL_RGBA, GL_FLOAT, NULL, fboTexParams };
		fboParams = std::make_shared<FBOParams>(FBOParams{ false, static_cast<GLuint>(ScreenWidth), static_cast<GLuint>(ScreenHeight), GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, std::vector<FBOTexGenParams>{fboTexGenParams} });
		fbo = std::make_unique<FrameBufferObject>(fboParams);
		fboVec = std::make_unique<std::vector<std::shared_ptr<FrameBufferObject>>>();
		fboVec->push_back(fbo);
		fboHandlr = std::make_shared<Fbo_Handler>(std::move(fboVec), "Final_Output_Pass", true, 0, 0, nullptr, nullptr);
		m_FboHandlerVec->push_back(fboHandlr);

	}

	Impl(Impl&& rhs) noexcept = default;
	Impl& operator=(Impl&& rhs) noexcept = default;

	Impl(const Impl& rhs) noexcept = delete;
	Impl& operator=(const Impl& rhs) noexcept = delete;

	~Impl() = default;
};

Scene_Fbo_Handler_Manager::Scene_Fbo_Handler_Manager(const std::string& sceneName)
	: m_pImpl{ new Impl(sceneName) }
{
}

std::shared_ptr<Fbo_Handler> Scene_Fbo_Handler_Manager::FindFboHandler(const std::string& handlerName) const
{
	auto it = std::find_if(Pimpl()->m_FboHandlerVec->begin(), Pimpl()->m_FboHandlerVec->end(), [&](std::shared_ptr<Fbo_Handler> hndlr) {return hndlr->GetHandlerName() == handlerName; });
	return  it != Pimpl()->m_FboHandlerVec->end() ? *it : nullptr;
}


void Scene_Fbo_Handler_Manager::ResizeScreenFboHandlers(const GLuint& width, const GLuint& height)
{
	for(auto i = 0; i< Pimpl()->m_FboHandlerVec->size(); ++i)
	{
		if (Pimpl()->m_FboHandlerVec->at(i)->CanResizeWithWindow()) 
		{
			Pimpl()->m_FboHandlerVec->at(i)->ResizeFBO(width, height);
		}
	}
}

Scene_Fbo_Handler_Manager::~Scene_Fbo_Handler_Manager() = default;

#ifndef IRRADIANCE_CONVOLUTION_RENDER_PASS_HANDLER
#define IRRADIANCE_CONVOLUTION_RENDER_PASS_HANDLER

#include "Scene_Render_Pass_Handler.h"

namespace NarakaKarEngine
{
    namespace RenderEngine
    {
        class Irradiance_Convolution_Render_Pass_Handler :
            public Scene_Render_Pass_Handler
        {
        public:
            explicit Irradiance_Convolution_Render_Pass_Handler() = delete;
            explicit Irradiance_Convolution_Render_Pass_Handler(std::shared_ptr<Fbo_Handler> fboHandlr
                , const std::vector<std::shared_ptr<Shader_Object>>& shaderVec
                , std::shared_ptr<std::vector<std::shared_ptr<std::any>>> inputs = nullptr);

            virtual void Update(std::shared_ptr<std::vector<std::shared_ptr<Render_Object>>> renderObj, const CamParam& camParam) override;

            Irradiance_Convolution_Render_Pass_Handler(Irradiance_Convolution_Render_Pass_Handler&& rhs) noexcept = default;
            Irradiance_Convolution_Render_Pass_Handler& operator=(Irradiance_Convolution_Render_Pass_Handler&& rhs) noexcept = default;

            Irradiance_Convolution_Render_Pass_Handler(const Irradiance_Convolution_Render_Pass_Handler& rhs) noexcept = delete;
            Irradiance_Convolution_Render_Pass_Handler& operator=(const Irradiance_Convolution_Render_Pass_Handler& rhs) noexcept = delete;

            ~Irradiance_Convolution_Render_Pass_Handler();
        };
    }
}

#endif
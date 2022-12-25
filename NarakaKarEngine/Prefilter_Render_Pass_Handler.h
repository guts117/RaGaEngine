#ifndef PREFILTER_RENDER_PASS_HANDLER
#define PREFILTER_RENDER_PASS_HANDLER

#include "Scene_Render_Pass_Handler.h"

namespace NarakaKarEngine
{
    namespace RenderEngine
    {
        class Render_Object;
        class Shader_Object;
        class Fbo_Handler;

        class Prefilter_Render_Pass_Handler :
            public Scene_Render_Pass_Handler
        {
        public:
            explicit Prefilter_Render_Pass_Handler() = delete;
            explicit Prefilter_Render_Pass_Handler(std::shared_ptr<Fbo_Handler> fboHandlr
                , const std::vector<std::shared_ptr<Shader_Object>>& shaderVec
                , std::shared_ptr<std::vector<std::shared_ptr<std::any>>> inputs = nullptr);

            virtual void Update(const std::vector<std::vector<std::shared_ptr<Render_Object>>>& renderObj, const CamParam* camParam = nullptr, const LightParam* lightParam = nullptr) override;

            Prefilter_Render_Pass_Handler(Prefilter_Render_Pass_Handler&& rhs) noexcept = default;
            Prefilter_Render_Pass_Handler& operator=(Prefilter_Render_Pass_Handler&& rhs) noexcept = default;

            Prefilter_Render_Pass_Handler(const Prefilter_Render_Pass_Handler& rhs) noexcept = delete;
            Prefilter_Render_Pass_Handler& operator=(const Prefilter_Render_Pass_Handler& rhs) noexcept = delete;

            ~Prefilter_Render_Pass_Handler();
        };
    }
}

#endif
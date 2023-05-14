#ifndef PREFILTER_RENDER_PASS_HANDLER
#define PREFILTER_RENDER_PASS_HANDLER

#include "Render_Pass_Handler.h"

namespace NarakaRenderEngine
{
    namespace RenderEngine
    {
        class Render_Object;
        class Shader_Object;
        class Fbo_Handler;

        class Prefilter_Render_Pass_Handler final :
            public Render_Pass_Handler
        {
        public:
            explicit Prefilter_Render_Pass_Handler() = delete;
            explicit Prefilter_Render_Pass_Handler(Fbo_Handler* fboHandlr
                , std::vector<rw_clustering_ptr<Shader_Object>>&& shaderVec
                , std::shared_ptr<std::vector<std::shared_ptr<std::any>>> inputs = nullptr);

            virtual void Update(std::vector<std::vector<Render_Object>>& renderObj, const CamParam* camParam = nullptr, const LightParam* lightParam = nullptr) override;

            Prefilter_Render_Pass_Handler(Prefilter_Render_Pass_Handler&& rhs) noexcept = default;
            Prefilter_Render_Pass_Handler& operator=(Prefilter_Render_Pass_Handler&& rhs) noexcept = default;

            Prefilter_Render_Pass_Handler(const Prefilter_Render_Pass_Handler& rhs) noexcept = delete;
            Prefilter_Render_Pass_Handler& operator=(const Prefilter_Render_Pass_Handler& rhs) noexcept = delete;

            ~Prefilter_Render_Pass_Handler();
        };
    }
}

#endif
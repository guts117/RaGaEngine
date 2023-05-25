#ifndef BILLBOARD_RENDER_PASS_HANDLER
#define BILLBOARD_RENDER_PASS_HANDLER

#include "Render_Pass_Handler.h"

namespace NarakaRenderEngine
{
    namespace RenderEngine
    {
        class Billboard_Render_Pass_Handler final :
            public Render_Pass_Handler
        {
        public:
            explicit Billboard_Render_Pass_Handler() = delete;
            explicit Billboard_Render_Pass_Handler(Fbo_Handler* fboHandlr
                , std::vector<rw_clustering_ptr<Shader_Object>>&& shaderVec
                , std::shared_ptr<std::vector<std::shared_ptr<std::any>>> inputs = nullptr);

            virtual void Update(std::vector<std::vector<Render_Object>>& renderObj, const CamParam* camParam = nullptr, const LightParam* lightParam = nullptr) override;

            Billboard_Render_Pass_Handler(Billboard_Render_Pass_Handler&& rhs) noexcept = default;
            Billboard_Render_Pass_Handler& operator=(Billboard_Render_Pass_Handler&& rhs) noexcept = default;

            Billboard_Render_Pass_Handler(const Billboard_Render_Pass_Handler& rhs) noexcept = delete;
            Billboard_Render_Pass_Handler& operator=(const Billboard_Render_Pass_Handler& rhs) noexcept = delete;

            ~Billboard_Render_Pass_Handler();
        };
    }
}

#endif

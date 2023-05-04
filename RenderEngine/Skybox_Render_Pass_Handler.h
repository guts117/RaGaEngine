#ifndef SKYBOX_RENDER_PASS_HANDLER
#define SKYBOX_RENDER_PASS_HANDLER

#include "Render_Pass_Handler.h"

namespace NarakaRenderEngine
{
    namespace RenderEngine
    {
        class Skybox_Render_Pass_Handler final :
            public Render_Pass_Handler
        {
        public:
            explicit Skybox_Render_Pass_Handler() = delete;
            explicit Skybox_Render_Pass_Handler(Fbo_Handler* fboHandlr
                , const std::vector<std::shared_ptr<Shader_Object>>& shaderVec
                , std::shared_ptr<std::vector<std::shared_ptr<std::any>>> inputs = nullptr);

            virtual void Update(const std::vector<std::vector<std::shared_ptr<Render_Object>>>& renderObj, const CamParam* camParam = nullptr, const LightParam* lightParam = nullptr) override;

            Skybox_Render_Pass_Handler(Skybox_Render_Pass_Handler&& rhs) noexcept = default;
            Skybox_Render_Pass_Handler& operator=(Skybox_Render_Pass_Handler&& rhs) noexcept = default;

            Skybox_Render_Pass_Handler(const Skybox_Render_Pass_Handler& rhs) noexcept = delete;
            Skybox_Render_Pass_Handler& operator=(const Skybox_Render_Pass_Handler& rhs) noexcept = delete;

            ~Skybox_Render_Pass_Handler();
        };
    }
}

#endif

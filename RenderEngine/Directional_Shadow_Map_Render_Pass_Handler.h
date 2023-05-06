#ifndef DIRECTIONAL_SHADOW_MAP_RENDER_PASS_HANDLER
#define DIRECTIONAL_SHADOW_MAP_RENDER_PASS_HANDLER

#include "Render_Pass_Handler.h"

namespace NarakaRenderEngine
{
    namespace RenderEngine
    {
        class Directional_Shadow_Map_Render_Pass_Handler final :
            public Render_Pass_Handler
        {
        public:
            explicit Directional_Shadow_Map_Render_Pass_Handler() = delete;
            explicit Directional_Shadow_Map_Render_Pass_Handler(Fbo_Handler* fboHandlr
                , std::vector<Shader_Object*>&& shaderVec
                , std::shared_ptr<std::vector<std::shared_ptr<std::any>>> inputs = nullptr);

            virtual void Update(const std::vector<std::vector<Render_Object>>& renderObj, const CamParam* camParam = nullptr, const LightParam* lightParam = nullptr) override;

            Directional_Shadow_Map_Render_Pass_Handler(Directional_Shadow_Map_Render_Pass_Handler&& rhs) noexcept = default;
            Directional_Shadow_Map_Render_Pass_Handler& operator=(Directional_Shadow_Map_Render_Pass_Handler&& rhs) noexcept = default;

            Directional_Shadow_Map_Render_Pass_Handler(const Directional_Shadow_Map_Render_Pass_Handler& rhs) noexcept = delete;
            Directional_Shadow_Map_Render_Pass_Handler& operator=(const Directional_Shadow_Map_Render_Pass_Handler& rhs) noexcept = delete;

            ~Directional_Shadow_Map_Render_Pass_Handler();
        };
    }
}

#endif

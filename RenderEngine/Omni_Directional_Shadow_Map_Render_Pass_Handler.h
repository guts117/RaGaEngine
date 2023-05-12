#ifndef OMNI_DIRECTIONAL_SHADOW_MAP_RENDER_PASS_HANDLER
#define OMNI_DIRECTIONAL_SHADOW_MAP_RENDER_PASS_HANDLER

#include "Render_Pass_Handler.h"

namespace NarakaRenderEngine
{
    namespace RenderEngine
    {
        class Omni_Directional_Shadow_Map_Render_Pass_Handler final :
            public Render_Pass_Handler
        {
        public:
            explicit Omni_Directional_Shadow_Map_Render_Pass_Handler() = delete;
            explicit Omni_Directional_Shadow_Map_Render_Pass_Handler(Fbo_Handler* fboHandlr
                , std::vector<clustering_ptr<Shader_Object>>&& shaderVec
                , std::shared_ptr<std::vector<std::shared_ptr<std::any>>> inputs = nullptr);

            virtual void Update(std::vector<std::vector<Render_Object>>& renderObj, const CamParam* camParam = nullptr, const LightParam* lightParam = nullptr) override;

            Omni_Directional_Shadow_Map_Render_Pass_Handler(Omni_Directional_Shadow_Map_Render_Pass_Handler&& rhs) noexcept = default;
            Omni_Directional_Shadow_Map_Render_Pass_Handler& operator=(Omni_Directional_Shadow_Map_Render_Pass_Handler&& rhs) noexcept = default;

            Omni_Directional_Shadow_Map_Render_Pass_Handler(const Omni_Directional_Shadow_Map_Render_Pass_Handler& rhs) noexcept = delete;
            Omni_Directional_Shadow_Map_Render_Pass_Handler& operator=(const Omni_Directional_Shadow_Map_Render_Pass_Handler& rhs) noexcept = delete;

            ~Omni_Directional_Shadow_Map_Render_Pass_Handler();
        };
    }
}

#endif
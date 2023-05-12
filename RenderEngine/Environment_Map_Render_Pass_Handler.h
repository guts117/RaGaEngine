#ifndef ENVIRONMENT_MAP_RENDER_PASS_HANDLER
#define ENVIRONMENT_MAP_RENDER_PASS_HANDLER

#include "Render_Pass_Handler.h"

namespace NarakaRenderEngine
{
    namespace RenderEngine
    {
        class Environment_Map_Render_Pass_Handler final :
            public Render_Pass_Handler
        {
        public:
            explicit Environment_Map_Render_Pass_Handler() = delete;
            explicit Environment_Map_Render_Pass_Handler(Fbo_Handler* fboHandlr
                , std::vector<clustering_ptr<Shader_Object>>&& shaderVec
                , std::shared_ptr<std::vector<std::shared_ptr<std::any>>> inputs = nullptr);

            virtual void Update(std::vector<std::vector<Render_Object>>& renderObj, const CamParam* camParam = nullptr, const LightParam* lightParam = nullptr) override;

            Environment_Map_Render_Pass_Handler(Environment_Map_Render_Pass_Handler&& rhs) noexcept = default;
            Environment_Map_Render_Pass_Handler& operator=(Environment_Map_Render_Pass_Handler&& rhs) noexcept = default;

            Environment_Map_Render_Pass_Handler(const Environment_Map_Render_Pass_Handler& rhs) noexcept = delete;
            Environment_Map_Render_Pass_Handler& operator=(const Environment_Map_Render_Pass_Handler& rhs) noexcept = delete;

            ~Environment_Map_Render_Pass_Handler();
        };
    }
}

#endif
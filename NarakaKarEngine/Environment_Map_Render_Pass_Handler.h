#ifndef ENVIRONMENT_MAP_RENDER_PASS_HANDLER
#define ENVIRONMENT_MAP_RENDER_PASS_HANDLER

#include "Scene_Render_Pass_Handler.h"

namespace NarakaKarEngine
{
    namespace RenderEngine
    {
        class Environment_Map_Render_Pass_Handler :
            public Scene_Render_Pass_Handler
        {
        public:
            explicit Environment_Map_Render_Pass_Handler() = delete;
            explicit Environment_Map_Render_Pass_Handler(std::shared_ptr<Fbo_Handler> fboHandlr
                , const std::vector<std::shared_ptr<Shader_Object>>& shaderVec
                , std::shared_ptr<std::vector<std::shared_ptr<std::any>>> inputs = nullptr);

            virtual void Update(std::shared_ptr<std::vector<std::shared_ptr<Render_Object>>> renderObj, const CamParam& camParam) override;

            Environment_Map_Render_Pass_Handler(Environment_Map_Render_Pass_Handler&& rhs) noexcept = default;
            Environment_Map_Render_Pass_Handler& operator=(Environment_Map_Render_Pass_Handler&& rhs) noexcept = default;

            Environment_Map_Render_Pass_Handler(const Environment_Map_Render_Pass_Handler& rhs) noexcept = delete;
            Environment_Map_Render_Pass_Handler& operator=(const Environment_Map_Render_Pass_Handler& rhs) noexcept = delete;

            ~Environment_Map_Render_Pass_Handler();
        };
    }
}

#endif
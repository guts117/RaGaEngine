cmake_minimum_required(VERSION 3.10)

# set the project name
project(RenderEngine VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add the executable
add_executable(RenderEngine main.cpp Animated_Mesh.cpp Animated_Model.cpp Billboard_Mesh.cpp Billboard_Shader.cpp Blur_PingPong_FrameBuffer.cpp Blur_Shader.cpp BRDF_Framebuffer.cpp BRDF_Shader.cpp Camera.cpp Compute_Shader.cpp Debug.cpp Depth_Framebuffer.cpp DirectionalLight.cpp EngineGUI.cpp EngineInputManager.cpp EngineUIMain.cpp Equirectangular_to_CubeMap_Framebuffer.cpp Equirectangular_to_CubeMap_Shader.cpp Framebuffer.cpp HDR_Framebuffer.cpp HDR_Shader.cpp Irradiance_Convolution_Shader.cpp Light.cpp Material.cpp Mesh.cpp Model.cpp Model_Shader.cpp MotionBlur_FrameBuffer.cpp MotionBlur_Shader.cpp OmniShadowMap_FrameBuffer.cpp Particle_Shader.cpp Particle.cpp ParticleSystem.cpp pch.cpp PointLight.cpp PreFilter_Framebuffer.cpp PreFilter_Shader.cpp PreZPass_Shader.cpp RenderEngineMain.cpp Shader.cpp ShadowMap_FrameBuffer.cpp Skybox.cpp Source.cpp SpotLight.cpp SSAO_Framebuffer.cpp SSAO_Shader.cpp SSAOBlur_Framebuffer.cpp SSAOBlur_Shader.cpp Static_Mesh.cpp Static_Model.cpp Static_Object.cpp Terrain_PreZPass_Shader.cpp Terrain_Shader.cpp Texture.cpp VertexBoneData.cpp Window.cpp ../deps/Common/include/imgui/imgui_demo.cpp ../deps/Common/include/imgui/imgui_draw.cpp ../deps/Common/include/imgui/imgui_impl_glfw.cpp ../deps/Common/include/imgui/imgui_impl_opengl3.cpp ../deps/Common/include/imgui/imgui_tables.cpp ../deps/Common/include/imgui/imgui_widgets.cpp ../deps/Common/include/imgui/imgui.cpp )

target_compile_options(RenderEngine PRIVATE -Wall -Wextra -Wpedantic)
target_compile_options(RenderEngine PRIVATE -O2)

target_include_directories(RenderEngine PUBLIC
                           "${PROJECT_SOURCE_DIR}/../deps/Common/include/imgui"
                           "${PROJECT_SOURCE_DIR}/../deps/linux/include"
                           "${PROJECT_SOURCE_DIR}/../PhysicsEngine"
                           )

target_link_directories(RenderEngine PUBLIC
                        "${PROJECT_SOURCE_DIR}/../deps/linux/lib"
                        "${PROJECT_BUILD_DIR}/PhysicsEngine")

target_link_libraries(RenderEngine PUBLIC GL GLU glfw GLEW assimp dl PhysicsEngine)
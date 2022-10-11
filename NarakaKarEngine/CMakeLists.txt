cmake_minimum_required(VERSION 3.10)

# set the project name
project(RenderEngine VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add the executable
add_executable(RenderEngine main.cpp Animated_Mesh.cpp Animated_Model.cpp Billboard_Mesh.cpp Billboard_Shader.cpp Bloom_Pass_Fbo_Handler.cpp Blur_Shader.cpp Brdf_Pass_Fbo_Handler.cpp BRDF_Shader.cpp Camera.cpp Compute_Shader.cpp Debug.cpp Depth_Pass_Fbo_Handler.cpp DirectionalLight.cpp EngineGUI.cpp EngineInputManager.cpp EngineUIMain.cpp Equirect_To_Cubemap_Pass_Fbo_Handler.cpp Equirectangular_to_CubeMap_Shader.cpp FrameBufferObject.cpp Shading_Pass_Fbo_Handler.cpp HDR_Shader.cpp Irradiance_Convolution_Shader.cpp Material.cpp Mesh.cpp Model.cpp Model_Shader.cpp Motion_Blur_Pass_Fbo_Handler.cpp MotionBlur_Shader.cpp Omni_Shadow_Pass_Fbo_Handler.cpp Particle_Shader.cpp Particle.cpp ParticleSystem.cpp pch.cpp PointLight.cpp Pre_Filter_Pass_Fbo_Handler.cpp PreFilter_Shader.cpp PreZPass_Shader.cpp RenderEngineMain.cpp Shader.cpp SceneViewer.cpp Shadow_Map_Pass_Fbo_Handler.cpp Skybox.cpp Source.cpp SpotLight.cpp Ssao_Pass_Fbo_Handler.cpp SSAO_Shader.cpp Ssao_Blur_Pass_Fbo_Handler.cpp SSAOBlur_Shader.cpp Static_Mesh.cpp Static_Model.cpp Static_Object.cpp Terrain_PreZPass_Shader.cpp Terrain_Shader.cpp Texture.cpp VertexBoneData.cpp Window.cpp ../deps/Common/include/imgui/imgui_demo.cpp ../deps/Common/include/imgui/imgui_draw.cpp ../deps/Common/include/imgui/imgui_impl_glfw.cpp ../deps/Common/include/imgui/imgui_impl_opengl3.cpp ../deps/Common/include/imgui/imgui_tables.cpp ../deps/Common/include/imgui/imgui_widgets.cpp ../deps/Common/include/imgui/imgui.cpp )

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
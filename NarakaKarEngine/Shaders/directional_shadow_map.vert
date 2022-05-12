#version 460

layout(location = 0) in vec3 pos;

uniform mat4 Model;
uniform mat4 DirectionalLightTransform;

void main()
{
	gl_Position = DirectionalLightTransform * Model* vec4(pos, 1.0);
}
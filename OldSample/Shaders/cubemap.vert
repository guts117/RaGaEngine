#version 460

layout (location = 0) in vec3 pos;

out vec3 TexCoord;			//WorldPos

uniform mat4 Projection;
uniform mat4 View;

void main()
{
	TexCoord = pos;
	gl_Position = Projection * View * vec4(pos, 1.0);	
}
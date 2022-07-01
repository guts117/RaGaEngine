#version 460

layout (location = 0) in vec3 pos;

out vec3 TexCoord;
out vec4 ClipSpacePos;
out vec4 PrevClipSpacePos;

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 prevPVM;

void main()
{
	TexCoord = pos;
	vec4 ClipSpacePosition =  Projection * View * vec4(pos, 1.0);	
	
	gl_Position = ClipSpacePosition.xyww;
	
	ClipSpacePos = ClipSpacePosition;
	PrevClipSpacePos = prevPVM * vec4(pos, 1.0);
}
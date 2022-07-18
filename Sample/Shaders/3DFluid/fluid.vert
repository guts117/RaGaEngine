#version 460												
															
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex;							
layout (location = 2) in vec3 norm;
																			
out vec3 VertexPos;
out vec3 LocalEyePos;

uniform mat4 Model;						
uniform mat4 Projection;
uniform mat4 View;
uniform vec3 eyePosition;

void main()							
{
	vec4 ClipSpacePosition = Projection * View * Model *  vec4(pos, 1.0);	
	gl_Position = ClipSpacePosition;	

	VertexPos = pos;
	LocalEyePos = vec3(inverse(Model) * vec4(eyePosition, 1.0));
};



#version 460

layout(early_fragment_tests) in;


in vec3 TexCoord;
in vec4 ClipSpacePos;
in vec4 PrevClipSpacePos;

layout(location = 0)out vec4 color;
layout(location = 1)out vec4 BrightColor;
layout(location = 2)out vec4 MotionVector;

uniform samplerCube skybox;

void main()
{
	color = texture(skybox, TexCoord);
	
	float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
	if(brightness>1.0f){
	BrightColor = vec4(color.rgb,1.0f);
	}
	else
	{
		BrightColor = vec4(0.0, 0.0,0.0, 1.0);
	}
	
	//motion vector
	vec3 NDCPos = (ClipSpacePos.xyz / ClipSpacePos.w)* 0.5 + 0.5;
    vec3 PrevNDCPos = (PrevClipSpacePos.xyz / PrevClipSpacePos.w)* 0.5 + 0.5;
    MotionVector = vec4(NDCPos - PrevNDCPos, 1.0f);
}

#version 460 core

// Interpolated values from the vertex shaders
in vec2 TexCoord;
in vec4 ClipSpacePos;
in vec4 PrevClipSpacePos;

// Ouput data
layout(early_fragment_tests) in;
layout(location = 0)out vec4 color;
layout(location = 1) out vec4 BrightColor;
layout(location = 2)out vec4 MotionVector;								

uniform sampler2D theTexture;

void main(){
	
	/*float LifeLevel = 1.0f;
	
	// Output color = color of the texture at the specified UV
	color = texture( theTexture, TexCoord );
	
	// Hardcoded life level, should be in a separate texture.
	if (TexCoord.x < LifeLevel && TexCoord.y > 0.3 && TexCoord.y < 0.7 && TexCoord.x > 0.04 )
		color = vec4(0.2, 0.8, 0.2, 1.0); // Opaque green*/
		
	//if blending not enabled 
	vec4 texColor = texture(theTexture, TexCoord);	
	/*if(texColor.a < 0.1)
        discard;*/          //discard wont work with early_fragment_tests
    color = texColor;
	
	float brightness = dot(texColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if(brightness>1.0f){
	BrightColor = vec4(texColor.rgb,1.0f);
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
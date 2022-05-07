#version 460 core

layout(early_fragment_tests) in;

// Interpolated values from the vertex shaders
in vec2 TexCoord;
in vec4 particlecolor;

// Ouput data
layout(location = 0)out vec4 color;
layout(location = 1)out vec4 BrightColor;
layout(location = 2)out vec4 MotionVector;

uniform sampler2D theTexture;

void main(){
	// Output color = color of the texture at the specified UV
	
	color = texture(theTexture, TexCoord)*particlecolor;	
	
	//if blending not enabled 
	/*if(texColor.a < 0.1)
        discard;       //discard wont work with early_fragment_tests
    color = texColor;*/
	BrightColor = color;
	 MotionVector = vec4(0,0,0,0);
	
}
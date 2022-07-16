#version 460 core

layout(early_fragment_tests) in;

// Interpolated values from the vertex shaders
in vec2 TexCoord;

layout(binding = 1) uniform sampler2DArray theTexture;
uniform int texIndex;
//layout(binding = 1) uniform sampler2D theTexture;

// Ouput data
layout(location = 0)out vec4 color;

void main(){
	// Output color = color of the texture at the specified UV

	//color = vec4(1, 0, 0, 1);
	color = texture(theTexture, vec3(TexCoord, texIndex)); 
	//color = texture(theTexture, TexCoord);
	//if blending not enabled 
	/*if(texColor.a < 0.1)
        discard;       //discard wont work with early_fragment_tests
    color = texColor;*/
}
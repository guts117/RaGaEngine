#version 460

layout(early_fragment_tests) in;

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D theTexture;

void main()
{
	vec2 texelSize = 1.0 / vec2(textureSize(theTexture, 0));
    vec3 Color = vec3(0.0, 0.0, 0.0);

    for (int i = -2 ; i < 2 ; i++) {
        for (int j = -2 ; j < 2; j++) {
           vec2 offset = vec2(float(i), float(j)) * texelSize;
           Color += texture(theTexture, TexCoord + offset).r;
        }
    }

    Color /= 16.0;                            //(i+j)*(i+j) ignore the negative sign. Should be equal to the noise dimension in the ssao shader

    FragColor = vec4(Color, 1.0);
}
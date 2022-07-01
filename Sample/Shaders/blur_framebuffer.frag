#version 460 core

layout(early_fragment_tests) in;
out vec4 FragColor;
  
in vec2 TexCoord;

uniform sampler2D theTexture;
  
uniform bool horizontal;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{             
    vec2 tex_offset = 1.0 / textureSize(theTexture, 0); // gets size of single texel
    vec3 result = texture(theTexture, TexCoord).rgb * weight[0]; // current fragment's contribution
    if(horizontal)
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(theTexture, TexCoord + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(theTexture, TexCoord - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(theTexture, TexCoord + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(theTexture, TexCoord - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }
    FragColor = vec4(result, 1.0);
}
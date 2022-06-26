#version 460

layout(early_fragment_tests) in;

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D theTexture;
uniform sampler2D blurTexture;
uniform bool hdr;
uniform float exposure;

void main()
{      
	const float gamma = 2.2;
    vec3 hdrColor = texture(theTexture, TexCoord).rgb;
	vec3 bloomColor = texture(blurTexture, TexCoord).rgb;
	hdrColor +=bloomColor;
	
    if(hdr)
    {
        // reinhard
        //vec3 result = hdrColor / (hdrColor + vec3(1.0));
        // exposure
        vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
		
        // also gamma correct while we're at it       
        result = pow(result, vec3(1.0 / gamma));
        FragColor = vec4(result, 1.0);
    }
    else
    {
        vec3 result = pow(hdrColor, vec3(1.0 / gamma));
        FragColor = vec4(result, 1.0);
    }	
}

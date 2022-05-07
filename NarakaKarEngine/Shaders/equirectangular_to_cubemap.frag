#version 460

layout(early_fragment_tests) in;

in vec3 TexCoord;			//WorldPos
layout(location = 0)out vec4 color;
layout(location = 1) out vec4 BrightColor;
layout(location = 2)out vec4 MotionVector;	

uniform sampler2D theTexture;  //equirectangularMap

const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
	vec2 uv = SampleSphericalMap(normalize(TexCoord));
    vec3 texColor = texture(theTexture, uv).rgb;
    
    color = vec4(texColor, 1.0);
	
	float brightness = dot(texColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if(brightness>1.0f){
	BrightColor = vec4(texColor.rgb,1.0f);
	}
	else
	{
		BrightColor = vec4(0.0, 0.0,0.0, 1.0);
	}
	
	MotionVector = vec4(0,0,0,0);
}

#version 460

//layout(early_fragment_tests) in;  //dont do this here
in vec4 FragPos;

uniform vec3 lightPos;
uniform float farPlane;

void main()
{	
	float distance = length(FragPos.xyz - lightPos);
	distance = distance/farPlane;
	gl_FragDepth = distance;          //early_fragment_tests fucks this up
	
}
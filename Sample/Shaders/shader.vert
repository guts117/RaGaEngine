#version 460												
															
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex;							
layout (location = 2) in vec3 norm;
layout (location = 3) in vec3 tangent;
									
//out vec4 vCol;
out vec2 TexCoord;	
out vec3 Normal;											
out mat3 TBN;
out vec3 FragPos;
out vec4 DirectionalLightSpacePos;

out vec4 ClipSpacePos;
out vec4 PrevClipSpacePos;

uniform mat4 Model;						
uniform mat4 Projection;
uniform mat4 View;

uniform mat4 prevPVM;					
uniform mat4 DirectionalLightTransform;


mat3 CalcTBN(vec3 Normal, vec3 Tangent)
{
	vec3 normal = normalize(Normal);
    vec3 tangent = normalize(Tangent);
	tangent = normalize(tangent - dot(tangent, normal)* normal);
	vec3 Bitangent = cross(tangent, normal);
	
	/*if (dot(cross(tangent, normal), Bitangent) < 0.0f){
		tangent = tangent * -1.0f;
	}*/
	
	mat3 TBN = mat3(tangent, Bitangent, normal);
	
	return TBN;
}	

void main()							
{
	vec4 ClipSpacePosition = Projection * View * Model *  vec4(pos, 1.0);	
	gl_Position = ClipSpacePosition;	
	DirectionalLightSpacePos = DirectionalLightTransform * Model * vec4(pos, 1.0);	
	//vCol = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);

	TexCoord = tex;	
	
	vec3 T = mat3(Model)*tangent;
	Normal = mat3(Model)*norm;
	
	TBN = CalcTBN(Normal, T);
	FragPos = (Model * vec4(pos, 1.0f)).xyz;	
	
	ClipSpacePos = ClipSpacePosition;
    PrevClipSpacePos =  prevPVM* vec4(pos, 1.0);
};



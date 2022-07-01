#version 460												

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex;							
layout (location = 2) in vec3 norm;
layout (location = 3) in vec3 tangent;
layout (location = 4) in ivec4 BoneIDs;
layout (location = 5) in vec4 Weights;

//out vec4 vCol;
out vec2 TexCoord;												
out vec3 Normal;
out mat3 TBN;
out mat3 TBN2;
out vec3 FragPos;
out vec4 DirectionalLightSpacePos;

uniform mat4 prevPVM;

out vec4 ClipSpacePos;
out vec4 PrevClipSpacePos;

uniform mat4 Model;						
uniform mat4 Projection;
uniform mat4 View;					
uniform mat4 DirectionalLightTransform;

const int MAX_BONES = 100;
uniform mat4 gBones[MAX_BONES];
uniform mat4 gPrevBones[MAX_BONES];

mat3 CalcTBN(vec3 Normal, vec3 Tangent)
{
	vec3 normal = normalize(Normal);
    vec3 tangent = normalize(Tangent);
    tangent = normalize(tangent - dot(normal, tangent));
	vec3 Bitangent = cross(normal, tangent);
	
	if (dot(cross(normal, tangent), Bitangent) < 0.0f){
		tangent = tangent * -1.0f;
	}
	mat3 TBN = mat3(tangent, Bitangent, normal);
	
	return TBN;
}	
	

void main()							
{	
	mat4 BoneTransform = gBones[BoneIDs[0]] * Weights[0];
    	BoneTransform += gBones[BoneIDs[1]] * Weights[1];
    	BoneTransform += gBones[BoneIDs[2]] * Weights[2];
    	BoneTransform += gBones[BoneIDs[3]] * Weights[3];

	vec4 PosL = BoneTransform * vec4(pos, 1.0);
	vec4 ClipSpacePosition = Projection * View * Model * PosL;	
	gl_Position = ClipSpacePosition;
	DirectionalLightSpacePos = DirectionalLightTransform * Model * vec4(PosL.xyz, 1.0);	
	//vCol = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);

	TexCoord = tex;	

	vec3 T = mat3(Model)*tangent;
	Normal = mat3(Model)*norm;
	
	TBN = CalcTBN(Normal, T);
	FragPos = (Model * vec4(PosL.xyz, 1.0f)).xyz;
    
	//motion vector
	mat4 PrevBoneTransform = gPrevBones[BoneIDs[0]] * Weights[0];
    PrevBoneTransform += gPrevBones[BoneIDs[1]] * Weights[1];
    PrevBoneTransform += gPrevBones[BoneIDs[2]] * Weights[2];
    PrevBoneTransform += gPrevBones[BoneIDs[3]] * Weights[3];

    ClipSpacePos = ClipSpacePosition;
    vec4 PrevPosL = PrevBoneTransform * vec4(pos, 1.0);
    PrevClipSpacePos =  prevPVM * PrevPosL;
};
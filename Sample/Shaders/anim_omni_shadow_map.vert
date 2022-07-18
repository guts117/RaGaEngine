#version 460

layout(location = 0) in vec3 pos;
layout (location = 4) in ivec4 BoneIDs;
layout (location = 5) in vec4 Weights;

uniform mat4 Model;

const int MAX_BONES = 100;
uniform mat4 gBones[MAX_BONES];

void main()
{
	mat4 BoneTransform = gBones[BoneIDs[0]] * Weights[0];
    	BoneTransform += gBones[BoneIDs[1]] * Weights[1];
    	BoneTransform += gBones[BoneIDs[2]] * Weights[2];
    	BoneTransform += gBones[BoneIDs[3]] * Weights[3];

	vec4 PosL = BoneTransform * vec4(pos, 1.0);	

	gl_Position = Model* PosL;
}
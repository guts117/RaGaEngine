#ifndef VERTEX_BONE_DATA
#define VERTEX_BONE_DATA

#include "CommonValues.h"

class VertexBoneData
{
public:
	explicit VertexBoneData();

	void AddBoneData(unsigned int BoneID, float Weight);
private:
	unsigned int IDs[NUM_BONES_PER_VERTEX];
	float Weights[NUM_BONES_PER_VERTEX];
};

#endif
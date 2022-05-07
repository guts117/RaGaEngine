#pragma once
#include "CommonValues.h"
#include <cstdio>
#include <string>

class VertexBoneData
{
public:
	unsigned int IDs[NUM_BONES_PER_VERTEX];
	float Weights[NUM_BONES_PER_VERTEX];

	VertexBoneData()
	{
		memset(IDs, 0, sizeof(IDs));    // init all values in array = 0
		memset(Weights, 0, sizeof(Weights));
	}


	void AddBoneData(unsigned int BoneID, float Weight);
};


#include "VertexBoneData.h"
#include <cstdio>
#include <string>

VertexBoneData::VertexBoneData()
{
	memset(IDs, 0, sizeof(IDs));    // init all values in array = 0
	memset(Weights, 0, sizeof(Weights));
}

void VertexBoneData::AddBoneData(unsigned int BoneID, float Weight)
{
	for (std::size_t i = 0; i < NUM_BONES_PER_VERTEX; i++)
	{

		if (Weights[i] == 0.0f)
		{
			IDs[i] = BoneID;
			Weights[i] = Weight;
			return;
		}

		//assert(0);
	}
}

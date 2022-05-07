#include "VertexBoneData.h"

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

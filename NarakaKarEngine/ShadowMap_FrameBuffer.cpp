#include "ShadowMap_Framebuffer.h"

bool ShadowMap_Framebuffer::Init(GLuint width, GLuint height)
{
	src_width = width; src_height = height;

	glGenFramebuffers(1, &FBO);
	glGenTextures(NUM_CASCADES, buffers);

	for (int i = 0; i < NUM_CASCADES; ++i) {
		glBindTexture(GL_TEXTURE_2D, buffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, src_width, src_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float bColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, bColor);
	}
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, buffers[0], 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		printf("Framebufer error: %i\n", status);
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

void ShadowMap_Framebuffer::Write()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
}



void ShadowMap_Framebuffer::Write(int CascadeIndex)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, buffers[CascadeIndex], 0);
}

void ShadowMap_Framebuffer::Read(int i, GLenum textureUnit)
{
	glActiveTexture(textureUnit+ i);
	glBindTexture(GL_TEXTURE_2D, buffers[i]);

	/*glActiveTexture(textureUnit+i);
	glBindTexture(GL_TEXTURE_2D, buffers[i]);*/
}

float ShadowMap_Framebuffer::GetCascadeEnd(unsigned int i)
{
	return cascadeEnd[i];
}

void ShadowMap_Framebuffer::CalcOrthProjs(const glm::mat4& Cam, const glm::mat4* vView, const float& angle)
{
	float ar = float(ScreenHeight)/float(ScreenWidth);
	glm::mat4 CamInv = glm::inverse(Cam);

	float tanHalfHFOV = glm::tan(glm::radians(angle / 2.0f)) /ar;
	float tanHalfVFOV = glm::tan(glm::radians(angle * ar / 2.0f)) / ar;

	for (unsigned int i = 0; i < NUM_CASCADES; ++i)
	{
		float xn = cascadeEnd[i] * tanHalfHFOV;
		float xf = cascadeEnd[i + 1] * tanHalfHFOV;
		float yn = cascadeEnd[i] * tanHalfVFOV;
		float yf = cascadeEnd[i + 1] * tanHalfVFOV;

		glm::vec4 frustrumCorners[NUM_FRUSTUM_CORNERS] = {
			//near face
			glm::vec4(xn,yn,-cascadeEnd[i],1.0f),
			glm::vec4(-xn,yn,-cascadeEnd[i],1.0f),
			glm::vec4(xn,-yn,-cascadeEnd[i],1.0f),
			glm::vec4(-xn,-yn,-cascadeEnd[i],1.0f),
			//far Face
			glm::vec4(xf,yf,-cascadeEnd[i + 1],1.0f),
			glm::vec4(-xf,yf,-cascadeEnd[i + 1],1.0f),
			glm::vec4(xf,-yf,-cascadeEnd[i + 1],1.0f),
			glm::vec4(-xf,-yf,-cascadeEnd[i + 1],1.0f)
		};

		float minX = std::numeric_limits<float>::max();
		float maxX = std::numeric_limits<float>::min();
		float minY = std::numeric_limits<float>::max();
		float maxY = std::numeric_limits<float>::min();
		float minZ = std::numeric_limits<float>::max();
		float maxZ = std::numeric_limits<float>::min();

		for (unsigned int j = 0; j < NUM_FRUSTUM_CORNERS; ++j)
		{
			glm::vec4 vW = CamInv * frustrumCorners[j];
			frustrumCorners[j] = vView[0] * vW;

			minX = glm::min(minX, frustrumCorners[j].x);
			maxX = glm::max(maxX, frustrumCorners[j].x);
			minY = glm::min(minY, frustrumCorners[j].y);
			maxY = glm::max(maxY, frustrumCorners[j].y);
			minZ = glm::min(minZ, frustrumCorners[j].z);
			maxZ = glm::max(maxZ, frustrumCorners[j].z);

			if (j == 7)
			{
				modeldFrusCorns[i][0] = glm::inverse(vView[0]) * glm::vec4(maxX, maxY, minZ, 1.0f);
				modeldFrusCorns[i][1] = glm::inverse(vView[0]) * glm::vec4(minX, maxY, minZ, 1.0f);
				modeldFrusCorns[i][2] = glm::inverse(vView[0]) * glm::vec4(maxX, minY, minZ, 1.0f);
				modeldFrusCorns[i][3] = glm::inverse(vView[0]) * glm::vec4(minX, minY, minZ, 1.0f);
				modeldFrusCorns[i][4] = glm::inverse(vView[0]) * glm::vec4(maxX, maxY, maxZ, 1.0f);
				modeldFrusCorns[i][5] = glm::inverse(vView[0]) * glm::vec4(minX, maxY, maxZ, 1.0f);
				modeldFrusCorns[i][6] = glm::inverse(vView[0]) * glm::vec4(maxX, minY, maxZ, 1.0f);
				modeldFrusCorns[i][7] = glm::inverse(vView[0]) * glm::vec4(minX, minY, maxZ, 1.0f);
			}
		}
	}
}

glm::mat4 ShadowMap_Framebuffer::GetProjMat(glm::mat4& view, unsigned int index)
{
	return glm::ortho((view * modeldFrusCorns[index][1]).x, (view * modeldFrusCorns[index][0]).x, (view * modeldFrusCorns[index][2]).y, (view * modeldFrusCorns[index][0]).y, (view * modeldFrusCorns[index][0]).z, (view * modeldFrusCorns[index][4]).z);
}

float ShadowMap_Framebuffer::GetRatio(glm::mat4& view, int index)
{
	if (index > -1 && index < NUM_CASCADES+1)
	{
		return((-(view * modeldFrusCorns[index][0]).z + (view * modeldFrusCorns[index][4]).z));
	}
	else
	{
		return 1.0f;
	}
}

glm::vec3 ShadowMap_Framebuffer::GetModlCent(unsigned int index)
{
	glm::vec4 temp = glm::vec4(0.0f);
	for (unsigned int i = 0; i < NUM_FRUSTUM_CORNERS; ++i)
	{
		temp += modeldFrusCorns[index][i];
	}
	glm::vec4 temp2 = temp / static_cast<float > (NUM_FRUSTUM_CORNERS);
	return glm::vec3(temp2);
}

ShadowMap_Framebuffer::~ShadowMap_Framebuffer()
{
	if (FBO) {
		glDeleteFramebuffers(1, &FBO);
	} 
	if (buffers) {
		glDeleteTextures(NUM_CASCADES, buffers);
	}
}

#ifndef COMMONVALS
#define COMMONVALS

#include "stb_image.h"
#define _USE_MATH_DEFINES
#include <math.h>

const int MAX_POINT_LIGHTS_WITH_SHADOW = 3;
const int MAX_SPOT_LIGHTS = 3;
const unsigned int MAX_BONES = 100;
const unsigned int NUM_BONES_PER_VERTEX = 4;
const int MaxParticles = 1000;
const int ScreenWidth = 1920;
const int ScreenHeight = 1080;
const int NUM_CASCADES = 3;
const int NUM_FRUSTUM_CORNERS = 8;
const int NUM_OBJECTS = 3;
const int NUM_TERRAIN_LAYERS = 4;
const float toRadians = static_cast<float>(M_PI) / 180.0f;
const int albedoTexUnit		= 0;
const int metallicTexUnit	= 5;
const int normalTexUnit		= 6;
const int roughTexUnit		= 10;
const int parallaxTexUnit	= 11;
const int glowTexUnit		= 12;
const int mainShadowTexUnit = 2;
const int irradianceTexUnit = 8;
const int prefilterTexUnit	= 9;
const int brdflutTexUnit	= 10;
const int aoTexUnit			= 14;
const float camNearZ		= 0.1f;
const float camFarZ			= 1000.0f;
#endif
#version 460												

const int NUM_CASCADES 				= 3;
const int MAX_POINT_LIGHTS_SHADOW  	= 3;
const int MAX_SPOT_LIGHTS 			= 3;
const float MAX_REFLECTION_LOD 		= 4.0;
const float PI 						= 3.14159265359;
	
const vec3 sampleOffsetDirections[20] 
	= vec3[]
	(
	   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
	   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
	   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
	   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
	   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
	);
	
in vec2 TexCoord;
in vec2 TexCoordTerr;
in vec3 Normal;
in mat3 TBN;
in vec3 FragPos;

in vec4 DirectionalLightSpacePos[NUM_CASCADES];
in float ClipSpacePosZ;

in vec4 ClipSpacePos;
in vec4 PrevClipSpacePos;

layout(early_fragment_tests) in;
layout(location = 0)out vec4 color;		
layout(location = 1)out vec4 BrightColor;		
layout(location = 2)out vec4 MotionVector;								

struct DirectionalLight
{
	vec3 color;
	vec3 direction;
};

//Cluster shading structs and buffers
struct PointLight{
    vec4 position;
    vec4 color;
	bool enabled;
    float intensity;
    float range;
};
struct LightGrid{
    uint offset;
    uint count;
};
layout (std430, binding = 2) readonly buffer screenToView{
    mat4 inverseProjection;
    uvec4 tileSizes;
	uvec2 tileSizeInPixel;
    uvec2 screenDimensions;
    float scale;
	float bias;
	float zNear;
	float zFar;
};
layout (std430, binding = 3) readonly buffer lightSSBO{
    PointLight pointLight[];
};
layout (std430, binding = 4) buffer lightIndexSSBO{
    uint globalLightIndexList[];
};
layout (std430, binding = 5) buffer lightGridSSBO{
    LightGrid lightGrid[];
};

struct SpotLight
{
	vec3 color;
    vec3 position;
	vec3 direction;
	float edge;
};

struct OmniShadowMap
{
	samplerCube shadowMap;
	float farPlane;
};

struct DirectionalShadowMaps
{
	sampler2D shadowMap;
};

struct Material
{
	sampler2DArray parallaxMap;
    sampler2DArray roughnessMap;
	sampler2DArray normalMap;
	sampler2DArray metallicMap;
	sampler2DArray albedoMap;
};

uint PointLightCount;
uniform int SpotLightCount;

uniform DirectionalLight directionalLight;
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;
uniform float CascadeEndClipSpace[NUM_CASCADES];
uniform DirectionalShadowMaps directionalShadowMaps[NUM_CASCADES];
uniform sampler2D AOMap;
uniform sampler2D depthMap;
uniform sampler2D blendMap;
uniform OmniShadowMap omniShadowMaps[MAX_POINT_LIGHTS_SHADOW + MAX_SPOT_LIGHTS];

uniform Material material;

uniform vec3 eyePosition;

uniform float height_scale;

//Debug from here
uniform bool showAO;
uniform bool showDepth;
uniform bool showLightSlices;
uniform bool showShadowSlices;

vec4 cascadeCol 			= vec4(0.0, 0.0, 0.0, 0.0);

vec3 colors[8] 				= vec3[](
   vec3(0, 0, 0),    vec3( 0,  0,  1), vec3( 0, 1, 0),  vec3(0, 1,  1),
   vec3(1,  0,  0),  vec3( 1,  0,  1), vec3( 1, 1, 0),  vec3(1, 1, 1)
);
//Debug to here

vec2 CalcScreenTexCoord()
{
    return gl_FragCoord.xy / screenDimensions;
}

float CalcDirectionalShadowFactor(int cascadeIndex, vec3 lightDir, vec4 lightSpacePos)
{
	vec3 projCoords 		= lightSpacePos.xyz / lightSpacePos.w;
	projCoords 				= (projCoords * 0.5) + 0.5;
	
	float current 			= projCoords.z;

	vec3 normal 			= normalize(Normal);
	lightDir 				= normalize(lightDir);
	
	float bias 				= 0.0;
	if(cascadeIndex > 1) 
	{
		bias 				= max(0.001 * (1 - dot(normal, lightDir)), 0.0002);
	}
	else
	{
		bias 				= max(0.0001 * (1 - dot(normal, lightDir)), 0.0001);
	}
	
	float shadow 			= 0.0;
	
	vec2 texelSize 			= 1.0 / textureSize(directionalShadowMaps[cascadeIndex].shadowMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth 	= texture(directionalShadowMaps[cascadeIndex].shadowMap, projCoords.xy + vec2(x,y)* texelSize).r;
			shadow 			+= current - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	
	shadow					/= 9.0;
	
	if(projCoords.z > 1.0)
	{
		shadow = 0.0;
	}
	
	return shadow;
}

float CalcOmniShadowFactor(vec3 lightPos, uint shadowIndex, float bias)
{
	vec3 fragToLight 		= FragPos - lightPos;
	float current 			= length(fragToLight);
	
	float shadow 			= 0.0;
	int samples 			= 20;
	
	float viewDistance 		= length(eyePosition - FragPos);
	float diskRadius 		= (1.0 + (viewDistance / omniShadowMaps[shadowIndex].farPlane))/25.0;
	
	for(int i = 0; i< samples; i++)
	{
		float closest 		= texture(omniShadowMaps[shadowIndex].shadowMap, fragToLight + sampleOffsetDirections[i]*diskRadius).r;
		closest 			*= omniShadowMaps[shadowIndex].farPlane;
		if(current - bias > closest)
		{
			shadow 			+=1.0;
		}
	}
	
	shadow /= float(samples);

	return shadow;
}

float DistributionGGX(float NdotH, float roughness)
{
	float a 				= roughness * roughness;
	float a2 				= a * a;
	float NdotH2 			= NdotH * NdotH;
	
	float nom 				= a2;
	float denom 			= (NdotH2 * (a2 - 1.0) + 1.0);
	denom 					= PI * denom * denom;
	
	return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r 				= (roughness + 1.0);
	float k 				= (r * r) / 8.0;
	
	float nom 				= NdotV;
	float denom 			= NdotV * (1.0 - k) + k;
	
	return nom / denom;
}

float GeometrySmith(float NdotV, float NdotL, float roughness)
{
	float ggx2 				= GeometrySchlickGGX(NdotV, roughness);
	float ggx1 				= GeometrySchlickGGX(NdotL, roughness);
	
	return ggx1 * ggx2;
} 

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
	float angle 			= 1.0 - min(cosTheta, 1.0);
	float angle5 			= angle * angle * angle * angle * angle;
	return max(F0 + (1.0 - F0) * angle5, 0.0);
	//return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
	float angle 			= 1.0 - min(cosTheta, 1.0);	
	float angle5 			= angle * angle * angle * angle * angle;
	return max(F0 + (max(vec3(1.0 - roughness), F0) - F0) * angle5, 0.0);
	//return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - min(cosTheta, 1.0), 5.0);
}

vec4 CalcLightByDirection(vec3 lightColor, vec3 lightDir, vec3 viewDir, vec3 normal, vec3 F0, vec3 albedo, float metallic, float roughness, float shadowFactor, bool is_DirectionLight, float radius)
{
	float distance			= length(lightDir);
	lightDir 				= normalize(lightDir);

	vec3 halfway 			= normalize(viewDir + lightDir);
	
	float attenuation 		= 0.0f;
	vec3 radiance			= vec3(0.0, 0.0, 0.0);
	if(is_DirectionLight) 
	{ 
		radiance 			= lightColor; 
	}
	else					
	{ 
		if (radius == -1) 
		{
			attenuation 	= 1.0 / (distance * distance);
		}
		else
		{
			float dByR 		= distance / radius;
			float dByR4		= dByR * dByR * dByR * dByR;
			float dByR4C 	= clamp(1 - dByR4, 0.0, 1.0);
			float dByR4C2 	= dByR4C * dByR4C;
			attenuation 	= dByR4C2 / (1.0  + (distance * distance)); 
			//attenuation 	= 1.0f - smoothstep(radius * 0.75f, radius, distance); 
		}
		radiance 			= lightColor * attenuation; 	
	}
	
	float NdotV 			= max(dot(normal, viewDir), 0.0);
	float NdotL 			= max(dot(normal, lightDir), 0.0);
	float NdotH 			= max(dot(normal, halfway), 0.0);
	float HdotV				= max(dot(halfway, viewDir), 0.0);
	
	float NDF 				= DistributionGGX(NdotH, roughness);
	float G 				= GeometrySmith(NdotV, NdotL, roughness);
	vec3 F 					= FresnelSchlick(HdotV, F0);
	
	vec3 nominator 			= NDF * G * F;
	float denominator 		= 4 * max(NdotV * NdotL, 0.0);
	vec3 specular 			= nominator / max(denominator, 0.0001);
	
	vec3 kS 				= F;
	vec3 kD					= vec3(1.0) - kS;
	kD						*= 1.0 - metallic;
		
	vec3 Lo 				= (kD * (albedo / PI) + specular) * radiance * NdotL;
	
	return vec4((1.0 - shadowFactor) * Lo, 1.0);
}

vec4 CalcDirectionalLight(vec3 viewDir, vec3 normal, vec3 F0, vec3 albedo, float metallic, float roughness)
{
	float shadowFactor 		= 0.0;

	for (int i = 0 ; i < NUM_CASCADES ; i++) 
	{
		if (ClipSpacePosZ <= CascadeEndClipSpace[i]) 
		{
			shadowFactor	= CalcDirectionalShadowFactor(i, directionalLight.direction, DirectionalLightSpacePos[i]);
			if (i == 0) 
				cascadeCol	= vec4(0.1, 0.0, 0.0, 0.0);
			else if (i == 1)
				cascadeCol	= vec4(0.0, 0.1, 0.0, 0.0);
			else if (i == 2)
				cascadeCol	= vec4(0.0, 0.0, 0.1, 0.0);
			break;
		}
	}	
	return CalcLightByDirection(directionalLight.color, -directionalLight.direction, viewDir, normal, F0, albedo, metallic, roughness, shadowFactor, true, -1);
}

vec4 CalcPointLight(PointLight pLight, vec3 viewDir, vec3 normal, vec3 F0, vec3 albedo, float metallic, float roughness, uint shadowIndex)
{
	vec3 lightDir 			= pLight.position.xyz - FragPos;		
	float bias 				= max(0.5 * (1 - dot(normal, normalize(lightDir))), 0.1);
	float shadowFactor 		= CalcOmniShadowFactor(pLight.position.xyz, shadowIndex, bias);		
	vec4 color 				= CalcLightByDirection(pLight.color.rgb * pLight.intensity, lightDir, viewDir, normal, F0, albedo, metallic, roughness, shadowFactor, false, pLight.range);	
	return color;
}

vec4 CalcSpotLight(SpotLight sLight, vec3 viewDir, vec3 normal, vec3 F0, vec3 albedo, float metallic, float roughness, uint shadowIndex)
{
	vec3 rayDirection 		= normalize(FragPos - sLight.position);
	float slFactor 			= dot(rayDirection, sLight.direction);

	if(slFactor > sLight.edge)
	{	
		vec3 lightDir 		= normalize(sLight.position - FragPos);
		float bias 			= max(0.5 * (1 - dot(normal, lightDir)), 0.1);	
		float shadowFactor 	= CalcOmniShadowFactor(sLight.position, shadowIndex, bias);
		vec4 color 			= CalcLightByDirection(sLight.color, lightDir, viewDir, normal, F0, albedo, metallic, roughness, shadowFactor, false, -1);	
		return color * (1.0f - (1.0f - slFactor) * (1.0f / (1.0f - sLight.edge)));
	}
	else
	{
		return vec4(0, 0, 0, 0);
	}
}

float LinearDepth(float depthSample){
    float depthRange		= 2.0 * depthSample - 1.0;
    float linear 			= 2.0 * zNear * zFar / (zFar + zNear - depthRange * (zFar - zNear));
    return linear;
}

vec4 CalcPointLights(vec3 viewDir, vec3 normal, vec3 F0, vec3 albedo, float metallic, float roughness, uint tileIndex)
{
	//ToDo:
	PointLightCount 		= 2;

	vec4 totalColor 		= vec4(0, 0, 0, 1);		//set alpha to 1 when using blending
	
    uint lightCount       	= lightGrid[tileIndex].count;
    uint lightIndexOffset 	= lightGrid[tileIndex].offset;

    //Reading from the global light list and calculating the radiance contribution of each light.
    for(int i = 0; i < lightCount; i++){
        uint lightIndex		= globalLightIndexList[lightIndexOffset + i];
        totalColor 			+= CalcPointLight(pointLight[lightIndex], viewDir, normal, F0, albedo, metallic, roughness, lightIndex);
    }

	return totalColor;
}

vec4 CalcSpotLights(vec3 viewDir, vec3 normal, vec3 F0, vec3 albedo, float metallic, float roughness)
{
	vec4 totalColor 		= vec4(0, 0, 0, 1); //set alpha to 1 when using blending
	for(int i = 0; i < SpotLightCount; i++)
	{
		totalColor 			+= CalcSpotLight(spotLights[i], viewDir, normal, F0, albedo, metallic, roughness, i + PointLightCount);
	}
	
	return totalColor;
}

vec3 CalcMultipleTexture(sampler2DArray mat, vec2 texCoord, vec3 color, float amnt)
{
	vec3 bgrTextureColor 	= texture(mat, vec3(texCoord,0)).rgb * amnt; 
	vec3 rTextureColor 		= texture(mat, vec3(texCoord,1)).rgb * color.r;
	vec3 gTextureColor 		= texture(mat, vec3(texCoord,2)).rgb * color.g;
	vec3 bTextureColor 		= texture(mat, vec3(texCoord,3)).rgb * color.b;
	return bgrTextureColor + rTextureColor + gTextureColor + bTextureColor;
}	

vec3 CalcBumpedNormal(vec2 texCoord, vec3 Color, float Amnt)
{
    vec3 bumpedNormal 		= CalcMultipleTexture(material.normalMap, texCoord, Color, Amnt);
    bumpedNormal 			= 2.0 * bumpedNormal - vec3(1.0, 1.0, 1.0);
    bumpedNormal 			= TBN * bumpedNormal;
    bumpedNormal 			= normalize(bumpedNormal);
    return bumpedNormal;
}

vec2 CalcParallaxMapping(vec3 viewDir, vec2 texCoord, vec3 Color, float Amnt)
{
	const float minLayers 	= 8.0;
	const float maxLayers 	= 32.0;
	// number of depth layers
	float numLayers 		= mix(maxLayers, minLayers, max(dot(vec3(0.0, 0.0, 1.0), viewDir), 0.0));  
    // calculate the size of each layer
    float layerDepth 		= 1.0 / numLayers;
    // depth of current layer
    float currLayerDepth 	= 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P 					= viewDir.xy * height_scale; 
    vec2 deltaTexCoords 	= P / numLayers;
	
	// get initial values
	vec2  currTexCoords     = texCoord;
	float currDepthMapValue = CalcMultipleTexture(material.parallaxMap,currTexCoords, Color, Amnt).r;
	  
	while(currLayerDepth < currDepthMapValue)
	{
		// shift texture coordinates along direction of P
		currTexCoords 		-= deltaTexCoords;
		// get depthmap value at current texture coordinates
		currDepthMapValue 	= CalcMultipleTexture(material.parallaxMap, currTexCoords, Color, Amnt).r;  
		// get depth of next layer
		currLayerDepth 		+= layerDepth;  
	}

	// get texture coordinates before collision (reverse operations)
	vec2 prevTexCoords 		= currTexCoords + deltaTexCoords;

	// get depth after and before collision for linear interpolation
	float afterDepth  		= currDepthMapValue - currLayerDepth;
	float beforeDepth 		= CalcMultipleTexture(material.parallaxMap, prevTexCoords, Color, Amnt).r - currLayerDepth + layerDepth;

	// interpolation of texture coordinates
	float weight 			= afterDepth / (afterDepth - beforeDepth);
	vec2 finalTexCoords 	= prevTexCoords * weight + currTexCoords * (1.0 - weight);

	return finalTexCoords;	
}
	
void main()												
{
	vec3 blendMapColor 		= texture(blendMap, TexCoordTerr).rgb;
	float bgrTxtreAmnt 		= 1 - (blendMapColor.r + blendMapColor.g + blendMapColor.b);
	vec2 newTexCoord		= vec2(0.0, 0.0);
	vec3 viewDir			= vec3(0.0, 0.0, 0.0);
	
	if(height_scale == 0.0f)
	{
		newTexCoord 		= TexCoord;
	}
	else
	{
		viewDir 			= normalize(transpose(TBN) * (eyePosition - FragPos));
		newTexCoord 		= CalcParallaxMapping(viewDir, TexCoord, blendMapColor, bgrTxtreAmnt);	
	}

	vec3 newNormal 			= CalcBumpedNormal(newTexCoord, blendMapColor, bgrTxtreAmnt);
	
	vec3 albedo 			= CalcMultipleTexture(material.albedoMap, newTexCoord, blendMapColor, bgrTxtreAmnt);
	float metallic 			= CalcMultipleTexture(material.metallicMap, newTexCoord, blendMapColor, bgrTxtreAmnt).r;
	float roughness 		= CalcMultipleTexture(material.roughnessMap, newTexCoord, blendMapColor, bgrTxtreAmnt).r;

	viewDir					= normalize(eyePosition - FragPos);
	vec3 R 					= reflect(-viewDir , newNormal);
	vec3 F0 				= vec3(0.04);
	F0 						= mix(F0, albedo, metallic);
	
	//Locating which cluster you are a part of
	ivec2 pixelPos 			= ivec2(gl_FragCoord.xy);
	float z 				= LinearDepth(texelFetch(depthMap, pixelPos, 0).r);
	uint zTile     			= uint(max(log(z) * scale + bias, 0.0));
    uvec3 tiles    			= uvec3(uvec2(pixelPos.x / tileSizeInPixel.x, pixelPos.y / tileSizeInPixel.y), zTile);
    uint tileIndex 			= tiles.x + tileSizes.x * tiles.y + (tileSizes.x * tileSizes.y) * tiles.z;

	vec4 finalColor 		=  CalcDirectionalLight(viewDir, newNormal, F0, albedo, metallic, roughness);
	finalColor 				+= CalcPointLights(viewDir, newNormal, F0, albedo, metallic, roughness, tileIndex);
	finalColor 				+= CalcSpotLights(viewDir, newNormal, F0, albedo, metallic, roughness);
	
	 // ambient lighting (we now use IBL as the ambient term)
	vec3 F = FresnelSchlickRoughness(max(dot(newNormal, viewDir), 0.0), F0, roughness);
	
    vec3 kS 				= F;
    vec3 kD 				= 1.0 - kS;
    kD 						*= 1.0 - metallic;	  
    vec3 irradiance 		= texture(irradianceMap, newNormal).rgb;
    vec3 diffuse    		= irradiance * albedo;
	
	//sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular par.
	vec3 prefilteredColor 	= textureLod(prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;  
	vec2 brdf 				= texture(brdfLUT, vec2(max(dot(newNormal, viewDir), 0.0), roughness)).rg;
	vec3 specular 			= prefilteredColor * (F * brdf.x + brdf.y); //multiplying by irradiance fixes all problems
	
    vec3 ambient 			= vec3(0.0, 0.0, 0.0);
	float aoFactor 			= texture(AOMap, CalcScreenTexCoord()).r;
	
	if(showAO)
	{
		ambient 			= vec3(1.0, 1.0, 1.0);
		finalColor			= vec4(ambient * aoFactor, 1.0);
	}
	else
	{
		ambient 			= (kD * diffuse + specular) * aoFactor;
	}
	
	color 					= vec4(ambient, 1.0) + finalColor;

	if(showShadowSlices)
	{
		color 				+= cascadeCol;
	}
	
	if(showLightSlices)
	{
		float zTile_f     	= float(zTile);
		color				+= vec4(colors[uint(mod(zTile_f, 8.0))], 1.0);
	}
	
	if(showDepth)
	{
		color 				= vec4(vec3(z / zFar), 1.0);
		//color 				= vec4(vec3(z), 1.0);
	}
	
	if(color.a > 1.0f)
	{
        //discard;
		color.a 			= 1.0f;
	}	
	
	float brightness 		= dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > 1.0f && !showAO && !showDepth)
	{
		BrightColor 		= vec4(color.rgb, 1.0f);
	}
	else
	{	
		BrightColor 		= vec4(0.0, 0.0, 0.0, 1.0);
	}
	
	//motion vector
	vec3 NDCPos 			= (ClipSpacePos.xyz / ClipSpacePos.w)* 0.5 + 0.5;
    vec3 PrevNDCPos 		= (PrevClipSpacePos.xyz / PrevClipSpacePos.w)* 0.5 + 0.5;
    MotionVector 			= vec4(NDCPos - PrevNDCPos, 1.0f);
};
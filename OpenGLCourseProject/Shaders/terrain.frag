#version 460												
const int NUM_CASCADES = 3;
											
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

const int MAX_POINT_LIGHTS_SHADOW  = 3;
const int MAX_SPOT_LIGHTS = 3;
const float MAX_REFLECTION_LOD = 4.0;

const float PI = 3.14159265359;

vec3 NewNormal;
vec2 NewTexCoord;

vec4 CascadeIndicator = vec4(0.0, 0.0, 0.0, 0.0);

vec3 albedo;
float metallic;
float roughness;
vec3 N;
vec3 V;
vec3 F0 = vec3(0.04);

struct Light
{
	vec3 color;
};

struct DirectionalLight
{
	Light base;
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
layout (std430, binding = 2) buffer screenToView{
    mat4 inverseProjection;
    uvec4 tileSizes;
    uvec2 screenDimensions;
    float scale;
    float bias;
};
layout (std430, binding = 3) buffer lightSSBO{
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
	Light base;
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
uniform sampler2D blendMap;
uniform OmniShadowMap omniShadowMaps[MAX_POINT_LIGHTS_SHADOW + MAX_SPOT_LIGHTS];

uniform Material material;

uniform vec3 eyePosition;

uniform float height_scale;

uniform float camNearZ;
uniform float camFarZ;

//debug
uniform bool showAO;
uniform bool showLightSlices;
uniform bool showMotionBlur;

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

vec2 CalcScreenTexCoord()
{
    return gl_FragCoord.xy / screenDimensions;
}


float CalcDirectionalShadowFactor(int CascadeIndex, DirectionalLight light, vec4 LightSpacePos)
{
	vec3 projCoords = LightSpacePos.xyz/LightSpacePos.w;
	projCoords = (projCoords * 0.5) + 0.5;
	
	float current = projCoords.z;

	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(light.direction);
	
	float bias = 0.0;
	if(CascadeIndex>1) bias = max(0.001 * (1 - dot(normal, lightDir)), 0.0002);
	else bias = max(0.0001 * (1 - dot(normal, lightDir)), 0.0001);
	
	float shadow = 0.0;
	
	vec2 texelSize = 1.0/textureSize(directionalShadowMaps[CascadeIndex].shadowMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(directionalShadowMaps[CascadeIndex].shadowMap, projCoords.xy + vec2(x,y)* texelSize).r;
			shadow += current - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	
	shadow/=9.0;
	
	if(projCoords.z > 1.0)
	{
		shadow = 0.0;
	}
	
	return shadow;
}

float CalcOmniShadowFactor(vec3 lightPos, uint shadowIndex, float bias)
{
	vec3 fragToLight = FragPos - lightPos;
	float current = length(fragToLight);
	
	float shadow = 0.0;
	int samples = 20;
	
	float viewDistance = length(eyePosition - FragPos);
	float diskRadius = (1.0 + (viewDistance/omniShadowMaps[shadowIndex].farPlane))/25.0;
	
	for(int i = 0; i< samples; i++)
	{
		float closest = texture(omniShadowMaps[shadowIndex].shadowMap, fragToLight + sampleOffsetDirections[i]*diskRadius).r;
		closest *= omniShadowMaps[shadowIndex].farPlane;
		if(current - bias > closest)
		{
			shadow +=1.0;
		}
	}
	
	shadow/= float(samples);

	return shadow;
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness*roughness;
	float a2 = a*a;
	float NdotH = max(dot(N,H), 0.0);  //0.0
	float NdotH2 = NdotH*NdotH;
	
	float nom = a2;
	float denom = (NdotH2*(a2-1.0)+1.0);
	denom = PI*denom*denom;
	
	return nom/denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness +1.0);
	float k = (r*r)/8.0;
	
	float nom = NdotV;
	float denom = NdotV* (1.0-k)+k;
	
	return nom/denom;
}

float GeometrySmith(vec3 N,vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V),0.0);    //0.0
	float NdotL = max(dot(N, L), 0.0);   //0.0
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);
	
	return ggx1*ggx2;
} 

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return max(F0+(1.0-F0)*pow(1.0-min(cosTheta,1.0), 5.0),0.0);
	//return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}


vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
	return max(F0+(max(vec3(1.0-roughness),F0)-F0)*pow(1.0-min(cosTheta,1.0), 5.0),0.0);
	//return F0+(max(vec3(1.0-roughness),F0)-F0)*pow(1.0-min(cosTheta,1.0), 5.0);
}


vec4 CalcLightByDirection(Light light, vec3 direction, float shadowFactor, bool is_DirectionLight)
{
	float distance = 0.0f;

	distance= length(direction);
	vec3 L = normalize(direction);

	vec3 H = normalize(V+L);
	float attenuation = 1.0/(distance*distance);
	vec3 radiance = light.color*attenuation;
	
	float NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(N, V, L, roughness);
	vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);    //0.0
	
	vec3 nominator = NDF*G*F;
	float denominator = 4* max(dot(N,V),0.0)*max(dot(N,L), 0.0);
	vec3 specular = nominator/max(denominator,0.5);    //0.001-had banding issues 
	
	vec3 kS = F;
	vec3 kD = vec3(1.0)-kS;
	kD*=1.0-metallic;
	
	float NdotL = max(dot(N, L),0.0);
	
	if(is_DirectionLight) radiance = light.color;
	vec3 Lo =(kD*albedo/PI+specular)*radiance* NdotL;
	
	return vec4((1.0-shadowFactor)*Lo, 1.0);
}

vec4 CalcLightByDirectionAndRange(vec3 lightColor, vec3 direction, float shadowFactor, float radius)
{
	float distance = 0.0f;

	distance= length(direction);
	vec3 L = normalize(direction);

	vec3 H = normalize(V+L);
	float attenuation = pow(clamp(1 - pow((distance / radius), 4.0), 0.0, 1.0), 2.0)/(1.0  + (distance * distance) );
	vec3 radiance = lightColor * attenuation;
	
	float NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(N, V, L, roughness);
	vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);    //0.0
	
	vec3 nominator = NDF*G*F;
	float denominator = 4* max(dot(N,V),0.0)*max(dot(N,L), 0.0);
	vec3 specular = nominator/max(denominator,0.5);    //0.001-has banding issues 
	
	vec3 kS = F;
	vec3 kD = vec3(1.0)-kS;
	kD*=1.0-metallic;
	
	float NdotL = max(dot(N, L),0.0);

	vec3 Lo =(kD*albedo/PI+specular)*radiance* NdotL;
	
	return vec4((1.0-shadowFactor)*Lo, 1.0);
	
}

vec4 CalcDirectionalLight()
{
	float shadowFactor = 0.0;

	for (int i = 0 ; i < NUM_CASCADES ; i++) {
     if (ClipSpacePosZ<= CascadeEndClipSpace[i]) 
	 {
	 //int i = 0;
        shadowFactor = CalcDirectionalShadowFactor(i, directionalLight, DirectionalLightSpacePos[i]);
		  if (i == 0) 
                CascadeIndicator = vec4(0.1, 0.0, 0.0, 0.0);
            else if (i == 1)
                CascadeIndicator = vec4(0.0, 0.1, 0.0, 0.0);
            else if (i == 2)
                CascadeIndicator = vec4(0.0, 0.0, 0.1, 0.0);
       break;
     }
	}	
	return CalcLightByDirection(directionalLight.base, directionalLight.direction, shadowFactor, true);
}

vec4 CalcPointLight(PointLight pLight, uint shadowIndex)
{
	vec3 direction = FragPos - pLight.position.xyz;
	float distance = length(direction);
	direction = normalize(direction);
	
	vec3 normal = normalize(NewNormal);
	float bias = max(0.5 * (1 - dot(normal, direction)), 0.1);
	
	float shadowFactor = CalcOmniShadowFactor(pLight.position.xyz, shadowIndex, bias);		
	vec4 color = CalcLightByDirectionAndRange(pLight.color.rgb, direction, shadowFactor, pLight.range);	
	return color;
}

vec4 CalcSpotLight(SpotLight sLight, uint shadowIndex)
{
	vec3 rayDirection = normalize(FragPos - sLight.position);
	float slFactor = dot(rayDirection, sLight.direction);

	if(slFactor > sLight.edge)
	{
		vec3 direction = FragPos - sLight.position;
		direction = normalize(direction);
		
		vec3 normal = normalize(NewNormal);
		float bias = max(0.5 * (1 - dot(normal, direction)), 0.1);
		
		float shadowFactor = CalcOmniShadowFactor(sLight.position, shadowIndex, bias);
		vec4 color = CalcLightByDirection(sLight.base, direction, shadowFactor, false);	
		return color * (1.0f - (1.0f- slFactor)*(1.0f/(1.0f -sLight.edge)));
	}else{
		return vec4(0, 0, 0, 0);
	}
}

float linearDepth(float depthSample){
    float depthRange = 2.0 * depthSample - 1.0;
    // Near... Far... wherever you are...
    float linear = 2.0 * camNearZ * camFarZ / (camFarZ + camNearZ - depthRange * (camFarZ - camNearZ));
    return linear;
}

vec3 colors[8] = vec3[](
   vec3(0, 0, 0),    vec3( 0,  0,  1), vec3( 0, 1, 0),  vec3(0, 1,  1),
   vec3(1,  0,  0),  vec3( 1,  0,  1), vec3( 1, 1, 0),  vec3(1, 1, 1)
);

vec4 CalcPointLights()
{
	PointLightCount = 2;

	vec4 totalColor = vec4(0, 0, 0, 1);		//set alpha to 1 when using blending
	
	//Locating which cluster you are a part of
    uint zTile     = uint(max(log2(linearDepth(gl_FragCoord.z)) * scale + bias, 0.0));
    uvec3 tiles    = uvec3( uvec2( gl_FragCoord.xy / tileSizes[3] ), zTile);
    uint tileIndex = tiles.x +
                     tileSizes.x * tiles.y +
                     (tileSizes.x * tileSizes.y) * tiles.z;  
	
    uint lightCount       = lightGrid[tileIndex].count;
    uint lightIndexOffset = lightGrid[tileIndex].offset;

    //Reading from the global light list and calculating the radiance contribution of each light.
    for(int i = 0; i < lightCount; i++){
        uint lightVectorIndex = globalLightIndexList[lightIndexOffset + i];
        totalColor += CalcPointLight(pointLight[lightVectorIndex], lightVectorIndex);
    }
	
	if(showLightSlices)
	{
		float zTile_f = float(zTile);
		totalColor = vec4(colors[uint(mod(zTile_f, 8.0))], 1.0);
	}
	
	return totalColor;
}

vec4 CalcSpotLights()
{
	
	vec4 totalColor = vec4(0, 0, 0, 1); //set alpha to 1 when using blending
	for(int i = 0; i < SpotLightCount; i++)
	{
		totalColor += CalcSpotLight(spotLights[i], i + PointLightCount);
	}
	
	return totalColor;
}

vec3 CalcMultipleTexture(sampler2DArray mat, vec2 texCoord, vec3 color, float amnt)
{
	vec3 bgrTextureColor = texture(mat, vec3(texCoord,0)).rgb * amnt; 
	vec3 rTextureColor = texture(mat, vec3(texCoord,1)).rgb * color.r;
	vec3 gTextureColor = texture(mat, vec3(texCoord,2)).rgb * color.g;
	vec3 bTextureColor = texture(mat, vec3(texCoord,3)).rgb * color.b;
	return bgrTextureColor+rTextureColor+gTextureColor+bTextureColor;
}	

vec3 CalcBumpedNormal(vec3 Color, float Amnt)
{
    vec3 BumpMapNormal = CalcMultipleTexture(material.normalMap, NewTexCoord, Color, Amnt);
    BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);
    vec3 newNormal;
    newNormal = TBN*BumpMapNormal;
    newNormal = normalize(newNormal);
    return newNormal;
}

vec2 CalcParallaxMapping(vec3 viewDir, vec2 texCoord,vec3 Color, float Amnt)
{
	// number of depth layers
    const float numLayers = 10;
    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy* -height_scale; 
    vec2 deltaTexCoords = P / numLayers;
	
	// get initial values
	vec2  currentTexCoords     = texCoord;
	float currentDepthMapValue = CalcMultipleTexture(material.parallaxMap,currentTexCoords, Color, Amnt).r;
	  
	while(currentLayerDepth < currentDepthMapValue)
	{
		// shift texture coordinates along direction of P
		currentTexCoords -= deltaTexCoords;
		// get depthmap value at current texture coordinates
		currentDepthMapValue = CalcMultipleTexture(material.parallaxMap,currentTexCoords, Color, Amnt).r;  
		// get depth of next layer
		currentLayerDepth += layerDepth;  
	}

	// get texture coordinates before collision (reverse operations)
	vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

	// get depth after and before collision for linear interpolation
	float afterDepth  = currentDepthMapValue - currentLayerDepth;
	float beforeDepth = CalcMultipleTexture(material.parallaxMap, prevTexCoords, Color, Amnt).r - currentLayerDepth + layerDepth;

	// interpolation of texture coordinates
	float weight = afterDepth / (afterDepth - beforeDepth);
	vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

	return finalTexCoords;  
	
}
	
void main()												
{
	vec3 blendMapColor = texture(blendMap, TexCoordTerr).rgb;
	float bgrTxtreAmnt = 1-(blendMapColor.r+blendMapColor.g+blendMapColor.b);

	vec3 viewDir = normalize(transpose(TBN)*(eyePosition-FragPos));
	if(height_scale==0.0f) NewTexCoord = TexCoord;
	else NewTexCoord = CalcParallaxMapping(viewDir, TexCoord, blendMapColor, bgrTxtreAmnt);

	NewNormal = CalcBumpedNormal(blendMapColor, bgrTxtreAmnt);
	
	albedo = CalcMultipleTexture(material.albedoMap,NewTexCoord, blendMapColor, bgrTxtreAmnt);
	metallic =  CalcMultipleTexture(material.metallicMap,NewTexCoord, blendMapColor, bgrTxtreAmnt).r;
	roughness =  CalcMultipleTexture(material.roughnessMap,NewTexCoord, blendMapColor, bgrTxtreAmnt).r;
	N = NewNormal;
	V = normalize(FragPos-eyePosition);
	vec3 R = reflect(V,N);
	F0 = mix(F0, albedo, metallic);
	
	vec4 finalColor = CalcDirectionalLight();
	finalColor += CalcPointLights();
	finalColor += CalcSpotLights();
	
	 // ambient lighting (we now use IBL as the ambient term)
	vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
	
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;	  
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse      = irradiance * albedo;
	
	//sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular par.
	vec3 prefilteredColor = textureLod(prefilterMap, R, roughness*MAX_REFLECTION_LOD).rgb;  
	vec2 brdf = texture(brdfLUT, vec2(max(dot(N,V),0.0),roughness)).rg;
	vec3 specular = prefilteredColor*(F*brdf.x+brdf.y); //multiplying by irradiance fixes all problems
	
    vec3 ambient = (kD*diffuse+specular)*texture(AOMap,CalcScreenTexCoord()).r;

	vec4 texColor = vec4(ambient,1.0)+finalColor;
	
	if(texColor.a > 1.0f)
	{
        //discard;
		texColor.a = 1.0f;
	}	

	color = texColor/*+CascadeIndicator*/;

	float brightness = dot(texColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if(brightness>1.0f)
	{
		BrightColor = vec4(texColor.rgb,1.0f);
	}
	else
	{
		BrightColor = vec4(0.0, 0.0,0.0, 1.0);
	}
	
	//motion vector
	vec3 NDCPos = (ClipSpacePos.xyz / ClipSpacePos.w)* 0.5 + 0.5;
    vec3 PrevNDCPos = (PrevClipSpacePos.xyz / PrevClipSpacePos.w)* 0.5 + 0.5;
    MotionVector = vec4(NDCPos - PrevNDCPos, 1.0f);
};
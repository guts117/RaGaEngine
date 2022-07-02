#version 460                                                                     

in vec2 TexCoord;

out vec4 FragColor;		

layout(early_fragment_tests) in;

uniform sampler2D theTexture;
uniform sampler2D noise;
uniform float SampleRad;
uniform mat4 Projection;

const int MAX_KERNEL_SIZE = 64;
uniform vec3 Kernel[MAX_KERNEL_SIZE];

layout (std430, binding = 2) readonly buffer screenToView{
    mat4 inverseProjection;
    uvec4 tileSizes;
	uvec2 tileSizeInPixel;
    uvec2 screenDimensions;
};

const float bias = 0.025;

vec3 PositionFromDepth(vec2 Coords)
{
	float depth = texture(theTexture, Coords).r;
	vec3 ndc = vec3(Coords, depth) * 2.0 - vec3(1.0);
	vec4 view = inverseProjection * vec4(ndc, 1.0);
	return view.xyz / view.w;
}

vec3 NormalFromDepth(vec2 Coords) 
{ 
  const vec2 offset1 = vec2(0.0,0.001);
  const vec2 offset2 = vec2(0.001,0.0);
  
  float depth = texture(theTexture, Coords).r;
  float depth1 = texture(theTexture, Coords + offset1).r;
  float depth2 = texture(theTexture, Coords + offset2).r;
  
  vec3 p1 = vec3(offset1, depth1 - depth);
  vec3 p2 = vec3(offset2, depth2 - depth);
  
  vec3 normal = cross(p1, p2);
  
  return normalize(normal);
}

void main()
{ 
	vec3 Pos = PositionFromDepth(TexCoord);	
	vec3 Normal = NormalFromDepth(TexCoord);
	// tile noise texture over screen based on screen dimensions divided by noise size
	vec2 noiseScale = vec2(screenDimensions)/4.0;
    vec3 randomVec = normalize(texture(noise, TexCoord * noiseScale).xyz);
	
    // create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(randomVec - Normal * dot(randomVec, Normal));
    vec3 bitangent = cross(tangent, Normal);
    mat3 TBN = mat3(tangent, bitangent, Normal);

	// iterate over the sample kernel and calculate occlusion factor
    float AO = 0.0;
    for (int i = 0 ; i < MAX_KERNEL_SIZE ; i++) 
	{
	    // get sample position
        vec3 samplePos = TBN * Kernel[i];
		samplePos = Pos+samplePos*SampleRad;
		
		// project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(samplePos, 1.0);
        offset = Projection * offset;
        offset.xyz /= offset.w;
        offset.xy = offset.xy * 0.5 + vec2(0.5);

        float sampleDepth= (PositionFromDepth(offset.xy)).z;

        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, SampleRad / abs(Pos.z - sampleDepth));
        AO += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;           
    }

    AO = 1.0 - AO/MAX_KERNEL_SIZE;

    FragColor = vec4(pow(AO,1.0));
	
	//FragColor = texture(noise, TexCoord*noiseScale);
	
	
}
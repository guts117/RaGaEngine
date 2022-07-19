#version 460 core

layout(early_fragment_tests) in;

#define STEPS 500
#define MAX_DIST 500
#define STEP_SIZE 0.005

// Interpolated values from the vertex shaders
in vec3 VertexPos;
in vec3 LocalEyePos;

layout(rgba16f, binding = 0) uniform image3D theTexture;

uniform vec3 box_size;

// Ouput data
layout(location = 0)out vec4 color;
layout(location = 1)out vec4 BrightColor;
layout(location = 2)out vec4 MotionVector;	

bool inside_cube(vec3 pos)
{
    vec3 q = box_size / 2;
    vec3 diff = abs(pos) - q;
    float dist = length(max(diff, vec3(0, 0, 0)));
    return dist <= 0.001;
}

ivec3 local_to_cube(vec3 pos)
{
    vec3 h = box_size / 2;
    vec3 diff = pos;
    return ivec3(diff * h + h);
}

vec4 ray_march(vec3 pos, vec3 dir)
{
    /*vec4 colour = vec4(0, 0, 0, 0);

    for (int i = 0; i < STEPS; i++)
    {
        if (!inside_cube(pos))
        {
            break;
        }

        vec4 value = imageLoad(theTexture, local_to_cube(pos));	
        colour = mix(value, colour, colour.a);     
        pos += dir * STEP_SIZE;
    }

    return colour;*/
	
	float alpha = 1.0;
    vec3 colour = vec3(0, 0, 0);

    for (int i = 0; i < STEPS; i++)
    {
        if (!inside_cube(pos))
        {
            //alpha *= (1 - bg_colour.a);
            colour += vec3(0, 0, 0) * alpha;
            break;
        }

        vec4 value = imageLoad(theTexture, local_to_cube(pos));

        alpha *= (1 - value.a);
        colour += value.rgb * alpha;
        
        pos += dir * STEP_SIZE;
    }

    return vec4(colour, alpha);
}

void main()
{
    vec3 ray_pos = VertexPos;
    vec3 ray_dir = ray_pos - LocalEyePos;

    vec4 ray_colour = ray_march(ray_pos, ray_dir);
    color = ray_colour;
	BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
	MotionVector = vec4(0.0, 0.0, 0.0, 1.0);
}
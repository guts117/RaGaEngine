#version 460 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 pos;
layout(location = 1) in vec4 pos_size; // Position of the center of the particule and size of the square
layout(location = 2) in vec4 color; // Position of the center of the particule and size of the square

// Output data ; will be interpolated for each fragment.
out vec2 TexCoord;
out vec4 particlecolor;

// Values that stay constant for the whole mesh.
uniform vec3 CameraRight_worldspace;
uniform vec3 CameraUp_worldspace;
uniform mat4 Projection; // Model-View-Projection matrix, but without the Model (the position is in BillboardPos; the orientation depends on the camera)
uniform mat4 View; // Model-View-Projection matrix, but without the Model (the position is in BillboardPos; the orientation depends on the camera)


void main()
{
	float particleSize = pos_size.w; // because we encoded it this way.
	vec3 particleCenter_wordspace = pos_size.xyz;
	
	vec3 vertexPosition_worldspace = 
		particleCenter_wordspace
		+ CameraRight_worldspace * pos.x * particleSize
		+ CameraUp_worldspace * pos.y * particleSize;

	// Output position of the vertex
	gl_Position = Projection * View * vec4(vertexPosition_worldspace, 1.0f);

	// UV of the vertex. No special space for this one.
	TexCoord = -pos.xy + vec2(0.5, 0.5);
	particlecolor = color;
}

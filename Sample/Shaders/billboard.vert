#version 460 core

		// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 pos;

		// Output data ; will be interpolated for each fragment.
out vec2 TexCoord;
out vec4 ClipSpacePos;
out vec4 PrevClipSpacePos;

		// Values that stay constant for the whole mesh.
uniform vec3 CameraRight_worldspace;
uniform vec3 CameraUp_worldspace;
uniform mat4 View; // Model-View-Projection matrix, but without the Model (the position is in BillboardPos; the orientation depends on the camera)
uniform mat4 Projection;
uniform vec3 BillboardPos; // Position of the center of the billboard
uniform vec2 BillboardSize; // Size of the billboard, in world units (probably meters)

uniform mat4 prevPV;	

void main()
{

	vec3 up = vec3(0.0f, 1.0f, 0.0f);  // use up and right for faraway objects and foliage
	vec3 right = vec3(1.0f, 0.0f, 0.0f);
	vec3 particleCenter_wordspace = BillboardPos;
	vec3 vertexPosition_worldspace = 
		particleCenter_wordspace
		+ CameraRight_worldspace * pos.x * BillboardSize.x
		+ up * pos.y * BillboardSize.y;


			// Output position of the vertex
	vec4 ClipSpacePosition = Projection*View* vec4(vertexPosition_worldspace, 1.0f);
	gl_Position = ClipSpacePosition;


			// Or, if BillboardSize is in percentage of the screen size (1,1 for fullscreen) :
			//vertexPosition_worldspace = particleCenter_wordspace;
			//gl_Position = VP * vec4(vertexPosition_worldspace, 1.0f); // Get the screen-space position of the particle's center
			//gl_Position /= gl_Position.w; // Here we have to do the perspective division ourselves.
			//gl_Position.xy += squareVertices.xy * vec2(0.2, 0.05); // Move the vertex in directly screen space. No need for CameraUp/Right_worlspace here.
			
			// Or, if BillboardSize is in pixels : 
			// Same thing, just use (ScreenSizeInPixels / BillboardSizeInPixels) instead of BillboardSizeInScreenPercentage.


			// UV of the vertex. No special space for this one.
	TexCoord = -pos.xy + vec2(0.5, 0.5);
	
    ClipSpacePos = ClipSpacePosition;
    PrevClipSpacePos =  prevPV* vec4(vertexPosition_worldspace, 1.0f);
}
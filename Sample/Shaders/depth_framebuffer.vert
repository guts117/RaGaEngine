#version 460                                                                       
                                                                                    
layout (location = 0) in vec3 pos;                                             

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;

void main()
{       
    gl_Position = Projection * View * Model* vec4(pos, 1.0);
}
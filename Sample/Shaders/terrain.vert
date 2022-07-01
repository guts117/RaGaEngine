#version 460                                                                          
                                                                                                
layout (location = 0) in vec3 pos;                                                   
layout (location = 1) in vec2 tex;                                                   
layout (location = 2) in vec3 norm;   
layout (location = 3) in vec3 tangent;                                                  
                                                                                                
uniform mat4 Model;                                                                         
                                                                                                
out vec3 WorldPos_CS_in;                                                                    
out vec2 TexCoord_CS_in;                                                                        
out vec3 Normal_CS_in;     
out vec3 Tangent_CS_in;                                                                
	   
void main()                                                                                     
{                                                                                               
    WorldPos_CS_in = (Model * vec4(pos, 1.0)).xyz;   	
    TexCoord_CS_in = tex;                                                            
    Normal_CS_in   = mat3(Model)*norm;
	Tangent_CS_in = mat3(Model)*tangent;
}
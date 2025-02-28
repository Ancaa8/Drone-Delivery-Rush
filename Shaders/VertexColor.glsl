#version 330 core

layout(location = 0) in vec3 in_position; 
layout(location = 1) in vec3 in_color;  

out vec3 frag_color; 

uniform mat4 view;     
uniform mat4 projection; 
uniform mat4 model;    

float noise(vec2 pos) {
    return fract(sin(dot(pos, vec2(12.9898, 78.233))) * 43758.5453123);
}

void main() {
    vec2 posXZ = in_position.xz;        
    float frequency = 0.05;              
    float amplitude = 0.5;          
    float height = noise(posXZ * frequency) * amplitude;

    
    vec3 updated_position = vec3(in_position.x, height, in_position.z);

    
    vec3 color1 = vec3(0.2, 0.6, 0.3); 
    vec3 color2 = vec3(0.8, 0.8, 0.5); 
    frag_color = mix(color1, color2, height / amplitude);

    gl_Position = projection * view * model * vec4(updated_position, 1.0);
}
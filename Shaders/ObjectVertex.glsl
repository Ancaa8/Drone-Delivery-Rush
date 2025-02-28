#version 330 core

layout(location = 0) in vec3 in_position;

out vec4 frag_color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform int objectID;

void main() {
    if (objectID == 1) {
        frag_color = vec4(0.5, 0.5, 0.5, 1.0);
    } else if (objectID == 2) {
        frag_color = vec4(0.0, 0.0, 0.0, 1.0);
    } else if (objectID == 3) {
        frag_color = vec4(0.5, 0.5, 0.5, 1.0);
    } else if (objectID == 4) {
        frag_color = vec4(0.8, 0.2, 0.2, 1.0);
    } else if (objectID == 5) {
        frag_color = vec4(0.55, 0.27, 0.07, 1.0);
    } else if (objectID == 6) {
        frag_color = vec4(0.0, 0.5, 0.0, 1.0);
    } else if (objectID == 7) {
        frag_color = vec4(1.0, 1.0, 0.0, 1.0);
    } else if (objectID == 8) {
        frag_color = vec4(1.0, 0.0, 1.0, 0.5);
    } else if (objectID == 9) {
        frag_color = vec4(1.0, 0.0, 1.0, 1.0); 
    }else if (objectID == 10) {
        frag_color = vec4(0.0, 0.0, 1.0, 1.0); 
    } else {
        frag_color = vec4(1.0, 1.0, 1.0, 1.0);
    }

    gl_Position = projection * view * model * vec4(in_position, 1.0);
}

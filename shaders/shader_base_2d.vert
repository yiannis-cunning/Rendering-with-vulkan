#version 450

layout(location = 0) out vec3 fragColor;    // Declare output var 0.

layout(location = 0) in vec3 pos;   // x cord, y cord, depth
layout(location = 1) in vec3 color; /* Type = input/output, */


layout(binding = 0) uniform UniformBufferObject {       // Do some translations if you want -  2d virtual space
    mat4 translation;
} ubo;  /* type = unifrom buffer, same for all vertexes*/



void main() {
    fragColor = color;
    gl_Position =  vec4(pos, 1.0); // ubo.translation * vec4(pos, 1.0);
}

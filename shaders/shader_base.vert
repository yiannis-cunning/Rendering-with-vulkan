#version 450

layout(location = 0) out vec3 fragColor;    // Declare output var 0.

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 color; /* Type = input/output, */


layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;  /* type = unifrom buffer, same for all vertexes*/


/*
layout(binding = 2, std140 (alignment) ) buffer somename{

    structname_array []
}
type = buffer, or SSBO - shared access to a buffer
- specify the alignment with std140

*/

void main() {
    //gl_Position = vec4(pos, -1.0, -1.0); // Specify the output position (vec4 in clip space )
    fragColor = color;
    gl_Position =  ubo.proj * ubo.view * ubo.model * vec4(pos, 1.0);
}

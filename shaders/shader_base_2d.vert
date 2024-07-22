#version 450

layout(location = 0) out vec3 fragColor;    // Declare output var 0.
layout(location = 1) out vec2 fragTexCoord;    // Declare output var 1.

layout(location = 0) in vec3 pos;   // x cord, y cord, depth
layout(location = 1) in vec3 color; /* Type = input/output, */
layout(location = 2) in vec2 texCord;

layout(binding = 0) uniform UniformBufferObject {       // Do some translations if you want -  2d virtual space
    mat4 translation;
} ubo;  /* type = unifrom buffer, same for all vertexes*/

layout(binding = 1) uniform push_cnst {
    vec2 sqidx;
} pc;


void main() {
    fragColor = color;
    fragTexCoord = texCord;
    //fragTexCoord = texCord + pc.sqidx;
    gl_Position =  ubo.translation * vec4(pos, 1.0);
}

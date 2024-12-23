#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inColor;

layout(set = 0, binding = 0) uniform UniformBufferObject 
{
    mat4 viewProj;
    vec3 viewPos;
} ubo;

layout (location = 0) out vec3 fragColor;

void main() 
{
    fragColor = inColor;
	gl_Position = ubo.viewProj * vec4(inPos, 1.f);
}

#version 450

layout (location = 0) in vec3 inPos;

layout(set = 0, binding = 0) uniform UniformBufferObject 
{
    mat4 viewProj;
    vec3 viewPos;
} ubo;

layout (location = 0) out vec3 outUVW;

void main() 
{
	outUVW = inPos;
	outUVW.xy *= -1.0;
	gl_Position = (mat4(mat3(ubo.viewProj)) * vec4(inPos, 1.0)).xyww;
}

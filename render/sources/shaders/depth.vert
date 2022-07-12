#version 450

layout(set = 0, binding = 0) uniform UniformBufferObject 
{
    mat4 viewProj;
    vec4 viewPos;
} ubo;

layout(push_constant) uniform constants 
{
    mat4 model;
} pushConstants;

layout (location = 0) in vec3 inPos;

out gl_PerVertex 
{
    vec4 gl_Position;   
};

 
void main()
{
	gl_Position =  ubo.viewProj * pushConstants.model * vec4(inPos, 1.0);
}
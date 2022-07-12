#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform UniformBufferObject 
{
    mat4 viewProj;
    vec4 viewPos;
} ubo;

layout(set = 1, binding = 0) uniform DirectionalUBO
{
    mat4 lightSpace;
	vec4 lightPos;
} directionalUbo;

layout(push_constant) uniform constants 
{
    mat4 model;
} pushConstants;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec3 fragPosition;
layout(location = 2) out vec2 fragTexCoord;
layout(location = 3) out vec3 viewPos;
layout(location = 4) out vec3 outLightVec;
layout(location = 5) out vec4 outShadowCoord;

out gl_PerVertex 
{
    vec4 gl_Position;   
};

const mat4 biasMat = mat4( 
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0 );

void main() 
{
    fragPosition = vec3(pushConstants.model * vec4(inPosition, 1.f));
    fragNormal = mat3(transpose(inverse(pushConstants.model))) * inNormal;
    fragTexCoord = inTexCoord;
    viewPos = ubo.viewPos.xyz;

    gl_Position = ubo.viewProj * vec4(fragPosition, 1.f);

    outLightVec = normalize(directionalUbo.lightPos.xyz - inPosition);
    outShadowCoord = (biasMat * directionalUbo.lightSpace) * vec4(fragPosition, 1.f);
}
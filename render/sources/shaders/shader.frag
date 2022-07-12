#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec3 fragPosition;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in vec3 viewPos;
layout(location = 4) in vec3 inLightVec;
layout(location = 5) in vec4 inShadowCoord;


#define constAmbient 0.1f;

// Material
layout(set = 2, binding = 0) uniform sampler2D ambientSampler;
layout(set = 2, binding = 1) uniform sampler2D diffuseSampler;
layout(set = 2, binding = 2) uniform sampler2D specularSampler;
layout(set = 2, binding = 3) uniform sampler2D alphaSampler;
layout(set = 2, binding = 4) uniform Material
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;

	float alpha;
	float shininess;

	bool ambientPresent;
	bool diffusePresent;
	bool specularPresent;
	bool alphaPresent;
} material;

// Directional Light
struct DirectionalLight
{
	vec4 direction;

	vec4 ambient;
	vec4 diffuse;
	vec4 specular;

	bool isActive;
};

layout(set = 1, binding = 1) uniform DirLight 
{
	DirectionalLight light;
} dirLight;

layout(set = 1, binding = 2) uniform sampler2D shadowMap;

float GetAlpha()
{
	if(material.alphaPresent)
		return texture(alphaSampler, fragTexCoord).a;

	return material.alpha;
}

vec3 GetAmbientObjColor()
{
	if(material.ambientPresent)
		return vec3(texture(ambientSampler, fragTexCoord));
	else
		if(material.diffusePresent)
			return vec3(texture(diffuseSampler, fragTexCoord));

	return material.ambient.xyz;
}

vec3 GetDiffuseObjColor()
{
	//todo handle albedo multiply

	if(material.diffusePresent)
		return vec3(texture(diffuseSampler, fragTexCoord));

	return material.diffuse.xyz;
}

vec3 GetSpecularObjColor()
{
	if(material.specularPresent)
		return vec3(texture(specularSampler, fragTexCoord));

	return material.specular.xyz;
}


vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
	if(!light.isActive)
		return vec3(0.f);

	vec3 lightDir = normalize(-light.direction.xyz);

	float diff = max(dot(normal, lightDir), 0.f);

	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.f), material.shininess);

	vec3 ambient = light.ambient.xyz * GetAmbientObjColor();
	vec3 diffuse = light.diffuse.xyz * diff * GetDiffuseObjColor();
	vec3 specular = light.specular.xyz * spec * GetSpecularObjColor();

	return (ambient + diffuse + specular);
}

float textureProj(vec4 shadowCoord, vec2 off)
{
	float shadow = 1.f;
	if ( shadowCoord.z > -1.0 && shadowCoord.z < 1.0 ) 
	{
		float dist = texture( shadowMap, shadowCoord.st + off ).r;
		if (shadowCoord.w > 0.f && dist < shadowCoord.z) 
		{
			shadow = constAmbient;
		}
	}

	return shadow;
}

float filterPCF(vec4 sc)
{
	ivec2 texDim = textureSize(shadowMap, 0);
	float scale = 1.5;
	float dx = scale * 1.0 / float(texDim.x);
	float dy = scale * 1.0 / float(texDim.y);

	float shadowFactor = 0.0;
	int count = 0;
	int range = 1;
	
	for (int x = -range; x <= range; x++)
	{
		for (int y = -range; y <= range; y++)
		{
			shadowFactor += textureProj(sc, vec2(dx*x, dy*y));
			count++;
		}
	
	}
	return shadowFactor / count;
}



void main()
{
	vec3 norm = normalize(fragNormal);
	vec3 viewDir = normalize(viewPos - fragPosition);

	float shadow = filterPCF(inShadowCoord / inShadowCoord.w);
	vec3 result = vec3(0.f);

	result += CalcDirLight(dirLight.light, norm, viewDir);
	
	result *= shadow;

	outColor = vec4(result, GetAlpha());
}
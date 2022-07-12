#include "LightComponent.h"

#include "render/VulkanRenderer/VulkanRenderer.h"
#include "core/CLog.h"

namespace Render
{
	void DirectionalLightComponent::Initialize(const void* params)
	{
		if (params != nullptr)
		{
			const LightComponentInitParams* initParams = static_cast<const LightComponentInitParams*>(params);

			ambientComponent = {
				initParams->color.x,
				initParams->color.y,
				initParams->color.z,
				initParams->ambientWeight
			};
			diffuseComponent = {
				initParams->color.x,
				initParams->color.y,
				initParams->color.z,
				initParams->diffuseWeight
			};
			specularComponent = {
				initParams->color.x,
				initParams->color.y,
				initParams->color.z,
				initParams->specularWeight
			};
		}
		else
		{
			LOG(LOG_ERROR, "Light component initializer was not set in AddComponent().", Core::ELogChannel::CLOG_RENDER);
		}
	}

	void DirectionalLightComponent::Constructor()
	{
		VulkanRenderer::UpdateLightBuffers();
	}

	void PointLightComponent::Initialize(const void* params)
	{
		constant = 0.f;
		linear = 0.f;
		quadratic = 0.f;

		if (params != nullptr)
		{
			const LightComponentInitParams* initParams = static_cast<const LightComponentInitParams*>(params);

			ambientComponent = {
				initParams->color.x,
				initParams->color.y,
				initParams->color.z,
				initParams->ambientWeight
			};
			diffuseComponent = {
				initParams->color.x,
				initParams->color.y,
				initParams->color.z,
				initParams->diffuseWeight
			};
			specularComponent = {
				initParams->color.x,
				initParams->color.y,
				initParams->color.z,
				initParams->specularWeight
			};
		}
		else
		{
			LOG(LOG_ERROR, "Light component initializer was not set in AddComponent().", Core::ELogChannel::CLOG_RENDER);
		}
	}

	void PointLightComponent::Constructor()
	{
		VulkanRenderer::UpdateLightBuffers();
	}

	void SpotLightComponent::Initialize(const void* params)
	{
		constant = 0.f;
		linear = 0.f;
		quadratic = 0.f;
		cutOff = 0.f;
		outerCutOff = 0.f;

		if (params != nullptr)
		{
			const LightComponentInitParams* initParams = static_cast<const LightComponentInitParams*>(params);

			ambientComponent = {
				initParams->color.x,
				initParams->color.y,
				initParams->color.z,
				initParams->ambientWeight
			};
			diffuseComponent = {
				initParams->color.x,
				initParams->color.y,
				initParams->color.z,
				initParams->diffuseWeight
			};
			specularComponent = {
				initParams->color.x,
				initParams->color.y,
				initParams->color.z,
				initParams->specularWeight
			};
		}
		else
		{
			LOG(LOG_ERROR, "Light component initializer was not set in AddComponent().", Core::ELogChannel::CLOG_RENDER);
		}
	}

	void SpotLightComponent::Constructor()
	{
		VulkanRenderer::UpdateLightBuffers();
	}
}

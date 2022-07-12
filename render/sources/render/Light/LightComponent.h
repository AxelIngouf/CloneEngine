#pragma once

#include "core/ECS/Component.h"
#include "core/Delegate.h"
#include "Vector/Vector3.h"
#include "Vector/Vector4.h"


namespace Render
{
	DELEGATE(DirectionalLightUpdate);

	DELEGATE(PointLightUpdate);

	DELEGATE(SpotLightUpdate);

	STRUCT(LightComponentInitParams,
		FIELD(float, ambientWeight),
		FIELD(float, diffuseWeight),
		FIELD(float, specularWeight),
		FIELD(LibMath::Vector3, color)
	);

	COMPONENT(DirectionalLightComponent,
		INIT_PARAM(LightComponentInitParams),
		FUNCTION(void, Initialize, const void*, params),
		FUNCTION(void, Constructor),
		FIELD(LibMath::Vector4, ambientComponent),
		FIELD(LibMath::Vector4, diffuseComponent),
		FIELD(LibMath::Vector4, specularComponent)
	);


	COMPONENT(PointLightComponent,
		INIT_PARAM(LightComponentInitParams),
		FUNCTION(void, Initialize, const void*, params),
		FUNCTION(void, Constructor),
		FIELD(LibMath::Vector4, ambientComponent),
		FIELD(LibMath::Vector4, diffuseComponent),
		FIELD(LibMath::Vector4, specularComponent),
		FIELD(float, constant),
		FIELD(float, linear),
		FIELD(float, quadratic)
	);

	COMPONENT(SpotLightComponent,
		INIT_PARAM(LightComponentInitParams),
		FUNCTION(void, Initialize, const void*, params),
		FUNCTION(void, Constructor),
		FIELD(LibMath::Vector4, ambientComponent),
		FIELD(LibMath::Vector4, diffuseComponent),
		FIELD(LibMath::Vector4, specularComponent),
		FIELD(float, constant),
		FIELD(float, linear),
		FIELD(float, quadratic),
		FIELD(float, cutOff),
		FIELD(float, outerCutOff)
	);
}

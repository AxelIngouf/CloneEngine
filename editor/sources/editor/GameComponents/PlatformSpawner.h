#pragma once
#include "core/ECS/System.h"
#include "core/reflection/StructMeta.h"
#include "editor/GameComponents/PlatformComponent.h"
#include "editor/GameEntities/Platform.h"
#include "Random.h"

using namespace Core;

namespace editor
{
	COMPONENT(PlatformSpawner,
		FUNCTION(void, Initialize, const void*, params),
		FUNCTION(void, Constructor),
		FUNCTION(void, Update, float, elapsedTime),
		FIELD(float, totalElapsedTime),
		FIELD(int, platformCount),
		FIELD(EPlatformDirection, previousPlatformDirection),
		FIELD(float, currentX),
		FIELD(float, currentZ),
		FIELD(bool, isNewTurn),
		SUPPLEMENT(
			EMPTY(),
			inline static LibMath::Random rand; ,
			EMPTY()
		)
	);
}

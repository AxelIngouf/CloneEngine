#pragma once
#include "core/ECS/Component.h"
#include "core/reflection/EnumMeta.h"

constexpr float PLATFORM_MINY_FALL = -250.f;

ENUM(EPlatformDirection,
    CONSTANT(FORWARD),
    CONSTANT(LEFT),
    CONSTANT(RIGHT)
);

STRUCT(PlatformComponentParams,
    FIELD(EPlatformDirection, platformDirection),
    FIELD(float, fallSecondTimer),
    SUPPLEMENT(
        EMPTY(),
        PlatformComponentParams(); ,
        EMPTY()
    )
);

COMPONENT(PlatformComponent,
    INIT_PARAM(PlatformComponentParams),
    FUNCTION(void, Initialize, const void*, params),
    FUNCTION(void, Update, float, elapsedTime),
    FUNCTION(void, Fall),
    FIELD(EPlatformDirection, platformDirection),
    FIELD(float, fallSecondTimer),
    FIELD(float, totalElapsedTime),
    FIELD(bool, isFalling),
    FIELD(int, platformId)
);

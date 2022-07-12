#pragma once
#include "core/ECS/Component.h"

STRUCT(RotationLimiterComponentParams,
    FIELD(float, pitchMinLimit),
    FIELD(float, rollMinLimit),
    FIELD(float, yawMinLimit),
    FIELD(float, pitchMaxLimit),
    FIELD(float, rollMaxLimit),
    FIELD(float, yawMaxLimit),
    SUPPLEMENT(
        EMPTY(),
        RotationLimiterComponentParams();,
        EMPTY()
    )
);

COMPONENT(RotationLimiterComponent,
    INIT_PARAM(RotationLimiterComponentParams),
    FUNCTION(void, Initialize, const void*, params),
    FUNCTION(void, Update, float, elapsedTime),
    FIELD(float, pitchMinLimit),
    FIELD(float, rollMinLimit),
    FIELD(float, yawMinLimit),
    FIELD(float, pitchMaxLimit),
    FIELD(float, rollMaxLimit),
    FIELD(float, yawMaxLimit)
);

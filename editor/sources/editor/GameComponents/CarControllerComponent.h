#pragma once
#include "core/ECS/Component.h"
#include "core/Delegate.h"

DELEGATE_Two_Params(CheckpointReached, int, checkpointId, int, totalCheckpoints);
DELEGATE_Two_Params(LapFinished, int, currentLap, int, totalLaps);

COMPONENT(CarControllerComponent,
    FUNCTION(void, BeginPlay),
    FUNCTION(int, GetControllerId),
    FUNCTION(int, GetNextCheckpointId),
    FUNCTION(void, SetNextCheckpointId, int, newCheckpointId),
    FUNCTION(int, GetCurrentLap),
    FUNCTION(void, SetCurrentLap, int, newLap),
    FUNCTION(void, CheckpointReached),
    PRIVATE_FIELD(int, nextCheckpointId),
    PRIVATE_FIELD(int, currentLap),
    PRIVATE_FIELD(int, totalLap),
    PRIVATE_FIELD(int, lastCheckpointId, Core::FieldFlag::READONLY),
    PRIVATE_FIELD(int, controllerId, Core::FieldFlag::READONLY),
    SUPPLEMENT(
        EMPTY(),
        public:
        CarControllerComponent();
        OnCheckpointReached onCheckpointReached;
        OnLapFinished onLapFinished; ,
        EMPTY()
    )
);


#pragma once
#include "core/ECS/Component.h"

namespace Physics {
    class PhysicsRigidActor;
    struct SweepHit;
}

COMPONENT(CheckpointComponent,
    FUNCTION(void, BeginPlay),
    FIELD(int, checkpointId),
    SUPPLEMENT(
        EMPTY(),
        CheckpointComponent();
        void CarOverlap(Physics::PhysicsRigidActor* thisObject, Physics::PhysicsRigidActor* otherObject, Physics::SweepHit sweepHit);
        void CheckpointReached();
        static void HighlightCheckpoint(int checkpointId); ,
        EMPTY()
    )
);
 
#pragma once
#include <PxSimulationEventCallback.h>

namespace Physics
{
    class SimulationEventCallback : public physx::PxSimulationEventCallback
    {
        void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) override { PX_UNUSED(constraints); PX_UNUSED(count); }
        void onWake(physx::PxActor** actors, physx::PxU32 count) override { PX_UNUSED(actors); PX_UNUSED(count); }
        void onSleep(physx::PxActor** actors, physx::PxU32 count) override { PX_UNUSED(actors); PX_UNUSED(count); }
        void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) override;
        void onAdvance(const physx::PxRigidBody* const* , const physx::PxTransform* , const physx::PxU32) override {}


        void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) override;
    };
}

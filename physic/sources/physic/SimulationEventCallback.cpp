#include "SimulationEventCallback.h"

#include "PhysicsInstance.h"
#include "PhysicsRigidActor.h"

using namespace physx;

namespace Physics
{
    void SimulationEventCallback::onTrigger(PxTriggerPair* pairs, PxU32 count)
    {
        for (PxU32 i = 0; i < count; i++)
        {
            if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
                continue;

            auto* firstActor = pairs[i].triggerActor;
            auto* secondActor = pairs[i].otherActor;

            auto* firstRigidBody = static_cast<PhysicsRigidActor*>(firstActor->userData);
            auto* secondRigidBody = static_cast<PhysicsRigidActor*>(secondActor->userData);

            if(firstRigidBody && secondRigidBody)
            {
                const SweepHit sweepHit{};

                firstRigidBody->onBeginOverlap.Broadcast(firstRigidBody, secondRigidBody, sweepHit);
                secondRigidBody->onBeginOverlap.Broadcast(secondRigidBody, firstRigidBody, sweepHit);
            }
        }
    }

    void SimulationEventCallback::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs,
                                            const PxU32 nbPairs)
    {
        PX_UNUSED(pairHeader);

        for (uint32_t iPair = 0; iPair < nbPairs; iPair++)
        {
            if(pairs[iPair].contactCount > 0)
            {
                auto* firstActor = pairs[iPair].shapes[0]->getActor();
                auto* secondActor = pairs[iPair].shapes[1]->getActor();

                auto* firstRigidBody = static_cast<PhysicsRigidActor*>(firstActor->userData);
                auto* secondRigidBody = static_cast<PhysicsRigidActor*>(secondActor->userData);

                if (firstRigidBody && secondRigidBody)
                {
                    if (pairs[iPair].flags.isSet(PxContactPairFlag::eACTOR_PAIR_LOST_TOUCH))
                    {
                        firstRigidBody->onEndOverlap.Broadcast(firstRigidBody, secondRigidBody);
                        secondRigidBody->onEndOverlap.Broadcast(secondRigidBody, firstRigidBody);
                    }
                    if (pairs[iPair].events.isSet(PxPairFlag::eNOTIFY_TOUCH_FOUND))
                    {
                        if (pairs[iPair].flags.isSet(PxContactPairFlag::eACTOR_PAIR_HAS_FIRST_TOUCH))
                        {
                            std::vector<PxContactPairPoint> contactPoints;

                            const PxU32 contactCount = pairs[iPair].contactCount;

                            if (contactCount > 0)
                            {
                                contactPoints.resize(contactCount);
                                pairs[iPair].extractContacts(&contactPoints[0], contactCount);

                                const SweepHit sweepHit(Vec3Convert(contactPoints[0].position),
                                    Vec3Convert(contactPoints[0].normal), contactPoints[0].separation < 0.f);

                                firstRigidBody->onBeginOverlap.Broadcast(firstRigidBody, secondRigidBody, sweepHit);
                                secondRigidBody->onBeginOverlap.Broadcast(secondRigidBody, firstRigidBody, sweepHit);

                            }
                        }
                    }
                }
            }

        }

    }
}
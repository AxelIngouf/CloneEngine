#include "PlatformSpawner.h"

#include "core/scenegraph/SceneGraph.h"
#include "editor/GameEntities/Boulder.h"

#define BOULDER_SPAWN_HEIGHT 50

using namespace editor;

void PlatformSpawner::Initialize(const void*)
{
    totalElapsedTime = PLATFORM_LIFESPAN;
    previousPlatformDirection = EPlatformDirection::FORWARD;
    currentX = 0;
    currentZ = 0;
    isNewTurn = false;
}

void PlatformSpawner::Constructor()
{
    totalElapsedTime = PLATFORM_LIFESPAN;
    currentX = 0;
    currentZ = 0;
}

void PlatformSpawner::Update(const float elapsedTime)
{
    totalElapsedTime += elapsedTime;

    while(totalElapsedTime >= PLATFORM_LIFESPAN)
    {
        totalElapsedTime -= PLATFORM_LIFESPAN;

        auto* sceneNode = World::GetLevel()->GetRoot()->CreateChild(CreatePlatformEntity);
        sceneNode->SetName("Platform_" + std::to_string(platformCount));

        const int randomInt = rand.RandomIntInRange(0, 10);
        EPlatformDirection direction;

        if(!isNewTurn && randomInt < 7)
        {
            if(previousPlatformDirection == EPlatformDirection::FORWARD)
            {
                if(randomInt == 0)
                {
                    direction = EPlatformDirection::LEFT;
                    isNewTurn = true;
                }
                else if (randomInt == 1)
                {
                    direction = EPlatformDirection::RIGHT;
                    isNewTurn = true;
                }
                else
                {
                    direction = previousPlatformDirection;
                }
            }
            else if (previousPlatformDirection == EPlatformDirection::LEFT)
            {
                direction = EPlatformDirection::FORWARD;
                isNewTurn = true;
            }
            else if(previousPlatformDirection == EPlatformDirection::RIGHT)
            {
                direction = EPlatformDirection::FORWARD;
                isNewTurn = true;
            }
        }
        else
        {
            direction = previousPlatformDirection;
            isNewTurn = false;
        }

        sceneNode->SetPosition({ currentX, 0, currentZ });

        {
            // platform component
            PlatformComponentParams initParams;
            initParams.platformDirection = direction;
            initParams.fallSecondTimer = PLATFORM_LIFESPAN * 3;
            auto* platformComponent = sceneNode->GetEntity()->AddComponent<PlatformComponent>(&initParams);
            platformComponent->platformId = platformCount;
        }



        if (rand.RandomIntInRange(0, 5) == 0)
        {
            auto* boulderNode= World::GetLevel()->GetRoot()->CreateChild(CreateBoulderEntity);

            boulderNode->SetWorldPosition({ currentX, BOULDER_SPAWN_HEIGHT, currentZ });
        }

        if (direction == EPlatformDirection::FORWARD)
        {
            currentZ += PLATFORM_XZ_SIZE;
        }
        else if (direction == EPlatformDirection::LEFT)
        {
            currentX -= PLATFORM_XZ_SIZE;
        }
        else if (direction == EPlatformDirection::RIGHT)
        {
            currentX += PLATFORM_XZ_SIZE;
        }

        previousPlatformDirection = direction;

        platformCount++;
    }
}

#include "Sound3D.h"

#include <fmod.hpp>

#include "SoundManager.h"
#include "core/ECS/Entity.h"
#include "core/scenegraph/SceneNode.h"

namespace Sound
{
    SoundComponentParams::SoundComponentParams() :
        file(File(EFileType::SOUND)),
        minDistance(1),
        maxDistance(10),
        isLoop(false)
    {
    }

    Sound3DComponent::Sound3DComponent(Sound3DComponent&& other) noexcept :
        Component<Sound3DComponent>(other),
        Sound2D(std::move(other)),
        location(other.location),
        path(other.path),
        isLoop(other.isLoop),
        minDistance(other.minDistance),
        maxDistance(other.maxDistance)
    {
        other.sound = nullptr;
        other.channel = nullptr;
    }

    void Sound3DComponent::Initialize(const void* params)
    {
        SoundComponentParams default;
        if(params == nullptr)
        {
            LOG(LOG_ERROR, "Sound component initializer was not set in AddComponent().", Core::ELogChannel::CLOG_SOUND);

            params = &default;
        }

        const SoundComponentParams* initParams = static_cast<const SoundComponentParams*>(params);

        path = initParams->file.path;
        isLoop = initParams->isLoop;
        minDistance = initParams->minDistance;
        maxDistance = initParams->maxDistance;
    }

    void Sound3DComponent::Constructor()
    {
        if (path == "")
        {
            LOG(LOG_ERROR, "Sound component file path was not set in the initializer.", Core::ELogChannel::CLOG_SOUND);

            return;
        }

        CreateSound(path, isLoop);
        SetMinMaxDistance(minDistance, maxDistance);
    }


    void Sound3DComponent::BeginPlay()
    {
        channel->setPaused(false);
    }

    void Sound3DComponent::Finalize()
    {
        Release();
    }

    void Sound3DComponent::Update(const float elapsedTime)
    {
        if(channel != nullptr)
        {
            auto* entity = Core::Entity::GetEntity(GetEntityHandle());
            if(entity)
            {
                LibMath::Vector3 entityLocation = entity->GetAnchor()->GetWorldTransformNoCheck().position;
                UpdateLocation(entityLocation, elapsedTime);
            }
        }
    }

    void Sound3DComponent::SetMinMaxDistance(const float minDist, const float maxDist)
    {
        sound->set3DMinMaxDistance(minDist, maxDist);
    }

    void Sound3DComponent::Set3DConeOrientation(LibMath::Vector3 orientation)
    {
        FMOD_VECTOR orientationVector = { orientation.x , orientation.y , orientation.z };
        channel->set3DConeOrientation(&orientationVector);
    }

    void Sound3DComponent::Set3DConeSettings(const float innerAngle, const float outerAngler, const float outerVolume)
    {
        channel->set3DConeSettings(innerAngle, outerAngler, outerVolume);
    }

    void Sound3DComponent::UpdateLocation(const LibMath::Vector3 newLocation, const float elapsedTime)
    {
        FMOD_VECTOR velocity;

        velocity.x = (newLocation.x - location.x) * elapsedTime;
        velocity.y = (newLocation.y - location.y) * elapsedTime;
        velocity.z = (newLocation.z - location.z) * elapsedTime;

        location = newLocation;

        const FMOD_VECTOR fmodLocation = { location.x , location.y , location.z };

        channel->set3DAttributes(&fmodLocation, &velocity);
    }

    void Sound3DComponent::SetLocation(const LibMath::Vector3 newLocation)
    {
        location = newLocation;
    }

    LibMath::Vector3 Sound3DComponent::GetLocation()
    {
        return location;
    }

    void Sound3DComponent::CreateSound(const std::string soundPath, const bool shoouldLoop)
    {
        path = soundPath;

        SoundManager::Load3DSound(this, soundPath, shoouldLoop);

        int channelIndex;
        channel->getIndex(&channelIndex);


        SoundManager::GetFMODSystem()->playSound(sound, nullptr, false, &channel);

        channel->setPaused(true);
        channel->setVolume(0.1f);
    }

    void Sound3DComponent::SetFMODSound(void* fmodSound)
    {
        sound = static_cast<FMOD::Sound*>(fmodSound);
    }
}

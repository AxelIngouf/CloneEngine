#include "SoundManager.h"

#include <fmod.hpp>
#include <fmod_errors.h>


#include "../../../render/sources/render/Camera/CameraComponent.h"
#include "../../../render/sources/render/Camera/FreeCam.h"
#include "core/CLog.h"
#include "core/GameLoop.h"
#include "core/PoolAllocator.h"
#include "core/ResourceManager.h"

namespace Sound
{
    SoundManager SoundManager::soundManager;

    bool    CheckResult(FMOD_RESULT result)
    {
        if(result == FMOD_OK)
        {
            return true;
        }

        LOG(LOG_WARNING, FMOD_ErrorString(result), Core::ELogChannel::CLOG_SOUND);

        return false;
    }

    Sound2D* SoundManager::Load2DSound(const std::string& path, const bool isLoop, const bool isUnique)
    {
        Sound2D* sound2D;
        const std::string soundIdentifier = path + (isLoop ? "loop" : "noloop");
        if(isUnique)
        {

            sound2D = ResourceManager::GetResource<Sound2D>(soundIdentifier);

            if (sound2D != nullptr)
            {
                return sound2D;
            }
        }

        FMOD_MODE soundFlags = FMOD_2D | FMOD_CREATESTREAM;
        if(isLoop)
        {
            soundFlags |= FMOD_LOOP_NORMAL;
        }
        else
        {
            soundFlags |= FMOD_LOOP_OFF;
        }

        FMOD::Sound* sound;
        soundManager.system->createSound(path.c_str(), soundFlags, nullptr, &sound);

        sound2D = Core::MemoryPool::Alloc<Sound2D>(path, sound);

        if(isUnique)
            ResourceManager::AddResource(soundIdentifier, sound2D);

        soundManager.system->playSound(sound2D->sound, nullptr, false, &sound2D->channel);

        sound2D->SetPauseSound(true);
        sound2D->isUnique = isUnique;
        sound2D->SetVolume(0.3f);

        return sound2D;
    }

    void SoundManager::Load3DSound(Sound3DComponent* soundComponent, const std::string& path, bool isLoop)
    {
        FMOD_MODE soundFlags = FMOD_3D | FMOD_CREATESTREAM;
        if (isLoop)
        {
            soundFlags |= FMOD_LOOP_NORMAL;
        }
        else
        {
            soundFlags |= FMOD_LOOP_OFF;
        }

        FMOD::Sound* sound;
        soundManager.system->createSound(path.c_str(), soundFlags, nullptr, &sound);

        soundComponent->SetFMODSound(sound);
    }

    void SoundManager::Play2DSound(const std::string& path, const bool isResetIfPlaying, const bool isPauseIfPlaying)
    {
        Sound2D* sound2D = Load2DSound(path, false);

        if(sound2D->IsPlaying() && isResetIfPlaying || !sound2D->IsPlaying() && !sound2D->IsPaused())
        {
            sound2D->PlaySound();
        }
        else if(isPauseIfPlaying && sound2D->IsPlaying() && !sound2D->IsPaused())
        {
            sound2D->SetPauseSound(true);
        }
        else
        {
            sound2D->SetPauseSound(false);
        }
    }

    void SoundManager::Update(const float elapsedTime)
    {
        LibMath::Vector3 cameraLocation;
        LibMath::Vector3 cameraForwardVector;
        LibMath::Vector3 cameraUpVector;

        if (Core::World::IsInPlay())
        {
            auto it = CameraComponent::GetAll();
            if (it.Next())
            {
                it->GetPosition();
                cameraLocation = it->GetPosition();
                cameraForwardVector = it->GetFront();
                cameraUpVector = it->GetUp();
            }
        }
        else
        {
            //sound init, sound manager needs to know camera's position and forward vector
            cameraLocation = Render::RenderApplication::GetCamera().GetPosition();
            cameraForwardVector = Render::RenderApplication::GetCamera().GetFront();
            cameraUpVector = Render::RenderApplication::GetCamera().GetUp();
        }

        FMOD_VECTOR velocity;
        velocity.x = (cameraLocation.x - soundManager.currentCameraLocation.x) * elapsedTime;
        velocity.y = (cameraLocation.y - soundManager.currentCameraLocation.y) * elapsedTime;
        velocity.z = (cameraLocation.z - soundManager.currentCameraLocation.z) * elapsedTime;

        soundManager.currentCameraLocation = cameraLocation;

        FMOD_VECTOR fmodLocation = { cameraLocation.x , cameraLocation.y , cameraLocation.z };

        FMOD_VECTOR forwardVector = { cameraForwardVector.x, cameraForwardVector.y, cameraForwardVector.z };

        FMOD_VECTOR upVector = { cameraUpVector.x, cameraUpVector.y, cameraUpVector.z };

        soundManager.system->set3DListenerAttributes(0, &fmodLocation, &velocity, &forwardVector, &upVector);

        soundManager.system->update();
    }

    SoundManager::SoundManager()
    {
        FMOD_RESULT result = System_Create(&system);
        ASSERT(CheckResult(result), "Sound Manager could not be created", Core::ELogChannel::CLOG_SOUND);

        result = system->init(512, FMOD_INIT_NORMAL, nullptr);
        ASSERT(CheckResult(result), "Sound Manager could not be initialized", Core::ELogChannel::CLOG_SOUND);
    }
}

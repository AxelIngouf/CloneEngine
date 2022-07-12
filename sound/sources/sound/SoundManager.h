#pragma once
#include <string>

#include "Sound2D.h"
#include "Sound3D.h"

namespace LibMath {
    struct Vector3;
}

namespace FMOD {
    class System;
}

struct FMOD_VECTOR;

namespace Sound
{
    class SoundManager
    {
    public:
        /**
         * Load a 2D sound that is paused by default.
         * 
         * @param path - Sound file path.
         * @param isLoop - States if the sound should loop.
         * @param isUnique - States if the sound is unique and should be managed by the resource manager.
         * If you want to play the same sound multiple times simultaneously you should set this to false.
         *
         * @return Newly loaded Sound2D pointer.
         */
        static Sound2D*         Load2DSound(const std::string& path, bool isLoop = false, bool isUnique = true);
        static void             Load3DSound(Sound3DComponent* soundComponent, const std::string& path, bool isLoop = false);

        static void             Play2DSound(const std::string& path, bool isResetIfPlaying = false, bool isPauseIfPlaying = false);

        static void             Update(float elapsedTime);

        static FMOD::System*    GetFMODSystem() { return soundManager.system; }

        static void             AttachPlayerCamera(const LibMath::Vector3 newCameraLocation, const LibMath::Vector3 forwardVector)
        {
            soundManager.cameraLocation = newCameraLocation;
            soundManager.cameraForwardVector = forwardVector;
            soundManager.currentCameraLocation = newCameraLocation;
        }

    private:
        SoundManager();

        static SoundManager soundManager;

        LibMath::Vector3 cameraLocation;
        LibMath::Vector3 cameraForwardVector;
        LibMath::Vector3 currentCameraLocation;


        FMOD::System* system = nullptr;
    };
}


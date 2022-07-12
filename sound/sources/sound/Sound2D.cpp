#include "Sound2D.h"

#include <fmod.hpp>
#include <utility>


#include "SoundManager.h"
#include "core/ResourceManager.h"

namespace Sound
{
    void Sound2D::PlaySound()
    {
        SoundManager::GetFMODSystem()->playSound(sound, nullptr, false, &channel);
        channel->setPaused(false);
        isPaused = false;
    }

    void Sound2D::SetPauseSound(const bool isPauseNew)
    {
        channel->setPaused(isPauseNew);
        isPaused = isPauseNew;
    }

    void Sound2D::Release() const
    {
        sound->release();
    }

    void Sound2D::SetVolume(const float volume) const
    {
        channel->setVolume(volume);
    }

    void Sound2D::SetPitch(const float pitch) const
    {
        channel->setPitch(pitch);
    }

    void Sound2D::SetMute(const bool isMuteNew)
    {
        channel->setMute(isMuteNew);
        isMute = isMuteNew;
    }

    float Sound2D::GetPitch() const
    {
        float pitch;
        channel->getPitch(&pitch);
        return pitch;
    }

    bool Sound2D::IsPlaying() const
    {
        bool isPlaying;
        channel->isPlaying(&isPlaying);
        return isPlaying;
    }

    Sound2D::Sound2D(std::string soundPath, FMOD::Sound* FMODSound)
        : sound(FMODSound), path(std::move(soundPath))
    {}

    Sound2D::Sound2D(Sound2D&& other) noexcept :
        sound(other.sound), channel(other.channel)
    {
        other.sound = nullptr;
        other.channel = nullptr;
    }

    Sound2D::Sound2D(const Sound2D& other) :
        sound(other.sound), channel(other.channel), isMute(other.isMute), isPaused(other.isPaused), isUnique(other.isUnique)
    {
    }

    Sound2D::~Sound2D()
    {
        if(sound)
        {
            sound->release();
            if(isUnique)
            {
                ResourceManager::DeleteResource(path + "noloop");
            }
        }
    }
}

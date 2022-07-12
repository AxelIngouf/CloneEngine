#pragma once
#include <string>

namespace FMOD {
    class Channel;
    class Sound;
}

namespace Sound
{
    class SoundManager;

    class Sound2D
    {
    public:
        Sound2D(std::string path, FMOD::Sound* sound);
        Sound2D(Sound2D&& other) noexcept;
        Sound2D(const Sound2D& other);
        ~Sound2D();

        void    PlaySound();
        void    SetPauseSound(bool isPauseNew);

        void    Release() const;

        void    SetVolume(float volume) const;
        void    SetPitch(float pitch) const;
        void    SetMute(bool isMuteNew);

        [[nodiscard]] bool  IsUnique() const { return isUnique; }
        [[nodiscard]] bool  IsPaused() const { return isPaused; }
        [[nodiscard]] float GetPitch() const;
        [[nodiscard]] bool  IsPlaying() const;
        [[nodiscard]] const std::string& GetPath() const { return path; }

    protected:
        friend SoundManager;
        Sound2D() = default;

        FMOD::Sound*    sound = nullptr;
        FMOD::Channel*  channel = nullptr;
        std::string     path;
        bool    isMute = false;
        bool    isPaused = false;

    private:
        bool    isUnique = false;
    };
}


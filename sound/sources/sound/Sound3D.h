#pragma once
#include <string>

#include "Sound2D.h"
#include "core/ECS/Component.h"
#include "Vector/TVector2.h"
#include "Vector/Vector3.h"
#include "Core/File.h"

namespace FMOD {
    class Channel;
    class Sound;
}

namespace Sound
{
    namespace Wrapper // todo: replace with real reflection data
    {
        inline static const Core::Type Sound2D_("Sound2D", Core::ETypeCategory::WRAPPER, sizeof(Sound2D), 8);
    }

    STRUCT(SoundComponentParams,
        FIELD(File, file),
        FIELD(float, minDistance),
        FIELD(float, maxDistance),
        FIELD(bool, isLoop),
        SUPPLEMENT(
            EMPTY(),
            SoundComponentParams();,
            EMPTY()
        )
    );

    COMPONENT(Sound3DComponent,
        OTHER_BASE(Sound2D),
        INIT_PARAM(SoundComponentParams),
        PRIVATE_FIELD(LibMath::Vector3, location, Core::FieldFlag::EDITOR_HIDDEN),
        FUNCTION(void, Initialize, const void*, params),
        FUNCTION(void, Constructor),
        FUNCTION(void, BeginPlay),
        FUNCTION(void, Finalize),
        FUNCTION(void, Update, float, elapsedTime),
        FUNCTION(void, UpdateLocation, LibMath::Vector3, newLocation, float, elapsedTime),
        FUNCTION(void, SetMinMaxDistance, float, minDistance, float, maxDistance),
        FUNCTION(void, Set3DConeOrientation, LibMath::Vector3, orientation),
        FUNCTION(void, Set3DConeSettings, float, innerAngle, float, outerAngler, float, outerVolume),
        FUNCTION(void, SetLocation, LibMath::Vector3, newLocation),
        FUNCTION(LibMath::Vector3, GetLocation),
        FUNCTION(void, CreateSound, std::string, soundPath, bool, isLoop = false),
        FUNCTION(void, SetFMODSound, void*, fmodSound),
        FIELD(std::string, path, Core::FieldFlag::READONLY),
        FIELD(float, minDistance),
        FIELD(float, maxDistance),
        FIELD(bool, isLoop),
        SUPPLEMENT(
            EMPTY(),
            Sound3DComponent() = default;
            Sound3DComponent(Sound3DComponent&& other) noexcept; ,
        EMPTY()
        )
    );
}


#pragma once
#include "core/ECS/Component.h"
#include "Vector/TVector2.h"
#include "imgui/UIObject.h"
#include "core/File.h"

constexpr float DELTA_COUNTER = 0.1f;

COMPONENT(HUDSpeedComponent,
    FUNCTION(void, Constructor),
    FUNCTION(void, Update, float, deltaTime),
    FUNCTION(void, Finalize),
    FIELD(UITextPointer, speed),
    FIELD(float, deltaCount, Core::FieldFlag::EDITOR_HIDDEN),
    FIELD(LibMath::Vector3, currentVehicleLocation, Core::FieldFlag::READONLY),
    SUPPLEMENT(
        EMPTY(),
        HUDSpeedComponent();
        HUDSpeedComponent(HUDSpeedComponent && other) noexcept;,
        EMPTY()
    )
);


COMPONENT(HUDCheckpointsComponent,
    FUNCTION(void, BeginPlay),
    FUNCTION(void, Finalize),
    FUNCTION(void, UpdateCheckpoints, int, checkpointId, int, totalCheckpoints),
    FIELD(UITextPointer, checkpoints),
    SUPPLEMENT(
        EMPTY(),
        HUDCheckpointsComponent() = default;
        HUDCheckpointsComponent(HUDCheckpointsComponent && other) noexcept;,
        EMPTY()
    )
);

COMPONENT(HUDLapsComponent,
    FUNCTION(void, BeginPlay),
    FUNCTION(void, Finalize),
    FUNCTION(void, UpdateLaps, int, currentLap, int, totalLap),
    FIELD(UITextPointer, laps),
    SUPPLEMENT(
        EMPTY(),
        HUDLapsComponent() = default;
        HUDLapsComponent(HUDLapsComponent && other) noexcept;,
        EMPTY()
    )
);

COMPONENT(HUDDistanceTraveledComponent,
    FUNCTION(void, BeginPlay),
    FUNCTION(void, Update, float, deltaTime),
    FUNCTION(void, Finalize),
    FIELD(UITextPointer, distance),
    FIELD(float, maxDistance, Core::FieldFlag::READONLY),
    SUPPLEMENT(
        EMPTY(),
        HUDDistanceTraveledComponent();
        HUDDistanceTraveledComponent(HUDDistanceTraveledComponent && other) noexcept;,
        EMPTY()
    )
);

STRUCT(UITextParams,
    FIELD(std::string, text),
    FIELD(LibMath::Vector2, location),
    FIELD(float, fontSize),
    SUPPLEMENT(
        EMPTY(),
        UITextParams(); ,
        EMPTY()
    )
);

COMPONENT(UITextComponent,
    INIT_PARAM(UITextParams),
    FUNCTION(void, Initialize, const void*, params),
    FUNCTION(void, Constructor),
    FUNCTION(void, Finalize),
    FIELD(std::string, textStr),
    FIELD(LibMath::Vector2, location),
    FIELD(float, fontSize),
    FIELD(UITextPointer, text)
);

STRUCT(UIImageParams,
    FIELD(File, file),
    FIELD(LibMath::Vector2, location),
    FIELD(LibMath::Vector2, size),
    SUPPLEMENT(
        EMPTY(),
        UIImageParams();,
        EMPTY()
    )
);

COMPONENT(UIImageComponent,
    INIT_PARAM(UIImageParams),
    FUNCTION(void, Initialize, const void*, params),
    FUNCTION(void, Constructor),
    FUNCTION(void, Finalize),
    FIELD(std::string, path),
    FIELD(LibMath::Vector2, location),
    FIELD(LibMath::Vector2, size),
    FIELD(UIImagePointer, image)
);

STRUCT(UIButtonParams,
    FIELD(std::string, text),
    FIELD(LibMath::Vector2, location),
    FIELD(LibMath::Vector2, size),
    SUPPLEMENT(
        EMPTY(),
        UIButtonParams();,
        EMPTY()
    )
);

COMPONENT(UIButtonComponent,
    INIT_PARAM(UIButtonParams),
    FUNCTION(void, Initialize, const void*, params),
    FUNCTION(void, Constructor),
    FUNCTION(void, Finalize),
    FIELD(std::string, textStr),
    FIELD(LibMath::Vector2, location),
    FIELD(LibMath::Vector2, size),
    FIELD(UIButtonPointer, button)
);
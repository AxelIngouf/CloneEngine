#include "HUDComponent.h"

#include "CarControllerComponent.h"

#include "core/ECS/Entity.h"
#include "core/scenegraph/SceneNode.h"
#include "imgui/ImguiImpl.h"

HUDSpeedComponent::HUDSpeedComponent() :
    deltaCount(0.f)
{
}

HUDSpeedComponent::HUDSpeedComponent(HUDSpeedComponent&& other) noexcept
    : speed(other.speed)
{
    other.speed.text = nullptr;
}

void HUDSpeedComponent::Constructor()
{
    speed.text = ImGuiImpl::ui.AddText("0 km/h", 50, 50, 50);

    ((UIText*)speed.text)->isHidden = false;
    //speed->SetAnchor(EAnchor::BOTTOM);

    currentVehicleLocation = Core::Entity::GetEntity(GetEntityHandle())->GetAnchor()->GetWorldTransformNoCheck().position;
}

void HUDSpeedComponent::Update(const float deltaTime)
{
    deltaCount += deltaTime;
    if(deltaCount >= DELTA_COUNTER)
    {
        const auto vehicleLocation = Core::Entity::GetEntity(GetEntityHandle())->GetAnchor()->GetWorldTransformNoCheck().position;

        const float diffX = currentVehicleLocation.x - vehicleLocation.x;
        const float diffY = currentVehicleLocation.y - vehicleLocation.y;
        const float diffZ = currentVehicleLocation.z - vehicleLocation.z;

        const float currentSpeed = LibMath::sqrt(diffX * diffX + diffY * diffY + diffZ * diffZ) / deltaCount / 0.27777777777778f;

        ((UIText*)speed.text)->text = std::to_string(static_cast<int>(currentSpeed)) + " km/h";

        currentVehicleLocation = vehicleLocation;

        deltaCount = 0.f;
    }
}

void HUDSpeedComponent::Finalize()
{
    if(speed.text)
        ImGuiImpl::ui.ReleaseUIObject(((UIText*)speed.text));
}

HUDCheckpointsComponent::HUDCheckpointsComponent(HUDCheckpointsComponent&& other) noexcept
    : checkpoints(other.checkpoints)
{
    other.checkpoints.text = nullptr;
}

void HUDCheckpointsComponent::BeginPlay()
{
    auto carControllerComponents = Core::Entity::GetEntity(GetEntityHandle())->GetComponents<CarControllerComponent>();
    if (carControllerComponents.Size() > 0)
    {
        checkpoints.text = ImGuiImpl::ui.AddText("checkpoints : 0 / 0", 250, 50, 50);

        carControllerComponents[0]->onCheckpointReached.Add(&HUDCheckpointsComponent::UpdateCheckpoints, this);
    }
}

void HUDCheckpointsComponent::Finalize()
{
    if(checkpoints.text)
        ImGuiImpl::ui.ReleaseUIObject(((UIText*)checkpoints.text));
}

void HUDCheckpointsComponent::UpdateCheckpoints(int checkpointId, int /*totalCheckpoints*/)
{
    ((UIText*)checkpoints.text)->text = "checkpoints : " + std::to_string(static_cast<int>(checkpointId - 1)) + " / " + std::to_string(4);
}

HUDLapsComponent::HUDLapsComponent(HUDLapsComponent&& other) noexcept
    : laps(other.laps)
{
    other.laps.text = nullptr;
}

void HUDLapsComponent::BeginPlay()
{
    auto carControllerComponents = Core::Entity::GetEntity(GetEntityHandle())->GetComponents<CarControllerComponent>();
    if (carControllerComponents.Size() > 0)
    {
        laps.text = ImGuiImpl::ui.AddText("laps : 0 / 0", 550, 50, 50);

        carControllerComponents[0]->onLapFinished.Add(&HUDLapsComponent::UpdateLaps, this);
    }
}

void HUDLapsComponent::Finalize()
{
    if (laps.text)
        ImGuiImpl::ui.ReleaseUIObject(((UIText*)laps.text));
}

void HUDLapsComponent::UpdateLaps(int lap, int /*totalLap*/)
{
    ((UIText*)laps.text)->text = "laps : " + std::to_string(static_cast<int>(lap)) + " / " + std::to_string(3);
}

HUDDistanceTraveledComponent::HUDDistanceTraveledComponent() :
    maxDistance(0.f)
{
}

HUDDistanceTraveledComponent::HUDDistanceTraveledComponent(HUDDistanceTraveledComponent&& other) noexcept
    : distance(other.distance)
{
    other.distance.text = nullptr;
}

void HUDDistanceTraveledComponent::BeginPlay()
{
    (distance.text) = ImGuiImpl::ui.AddText("0 meters", 50, 100, 50);
}

void HUDDistanceTraveledComponent::Update(const float /*deltaTime*/)
{
    const auto vehicleLocation = Core::Entity::GetEntity(GetEntityHandle())->GetAnchor()->GetWorldTransformNoCheck().position;

    const auto distanceFromOrigin = abs(vehicleLocation.x) + vehicleLocation.z;

    if(maxDistance < distanceFromOrigin)
    {
        const auto distanceString = std::to_string(distanceFromOrigin);
        ((UIText*)distance.text)->text = distanceString.substr(0, distanceString.find_first_of(".") + 3) + " meters";
        maxDistance = distanceFromOrigin;
    }
}

void HUDDistanceTraveledComponent::Finalize()
{
    if (distance.text)
        ImGuiImpl::ui.ReleaseUIObject(((UIText*)distance.text));
}

UITextParams::UITextParams() :
    text("Text here"),
    fontSize(10.f)
{
}

void UITextComponent::Initialize(const void* params)
{
    if (params)
    {
        const UITextParams* initParams = static_cast<const UITextParams*>(params);

        textStr = initParams->text;
        location = initParams->location;
        fontSize = initParams->fontSize;
    }
}

void UITextComponent::Constructor()
{
    text.text = ImGuiImpl::ui.AddText(textStr, location.x, location.y, fontSize);
}

void UITextComponent::Finalize()
{
    ImGuiImpl::ui.ReleaseUIObject(text.text);
}

UIImageParams::UIImageParams() :
    file(File(EFileType::IMAGE)),
    size(LibMath::Vector2(5.f, 5.f))
{
}

void UIImageComponent::Initialize(const void* params)
{
    if(params)
    {
        const UIImageParams* initParams = static_cast<const UIImageParams*>(params);

        path = initParams->file.path;
        location = initParams->location;
        size = initParams->size;
    }
}

void UIImageComponent::Constructor()
{
    image.image = ImGuiImpl::ui.AddImage(path, location.x, location.y, size.x, size.y);
}

void UIImageComponent::Finalize()
{
    ImGuiImpl::ui.ReleaseUIObject(image.image);
}

UIButtonParams::UIButtonParams() :
    text(""),
    size(LibMath::Vector2(5.f, 5.f))
{
}

void UIButtonComponent::Initialize(const void* params)
{
    if (params)
    {
        const UIButtonParams* initParams = static_cast<const UIButtonParams*>(params);

        textStr = initParams->text;
        location = initParams->location;
        size = initParams->size;
    }
}

void UIButtonComponent::Constructor()
{
    button.button = ImGuiImpl::ui.AddButton(textStr, location.x, location.y, size.x, size.y);
}

void UIButtonComponent::Finalize()
{
    button.button->onButtonPressed.ClearDelegates();
    ImGuiImpl::ui.ReleaseUIObject(button.button);
}


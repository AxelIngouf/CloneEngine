#include "BoulderComponent.h"
#include "core/ECS/Entity.h"
#include "core/scenegraph/SceneNode.h"

void BoulderComponent::Update(float /*elapsedTime*/)
{
    if(Core::Entity::GetEntity(GetEntityHandle())->GetAnchor()->GetWorldTransformNoCheck().position.y < BOULDER_MINY_FALL)
    {
        Core::Entity::GetEntity(GetEntityHandle())->GetAnchor()->Destroy();
    }
}
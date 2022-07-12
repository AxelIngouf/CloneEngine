#pragma once
#include "core/scenegraph/SceneNode.h"

constexpr int PLATFORM_XZ_SIZE = 20;
constexpr float PLATFORM_LIFESPAN = 3.5f;

const Core::Entity* CreatePlatformEntity(Core::SceneNode* entityAnchor);


#pragma once

#include "PxPhysicsAPI.h"

namespace Physics
{
    class ErrorCallback final : public physx::PxErrorCallback
    {
        void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) override;
    };
}


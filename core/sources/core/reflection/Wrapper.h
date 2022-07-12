#pragma once

#include "ReflectionAPI.h"
#include "BlackMagic.h"

#include "Matrix/Matrix.h"
#include "Quaternion/Quaternion.h"
#include "Vector/Vector.h"

#define WRAPPER(name, space, step) inline static const Core::Type name (STRINGYFY(space ## :: ## name), ETypeCategory::WRAPPER, sizeof(space ## :: ## name), step);

namespace Core
{
    namespace Wrapper
    {
        WRAPPER(Matrix4, LibMath, sizeof(float))
        WRAPPER(Quaternion, LibMath, sizeof(float))
        WRAPPER(Vector4, LibMath, sizeof(float))
        WRAPPER(Vector3, LibMath, sizeof(float))
        WRAPPER(Vector2, LibMath, sizeof(float))
    };
}

#pragma once

#include "core/reflection/StructMeta.h"
#include "Vector/Vector.h"
#include "Quaternion/Quaternion.h"

namespace Core
{
    STRUCT(
        Transform,
        FIELD(LibMath::Vector3, position),
        FIELD(LibMath::Quaternion, rotation),
        FIELD(LibMath::Vector3, scale),

        OPERATOR(const Transform&, operator=, const Transform&, rhs),
        OPERATOR(const Transform&, operator+=, const Transform&, rhs)
    );

    Transform operator+(Transform lhs, const Transform& rhs);
    bool operator==(const Transform& lhs, const Transform& rhs);
    bool operator!=(const Transform& lhs, const Transform& rhs);
}

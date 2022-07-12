#pragma once
#include <string>
#include "reflection/StructMeta.h"
#include "reflection/EnumMeta.h"


ENUM(EFileType,
    CONSTANT(SOUND),
    CONSTANT(MODEL),
    CONSTANT(IMAGE)
);

STRUCT(File,
    FUNCTION(void, SetPath, std::string, newPath),
    FUNCTION(bool, IsValid, std::string, newPath),
    FIELD(std::string, path),
    FIELD(EFileType, fileType, Core::FieldFlag::READONLY),
    SUPPLEMENT(
        EMPTY(),
        File() = default;
        File(EFileType pFileType); ,
        EMPTY()
    )
);
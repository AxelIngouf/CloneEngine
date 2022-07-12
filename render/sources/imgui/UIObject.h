#pragma once
#include <string>

#include "core/Delegate.h"
#include "Vector/TVector2.h"
#include "core/reflection/StructMeta.h"
#include "core/reflection/EnumMeta.h"
#include "core/ECS/Component.h"

ENUM(EAnchor,
    CONSTANT(TOP),
    CONSTANT(BOTTOM),
    CONSTANT(LEFT),
    CONSTANT(RIGHT),
    CONSTANT(CENTER),
    CONSTANT(TOP_LEFT),
    CONSTANT(TOP_RIGHT),
    CONSTANT(BOTTOM_LEFT),
    CONSTANT(BOTTOM_RIGHT)
);

ENUM(EObjectType,
    CONSTANT(TEXT, 1),
    CONSTANT(IMAGE, 2),
    CONSTANT(BUTTON, 3)
)

STRUCT(UIObject,
    FUNCTION(void, Draw),
    FUNCTION(void, Init),
    FUNCTION(void, SetLocationX, float, newX),
    FUNCTION(void, SetLocationY, float, newY),
    FUNCTION(void, SetLocation, float, newX, float, newY),
    FUNCTION(void, SetScreenLocation, float, newX, float, newY),
    FUNCTION(void, SetAnchor, EAnchor, newAnchor),
    FUNCTION(void, SetCursor),
    FUNCTION(void, UpdateScreenLocation),
    FUNCTION(LibMath::Vector2, GetRelativeLocation),
    FUNCTION(LibMath::Vector2, GetScreenLocation),
    FIELD(bool, isHidden),
    FIELD(float, xScreenLocation, Core::FieldFlag::READONLY),
    FIELD(float, yScreenLocation, Core::FieldFlag::READONLY),
    FIELD(float, xLocalLocation),
    FIELD(float, yLocalLocation),
    FIELD(bool, isDirtyLocation, Core::FieldFlag::EDITOR_HIDDEN),
    FIELD(int, id, Core::FieldFlag::EDITOR_HIDDEN),
    FIELD(EAnchor, anchor),
    FIELD(EObjectType, objectType),
    SUPPLEMENT(
        EMPTY(),
        UIObject();
        explicit UIObject(std::string otherText, int objectId, float x = 0.f, float y = 0.f, float fontSize = 10.f);,
        EMPTY()
    )
);


STRUCT(UIText,
    BASE(UIObject),
    FUNCTION(void, Draw),
    FIELD(float, size),
    FIELD(std::string, text),
    SUPPLEMENT(
        EMPTY(),
        UIText();
        explicit UIText(std::string otherText, int objectId, float x = 0.f, float y = 0.f, float fontSize = 10.f); ,
        EMPTY()
    )
);

STRUCT(UIImage,
    BASE(UIObject),
    FUNCTION(void, Draw),
    FUNCTION(void, Init),
    FIELD(float, width),
    FIELD(float, height),
    FIELD(std::string, path, Core::FieldFlag::READONLY),
    FIELD(void*, imagePointer, Core::FieldFlag::EDITOR_HIDDEN),
    SUPPLEMENT(
        EMPTY(),
        UIImage();
        explicit UIImage(const std::string & imagePath, int objectId, float x = 0.f, float y = 0.f, float imageWidth = -1.f, float imageHeight = -1.f);,
        EMPTY()
    )
);

DELEGATE(ButtonPressed);

STRUCT(UIButton,
    BASE(UIObject),
    FUNCTION(void, Draw),
    FIELD(float, width),
    FIELD(float, height),
    FIELD(std::string, label),
    SUPPLEMENT(
        EMPTY(),
        UIButton();
        explicit UIButton(const std::string& buttonLabel, int objectId, float x = 0.f, float y = 0.f, float buttonWidth = -1.f, float buttonHeight = -1.f);
        OnButtonPressed onButtonPressed;,
        EMPTY()
    )
);

STRUCT(UITextPointer,
    //FIELD(int, id = -1, Core::FieldFlag::EDITOR_HIDDEN | Core::FieldFlag::TRANSIENT),
    SUPPLEMENT(
        EMPTY(),
        UIText * text = nullptr;,
        EMPTY()
    )
);

STRUCT(UIImagePointer,
    //FIELD(int, id = -1, Core::FieldFlag::EDITOR_HIDDEN | Core::FieldFlag::TRANSIENT),
    SUPPLEMENT(
        EMPTY(),
        UIImage * image = nullptr; ,
        EMPTY()
    )
);

STRUCT(UIButtonPointer,
    //FIELD(int, id = -1, Core::FieldFlag::EDITOR_HIDDEN | Core::FieldFlag::TRANSIENT),
    SUPPLEMENT(
        EMPTY(),
        UIButton * button = nullptr; ,
        EMPTY()
    )
);
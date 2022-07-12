#pragma once

#include <string>

#include "core/ECS/Component.h"
#include "core/reflection/StructMeta.h"

struct lua_State;
using luaStatePtr = lua_State*;

namespace script
{
    using namespace Core; // TODO : make reflection namespace resistant

    namespace Wrapper
    {
        inline static const Type lua_State_("luaStatePtr", ETypeCategory::WRAPPER, sizeof(lua_State*), sizeof(lua_State*));
    }

    COMPONENT(ScriptComponent,

        FUNCTION(void, Initialize, const void*, params),
        FUNCTION(void, Constructor),
        FUNCTION(void, Finalize),
        FUNCTION(void, Update, float, elapsedTime),

        PRIVATE_FUNCTION(void, LoadScript),
        PRIVATE_FUNCTION(void, InitializeScript),

        PRIVATE_FIELD(std::string, script),
        PRIVATE_FIELD(luaStatePtr, L, FieldFlag::TRANSIENT),
        PRIVATE_FIELD(bool, HasUpdate, FieldFlag::TRANSIENT),

        SUPPLEMENT(
            EMPTY(),

            private:
                static int GetData(lua_State* L);
                static int SetData(lua_State* L);
                static int InvokeFunction(lua_State* L);
                static int log(lua_State* L);,
                   
            EMPTY()
        )
    )
}
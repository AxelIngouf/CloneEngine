#include "ScriptComponent.h"

#include <iostream>
#include <string>

extern "C"
{
#include <lauxlib.h> // everything with the "luaL_" prefix
#include <lua.h> // everything with the "lua_" prefix
#include <lualib.h>
}

#include "core/CLog.h"

namespace script
{
	void ScriptComponent::Initialize(const void* params)
	{
		script = (const char*)params;
	}

	void ScriptComponent::Constructor()
	{
		L = luaL_newstate();
		luaL_openlibs(L);

		lua_register(L, "print", log);
		lua_register(L, "InvokeFunction", InvokeFunction);

		LoadScript();
	}

	void ScriptComponent::Finalize()
	{
		lua_getglobal(L, "Finalize");
		if (lua_isfunction(L, -1))
		{
			if (lua_pcall(L, 1, 0, 0) != 0)
			{
				LOG(Core::ELogLevel::CLOG_ERROR,
					Core::CLog::FormatString("%s::Finalize() error: %s", script.c_str(), lua_tostring(L, -1)),
					Core::ELogChannel::CLOG_SCRIPT); // todo: make this line work
			}
		}

		lua_close(L);
		L = nullptr;
	}

	void ScriptComponent::Update(float elapsedTime)
	{
		if (HasUpdate == false)
		{
			return;
		}

		lua_getglobal(L, "Update");  /* function to be called */

		lua_pushnumber(L, elapsedTime);

		if (lua_pcall(L, 1, 0, 0) != 0)
		{
			LOG(Core::ELogLevel::CLOG_ERROR,
				Core::CLog::FormatString("%s::Update(%.3f) error: %s", script.c_str(), elapsedTime, lua_tostring(L, -1)),
				Core::ELogChannel::CLOG_SCRIPT); // todo: make this line work
		}
	}

	void ScriptComponent::LoadScript() // todo: re-use Lua_state.
	{
		size_t nameSize = script.size();
		const char* folder = "assets/";
		const size_t folderSize = strlen(folder);
		char* scriptPath = new char[folderSize + nameSize + 1]; // todo use resource manager
		memcpy(scriptPath, folder, folderSize);
		memcpy(&scriptPath[folderSize], &script[0], nameSize);
		scriptPath[folderSize + nameSize] = '\0';

		if (luaL_dofile(L, scriptPath))
		{
			LOG(Core::ELogLevel::CLOG_ERROR,
				Core::CLog::FormatString("%s load error : %s", script.c_str(), lua_tostring(L, -1)),
				Core::ELogChannel::CLOG_SCRIPT); // todo: make this line work
		}
		delete[] scriptPath;

		InitializeScript();

		lua_getglobal(L, "Update");
		HasUpdate = lua_isfunction(L, -1);
	}

	void ScriptComponent::InitializeScript()
	{
		lua_getglobal(L, "Initialize");
		if (lua_isfunction(L, -1) == false)
		{
			return;
		}

		if (lua_pcall(L, 1, 0, 0) != 0)
		{
			LOG(Core::ELogLevel::CLOG_ERROR,
				Core::CLog::FormatString("%s::Initialize() error: %s", script.c_str(), lua_tostring(L, -1)),
				Core::ELogChannel::CLOG_SCRIPT); // todo: make this line work
		}
	}

	int ScriptComponent::GetData(lua_State* /*LS*/)
	{
		return 1;
	}

	int ScriptComponent::SetData(lua_State* /*LS*/)
	{
		return 0;
	}

	int ScriptComponent::InvokeFunction(lua_State* /*LS*/)
	{
		return 0;
	}

	int ScriptComponent::log(lua_State* LS)
	{
		const char* message = luaL_checkstring(LS, -1);
		luaL_argcheck(LS, message, 1, "string expected");

		LOG(Core::ELogLevel::CLOG_ERROR, message, Core::ELogChannel::CLOG_GENERAL);

		return 0;
	}

	//void Scrip()
	//{

	//	/* initialize Lua */
	//	L = luaL_newstate();

	//	/* load Lua base libraries */
	//	luaL_openlibs(L);

	//	/* run the script */
	//	luaL_dofile(L, "assets/script/test.lua");

	//	/* cleanup Lua */
	//	lua_close(L);
	//}
}

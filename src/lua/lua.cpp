#include "lua.h"

#include "../includes.h"
#include "../config.h"
#include "../../libs/lua/luaaa.hpp"

void LuaEditor::Init()
{
	LuaState = luaL_newstate();
	luaL_openlibs(LuaState);
	BindLua(LuaState);
}

void LuaEditor::BindLua(lua_State* L)
{
	luaaa::LuaModule(L)
		.def("pi", 3.1415926535897932)
		.fun("testFn", []() {
		std::cout << "Yoo bro\n" << std::endl;
			})
		.fun("test", []() {
				ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(Config::System::ScreenCenter.X, Config::System::ScreenCenter.Y), 50, ImColor(255, 255, 255), 360);
			});
}

void LuaEditor::DrawLuaEditor()
{
	ImGui::SetNextWindowSize(ImVec2(700.000f, 400.000f), ImGuiCond_Once);
	if (!ImGui::Begin("LuaEditor by kiocode", nullptr)) {
		ImGui::End();
		return;
	}

	ImGui::InputTextMultiline("##lua", LuaScript, sizeof(LuaScript), ImVec2(700, 300));

	if (ImGui::Button("Execute##lua")) {
		try {
			int err = luaL_loadstring(LuaState, LuaScript);
			if (err == 0)
			{
				err = lua_pcall(LuaState, 0, 0, 0);
			}

			if (err)
			{
				std::cout << "\nlua err: " << lua_tostring(LuaState, -1) << std::endl;
				lua_pop(LuaState, 1);
			}
		}
		catch (const std::exception& e) {
			std::cout << "lua err: " << e.what() << std::endl;
		}
	}

	ImGui::End();
}
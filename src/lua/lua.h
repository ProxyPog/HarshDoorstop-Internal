#pragma once

#include "../../libs/lua/lua.hpp"

class LuaEditor {
public:
	static LuaEditor& GetInstance() {
		static LuaEditor instance;
		return instance;
	}

	bool ShowEditor = false;

	char LuaScript[9999] = R"(
print("Hello World!")

local test = 0
print(test)

function TestFunction()
	print("TestFunction")
end

TestFunction()

-- call c++ function
testFn()
		)";

	lua_State* LuaState = nullptr;
	void Init();
	void RenderLuaEditor();
	void BindLua(lua_State* L);
	
private:
	LuaEditor() = default;
	~LuaEditor() = default;

	LuaEditor(const LuaEditor&) = delete;
	LuaEditor& operator=(const LuaEditor&) = delete;
};
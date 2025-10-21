#pragma once

#include <Windows.h>

#include "../includes.h"

namespace Utility 
{
	void CreateConsole();
	void MouseMove(float tarx, float tary, float X, float Y, int smooth);
	void Log(uintptr_t address, const char* className, const char* methodName);
	void LogError(const char* text, const char* name);
	void ApplyChams(SDK::USkeletalMeshComponent* mesh, bool isVisible);
	SDK::FName StrToName(const wchar_t* str);
}
#pragma once

#include "../includes.h"

class ESP
{
public:
	static ESP& GetInstance() {
		static ESP instance;
		return instance;
	}

	void RenderSkeleton(SDK::ACharacter* pawn, ImColor color);
	void RenderSnapline(SDK::ACharacter* pawn, ImColor color);
	void RenderText(SDK::ACharacter* pawn, std::string& text, ImColor color);
	void RenderBox(SDK::ACharacter* pawn, ImColor color);
	void Render3DBox(SDK::ACharacter* pawn, ImColor color);

private:

	ESP() = default;
	~ESP() = default;

	ESP(const ESP&) = delete;
	ESP& operator=(const ESP&) = delete;
};
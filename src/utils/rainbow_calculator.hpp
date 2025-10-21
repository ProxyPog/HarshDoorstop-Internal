#include "../config.h"
#include "../includes.h"

namespace RainbowCalculator 
{

	void Update() 
	{
		float isRed = 0.0f, isGreen = 0.01f, isBlue = 0.0f;

		while (true)
		{
			if(ImGui::GetCurrentContext() == nullptr) continue;

			if (ImGui::GetFrameCount() % 1 == 0)
			{
				if (isGreen == 0.01f && isBlue == 0.0f) isRed += 0.01f;
				if (isRed > 0.99f && isBlue == 0.0f) { isRed = 1.0f; isGreen += 0.01f; }
				if (isGreen > 0.99f && isBlue == 0.0f) { isGreen = 1.0f; isRed -= 0.01f; }
				if (isRed < 0.01f && isGreen == 1.0f) { isRed = 0.0f; isBlue += 0.01f; }
				if (isBlue > 0.99f && isRed == 0.0f) { isBlue = 1.0f; isGreen -= 0.01f; }
				if (isGreen < 0.01f && isBlue == 1.0f) { isGreen = 0.0f; isRed += 0.01f; }
				if (isRed > 0.99f && isGreen == 0.0f) { isRed = 1.0f; isBlue -= 0.01f; }
				if (isBlue < 0.01f && isGreen == 0.0f) { isBlue = 0.0f; isRed += 0.01f; if (isRed < 0.01f) isGreen = 0.01f; }
			}

			Config::m_v4Rainbow = ImVec4(isRed, isGreen, isBlue, 1.0f);
			Config::m_v4Rainbow = ImColor(Config::m_v4Rainbow.x, Config::m_v4Rainbow.y, Config::m_v4Rainbow.z);

			Sleep(20);
		}
	}

}
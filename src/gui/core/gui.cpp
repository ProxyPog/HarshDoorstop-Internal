#include "gui.h"

#include "../modmenu.h"
#include "../../config.h"
#include "../../includes.h"
#include "../../features/core.h"
#include "../../features/main_loop.h"
#include "../../utils/general.h"
#include "../../utils/imgui/fonts/font.h"
#include "../../utils/imgui/imgui_helper.h"

void GUI::InitImGui()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(pDevice, pContext);
}

void GUI::LoadFonts() 
{
	ImGui::GetIO().Fonts->AddFontDefault();
	ImFontConfig font_cfg;
	font_cfg.GlyphExtraSpacing.x = 1.2;
	Config::m_pGameFont = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(TTSquaresCondensedBold, 14, 14, &font_cfg);
}

void GUI::RenderMouse() 
{
	if (!Config::m_bShowMouse) return;

	ImColor color = Config::m_bRainbowMouse ? Config::m_cRainbow : Config::m_cMouseColor;

	switch (Config::m_nMouseType) {
	case 0:
		ImGui::GetForegroundDrawList()->AddCircleFilled(ImGui::GetMousePos(), 4, color);
		break;
	case 1:
		ImGuiHelper::DrawOutlinedTextForeground(Config::m_pGameFont, ImVec2(Config::System::m_MousePos.x, Config::System::m_MousePos.y), 13.0f, color, false, "X");
		break;
	case 2:
		if (!ImGui::GetIO().MouseDrawCursor) {
			ImGui::GetIO().MouseDrawCursor = true;
		}
		break;
	}
}

void GUI::RenderUI() 
{

	if (Config::m_bShowMenu)
	{
		RenderMouse();

		// our menu
		ModMenu::Window();
	}

	if (Config::m_bWatermark)
	{
		ImGuiHelper::DrawOutlinedText(Config::m_pGameFont, ImVec2(Config::System::m_ScreenCenter.X, Config::System::m_ScreenSize.Y - 20), 13.0f, Config::m_cRainbow, true, Config::System::m_cAuthor);
		ImGuiHelper::DrawOutlinedText(Config::m_pGameFont, ImVec2(Config::System::m_ScreenCenter.X, 5), 13.0f, Config::m_cRainbow, true, "[ %.1f FPS ]", ImGui::GetIO().Framerate);
	}

	if (Config::m_bCrosshair) {
		MainLoop::GetInstance().DrawCrosshair();
	}

	if (Config::m_bAimbotFOVCheck) {
		ImGui::GetForegroundDrawList()->AddCircle(ImVec2(Config::System::m_ScreenCenter.X, Config::System::m_ScreenCenter.Y), Config::m_fAimbotFOV, ImColor(255, 255, 255), 360);
	}

}


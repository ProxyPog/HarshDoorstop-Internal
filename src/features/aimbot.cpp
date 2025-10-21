#include "aimbot.h"

#include "../config.h"
#include "../utils/general.h"
//myController->GetActorEyesViewPoint(&eyeLocation, &eyeRotation);
		   //myController->SetControlRotation(targetRot);
  auto myController = Config::m_pMyController;
    auto myPawn = Config::m_pMyPawn;
    auto myCharacter = Config::m_pMyCharacter;
    auto world = Config::m_pWorld;
void Aimbot::RegularAimbot(SDK::ACharacter* pawn) 
{
	if (!pawn || !Config::m_pMyController || Config::m_BonePairs.empty())
		return;

	SDK::USkeletalMeshComponent* mesh = pawn->Mesh;
	if (!mesh)
		return;

	SDK::FVector head = mesh->GetSocketLocation(mesh->GetBoneName(Config::m_BonePairs[4].second));

	SDK::FVector2D headScreen;

	if (
		!Config::m_pMyController->ProjectWorldLocationToScreen(head, &headScreen, false)
	) return;

	if (Config::m_bAimbotFOVCheck)
		//coord >/< screencord -/+ fov
	{
		if (headScreen.X > (Config::System::m_ScreenCenter.X + Config::m_fAimbotFOV))
			return;
		if (headScreen.X < (Config::System::m_ScreenCenter.X - Config::m_fAimbotFOV))
			return;
		if (headScreen.Y > (Config::System::m_ScreenCenter.Y + Config::m_fAimbotFOV))
			return;
		if (headScreen.Y < (Config::System::m_ScreenCenter.Y - Config::m_fAimbotFOV))
			return;
	}

	if (GetAsyncKeyState(VK_RBUTTON))
	{
		if (Config::m_pCurrentTarget == NULL)
		{
			Config::m_pCurrentTarget = pawn;
		}
       
		
		if (Config::m_pCurrentTarget == pawn) 
		{
			//mouse_event ass nigga they call me
			Utility::MouseMove(headScreen.X, headScreen.Y, Config::System::m_ScreenSize.X, Config::System::m_ScreenSize.Y, Config::m_fAimbotSmoothness);
            
		}
		
	}
	else 
	{
		Config::m_pCurrentTarget = NULL;
	}
	

}
#include "general.h"

#include <iostream>
#include <Windows.h>

#include "../config.h"

namespace Utility
{
	void CreateConsole() 
	{
		AllocConsole();
		AttachConsole(GetCurrentProcessId());
		SetConsoleTitle(Config::System::m_cAuthor);
		FILE* f;
		freopen_s(&f, "CONOUT$", "w", stdout);
	}

	SDK::FLinearColor FromImColorToFLinearColor(ImColor color) {
		return SDK::FLinearColor(color.Value.x, color.Value.y, color.Value.z, color.Value.w);
	}

	SDK::FName StrToName(const wchar_t* str)
	{
		return SDK::UKismetStringLibrary::Conv_StringToName(SDK::FString(TEXT(str)));
	}

	void ApplyChams(SDK::USkeletalMeshComponent* mesh, bool isVisible) {
		// Debug Print 1: Function entry and initial parameters.
		std::cout << "DEBUG: [Chams] ApplyChams started. Mesh: "
			<< std::hex << mesh << std::dec
			<< ", IsVisible: " << (isVisible ? "TRUE" : "FALSE") << std::endl;

		UC::TArray<SDK::UMaterialInterface*> Mats = mesh->GetMaterials();

		// Debug Print 2: Number of materials obtained from the mesh.
		std::cout << "DEBUG: [Chams] Mesh materials count: " << Mats.Num() << std::endl;

		auto ApplyMaterialToAll = [&](SDK::UMaterialInstanceDynamic* mat)
			{
				// Debug Print 3: Starting material iteration.
				std::cout << "DEBUG: [Chams] Starting to set material "
					<< std::hex << mat << std::dec
					<< " across all slots." << std::endl;

				int appliedCount = 0;
				for (int t = 0; t < Mats.Num(); t++)
				{
					if (Mats[t])
					{
						mesh->SetMaterial(t, mat);
						appliedCount++;
						// Optional: Print index of applied material
						// std::cout << "DEBUG: [Chams] Set material at slot " << t << "." << std::endl;
					}
				}
				// Debug Print 4: Total materials applied.
				std::cout << "DEBUG: [Chams] Finished setting materials. "
					<< appliedCount << " slots updated." << std::endl;
			};

		ImColor visibilityColor = isVisible ? Config::m_cChamsColorTargetVisible : Config::m_cChamsColorTargetHidden;

		// Debug Print 5: The color value being prepared. Using std::fixed and std::setprecision for float output.
		
		// Reset precision/fixed for potential later use
		std::cout << std::defaultfloat;

		if (Config::m_pChamsMaterial) {
			// Debug Print 6: Setting vector parameter on the material.
			std::cout << "DEBUG: [Chams] Setting color parameter on Chams Material "
				<< std::hex << Config::m_pChamsMaterial << std::dec << "." << std::endl;

			Config::m_pChamsMaterial->SetVectorParameterValue(StrToName(L"Color"), FromImColorToFLinearColor(visibilityColor));
			ApplyMaterialToAll(Config::m_pChamsMaterial);
		}
		else {
			// Debug Print 7: Error if the material is missing.
			std::cout << "ERROR: [Chams] Config::m_pChamsMaterial is NULL. Cannot apply chams." << std::endl;
		}
	}

	void MouseMove(float tarx, float tary, float X, float Y, int smooth)
	{
		float ScreenCenterX = (X / 2);
		float ScreenCenterY = (Y / 2);
		float TargetX = 0;
		float TargetY = 0;

		smooth = smooth + 3;

		if (tarx != 0)
		{
			if (tarx > ScreenCenterX)
			{
				TargetX = -(ScreenCenterX - tarx);
				TargetX /= smooth;
				if (TargetX + ScreenCenterX > ScreenCenterX * 2) TargetX = 0;
			}

			if (tarx < ScreenCenterX)
			{
				TargetX = tarx - ScreenCenterX;
				TargetX /= smooth;
				if (TargetX + ScreenCenterX < 0) TargetX = 0;
			}
		}

		if (tary != 0)
		{
			if (tary > ScreenCenterY)
			{
				TargetY = -(ScreenCenterY - tary);
				TargetY /= smooth;
				if (TargetY + ScreenCenterY > ScreenCenterY * 2) TargetY = 0;
			}

			if (tary < ScreenCenterY)
			{
				TargetY = tary - ScreenCenterY;
				TargetY /= smooth;
				if (TargetY + ScreenCenterY < 0) TargetY = 0;
			}
		}
		mouse_event(MOUSEEVENTF_MOVE, static_cast<DWORD>(TargetX), static_cast<DWORD>(TargetY), NULL, NULL);
	}

	void Log(uintptr_t address, const char* className, const char* methodName) {
		std::cout << "[ LOG ] " << className << "$$" << methodName << ": " << address << "\n" << std::endl;
	}

	void LogError(const char* text, const char* name) {
		std::cout << "[ LOG ]  " << text << ":  " << name << "\n" << std::endl;
	}
}
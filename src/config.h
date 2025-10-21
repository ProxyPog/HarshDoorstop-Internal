#pragma once

#include "includes.h"

#define RAINBOW_BORDERS TRUE;

class Config 
{
public:

	class System
	{
	public:
		inline static constexpr const char* m_cAppName = "UnrealBase";
		inline static constexpr const char* m_cAuthor = "0xGoose";

		inline static bool m_bInit = false;
		inline static bool m_bUpdateTargetsInDifferentThread = false; //game is freezing
		inline static bool m_bUpdateTargets = true;

		inline static POINT m_MousePos = { 0, 0 };

		inline static UINT m_vps = 1;
		inline static SDK::FVector2D m_ScreenSize = { 0, 0 };
		inline static SDK::FVector2D m_ScreenCenter = { 0, 0 };
		inline static D3D11_VIEWPORT m_Viewport;
	};

	class Keys
	{
	public:
		inline static constexpr const int m_cShowMenuKey = VK_INSERT;
		inline static constexpr const int m_cDeAttachKey = VK_DELETE;
	};

	class Offsets // get it with https://github.com/Spuckwaffel/UEDumper maybe
	{
	public:
		//uintptr_t Health__TakeDamage_Offset = 0x0;
	};

	inline static ImFont* m_pGameFont = NULL;

	inline static enum MenuTab
	{
		TAB_VISUALS,
		TAB_AIM,
		TAB_EXPLOITS,
		TAB_MISC,
		TAB_DEV
	};

	inline static int m_nTargetFetch = 2;
	
	inline static SDK::UEngine* m_pEngine;
	inline static SDK::UWorld* m_pWorld;
	inline static SDK::APlayerController* m_pMyController;
	inline static SDK::APawn* m_pMyPawn;
	inline static SDK::ACharacter* m_pMyCharacter;

	//inline static std::vector<SDK::AActor*> TargetsList{};
	inline static std::vector<SDK::ACharacter*> m_TargetsList{};
	inline static SDK::AActor* m_pCurrentTarget;

	inline static ImColor m_TargetPlayerColor = ImColor(255, 0, 0);

	//Dumper::DumpUBones() 
	inline static std::vector<std::pair<int, int>> m_BonePairs = {
		// Spine
		{1, 2},  // pelvis -> spine_01
		{2, 3},  // spine_01 -> spine_02
		{3, 4},  // spine_02 -> spine_03
		{4, 47}, // spine_03 -> neck_01
		{47, 48}, // neck_01 -> head

		// Left arm
		{4, 5},   // spine_03 -> clavicle_l
		{5, 6},   // clavicle_l -> UpperArm_L
		{6, 7},   // UpperArm_L -> lowerarm_l
		{7, 8},   // lowerarm_l -> Hand_L
		{8, 9},   // Hand_L -> index_01_l
		{9, 10},  // index_01_l -> index_02_l
		{10, 11}, // index_02_l -> index_03_l
		{8, 12},  // Hand_L -> middle_01_l
		{12, 13}, // middle_01_l -> middle_02_l
		{13, 14}, // middle_02_l -> middle_03_l
		{8, 15},  // Hand_L -> pinky_01_l
		{15, 16}, // pinky_01_l -> pinky_02_l
		{16, 17}, // pinky_02_l -> pinky_03_l
		{8, 18},  // Hand_L -> ring_01_l
		{18, 19}, // ring_01_l -> ring_02_l
		{19, 20}, // ring_02_l -> ring_03_l
		{8, 21},  // Hand_L -> thumb_01_l
		{21, 22}, // thumb_01_l -> thumb_02_l
		{22, 23}, // thumb_02_l -> thumb_03_l

		// Right arm
		{4, 26},  // spine_03 -> clavicle_r
		{26, 27}, // clavicle_r -> UpperArm_R
		{27, 28}, // UpperArm_R -> lowerarm_r
		{28, 29}, // lowerarm_r -> Hand_R
		{29, 30}, // Hand_R -> index_01_r
		{30, 31}, // index_01_r -> index_02_r
		{31, 32}, // index_02_r -> index_03_r
		{29, 33}, // Hand_R -> middle_01_r
		{33, 34}, // middle_01_r -> middle_02_r
		{34, 35}, // middle_02_r -> middle_03_r
		{29, 36}, // Hand_R -> pinky_01_r
		{36, 37}, // pinky_01_r -> pinky_02_r
		{37, 38}, // pinky_02_r -> pinky_03_r
		{29, 39}, // Hand_R -> ring_01_r
		{39, 40}, // ring_01_r -> ring_02_r
		{40, 41}, // ring_02_r -> ring_03_r
		{29, 42}, // Hand_R -> thumb_01_r
		{42, 43}, // thumb_01_r -> thumb_02_r
		{43, 44}, // thumb_02_r -> thumb_03_r

		// Left leg
		{1, 49},  // pelvis -> Thigh_L
		{49, 50}, // Thigh_L -> calf_l
		{50, 52}, // calf_l -> Foot_L
		{52, 53}, // Foot_L -> ball_l

		// Right leg
		{1, 55},  // pelvis -> Thigh_R
		{55, 56}, // Thigh_R -> calf_r
		{56, 58}, // calf_r -> Foot_R
		{58, 59}  // Foot_R -> ball_r
	};

// Global rainbow color
	inline static ImVec4 m_v4Rainbow = ImVec4(0.0f, 0.0f, 0.0f, 0.0f); 
 // Global rainbow color
	inline static ImColor m_cRainbow = ImColor(255.0f / 255, 255.0f / 255, 255.0f / 255); 

	inline static DWORD m_nLastShotTime = 0;
	inline static DWORD m_nLastTick = 0;

	inline static bool m_bShowMenu = false;
	inline static bool m_bWatermark = false;

	inline static bool m_bCameraFovChanger = false;
	inline static float m_fCameraCustomFOV = 120.0f;
	inline static SDK::FVector NewLocationz = SDK::FVector(1000.0f, 1000.0f, 100.0f);
	inline static SDK::FVector NewLocation = SDK::FVector(1000.0f, 1000.0f, 100.0f);
	inline static bool tp = false;
	inline static bool bullettp = false;
	inline static int AimBone = 0;

	inline static bool m_bTimeScaleChanger = false;
	inline static float m_fTimeScale = 1.0f;

	inline static bool m_bEnableDeveloperOptions = false;

	inline static bool m_bShowInspector = false;

	inline static bool m_bShowMouse = true;
	inline static bool m_bRainbowMouse = false;
	inline static ImColor m_cMouseColor = ImColor(255.0f / 255, 255.0f / 255, 255.0f / 255);
	inline static int m_nMouseType = 0;

	inline static bool m_bFly = false;

	inline static bool m_bNoGravity = false;
	inline static bool setactoreyes = false;;
	inline static bool m_bCrosshair = false;
	inline static bool m_bRainbowCrosshair = false;
	inline static ImColor m_cCrosshairColor = ImColor(255.0f / 255, 255.0f / 255, 255.0f / 255);
	inline static float m_fCrosshairSize = 5.0f;
	inline static int m_nCrosshairType = 0;

	inline static bool m_bPlayersSnapline = false;
	inline static bool m_bRainbowPlayersSnapline = false;
	inline static ImColor m_cPlayersSnaplineColor = ImColor(255.0f / 255, 255.0f / 255, 255.0f / 255);
	inline static int m_nPlayersSnaplineType = 2;

	inline static bool m_bPlayerChams = false;
	// Visible targets should be BLUE (R=0, G=0, B=255)
	inline static ImColor m_cChamsColorTargetVisible = ImColor(0.0f / 255, 0.0f / 255, 255.0f / 255);

	// Invisible (Hidden) targets should be RED (R=255, G=0, B=0)
	inline static ImColor m_cChamsColorTargetHidden = ImColor(255.0f / 255, 0.0f / 255, 0.0f / 255);
	inline static bool m_bRainbowPlayerChams = false;
	inline static SDK::UMaterialInstanceDynamic* m_pChamsMaterial;

	inline static bool m_bPlayerSkeleton = true;
	inline static bool m_bRainbowPlayerSkeleton = false;
	inline static ImColor m_cPlayerSkeletonColor = ImColor(255.0f / 255, 255.0f / 255, 255.0f / 255);

	inline static bool m_bPlayersBox = true;
	inline static bool entname = true;
	inline static bool m_bRainbowPlayersBox = false;
	inline static ImColor m_cPlayersBoxColor = ImColor(255.0f / 255, 255.0f / 255, 255.0f / 255);
	inline static bool m_bPlayersBoxFilled = false;

	inline static bool m_bKillAll = false;

	inline static bool m_bRainbowAimbotTargetColor = false;
	inline static ImColor m_cAimbotTargetColor = ImColor(255.0f / 255, 255.0f / 255, 255.0f / 255);

	inline static ImColor m_cTargetNotVisibleColor = ImColor(255.0f / 255, 255.0f / 255, 255.0f / 255);
	inline static bool m_bRainbowTargetNotVisibleColor = false;
	
	inline static bool m_bPlayersBox3D = false;

	inline static bool m_bBotChecker = false;
	inline static bool m_bRainbowBotChecker = false;
	inline static ImColor m_cBotCheckerColor = ImColor(0, 0, 255);
	inline static bool m_bBotCheckerText = true;

	inline static bool m_bPlayersHealth = false;

	inline static bool m_bGodMode = false;
	inline static bool InfiniteAmmo = false;
	inline static bool m_bNoRecoil = false;

	inline static bool m_bNoSpread = false;

	inline static bool m_bRapidFire = false;

	inline static bool m_bOneShot = false;

	inline static bool m_bInfiniteAmmo = false;
	
	inline static bool m_bSpeedHack = false;
	inline static float m_fSpeedValue = 1.0f;

	inline static bool m_bNoClip = false;

	inline static bool m_bEnableAimbot = false;
	inline static bool m_bAimbotFOVCheck = false;
	inline static float m_fAimbotFOV = 80.0f;
	inline static float m_fAimbotSmoothness = 0.5f;
	inline static float m_fHeadPosOffset = 2;
	inline static float m_fFeetPosOffset = 0;

};
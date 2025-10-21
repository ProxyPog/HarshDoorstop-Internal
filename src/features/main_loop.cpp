#include "main_loop.h"


#include "../config.h"
#include "../utils/validity.h"
#include "../utils/general.h"
#include "../utils/imgui/imgui_helper.h"
#include "esp.h"
#include "aimbot.h"
std::shared_ptr<std::vector<SDK::ACharacter*>> currentTargets;
uintptr_t PatternScan(uintptr_t moduleAdress, const char* signature)
{
    static auto patternToByte = [](const char* pattern)
        {
            auto       bytes = std::vector<int>{};
            const auto start = const_cast<char*>(pattern);
            const auto end = const_cast<char*>(pattern) + strlen(pattern);

            for (auto current = start; current < end; ++current)
            {
                if (*current == '?')
                {
                    ++current;
                    bytes.push_back(-1);
                }
                else { bytes.push_back(strtoul(current, &current, 16)); }
            }
            return bytes;
        };

    const auto dosHeader = (PIMAGE_DOS_HEADER)moduleAdress;
    const auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)moduleAdress + dosHeader->e_lfanew);

    const auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
    auto       patternBytes = patternToByte(signature);
    const auto scanBytes = reinterpret_cast<std::uint8_t*>(moduleAdress);

    const auto s = patternBytes.size();
    const auto d = patternBytes.data();

    for (auto i = 0ul; i < sizeOfImage - s; ++i)
    {
        bool found = true;
        for (auto j = 0ul; j < s; ++j)
        {
            if (scanBytes[i + j] != d[j] && d[j] != -1)
            {
                found = false;
                break;
            }
        }
        if (found) { return reinterpret_cast<uintptr_t>(&scanBytes[i]); }
    }
    return NULL;
}
void ModifyInstruction(LPVOID address, BYTE* code, DWORD size)
{
    DWORD oldProtect;
    VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy(address, code, size);
    VirtualProtect(address, size, oldProtect, &oldProtect);
}
void MainLoop::DrawCrosshair() 
{
	ImColor color = Config::m_bRainbowCrosshair ? Config::m_cRainbow : Config::m_cCrosshairColor;
	switch (Config::m_nCrosshairType)
	{
	case 0:
		ImGui::GetForegroundDrawList()->AddLine(ImVec2(Config::System::m_ScreenCenter.X - Config::m_fCrosshairSize, Config::System::m_ScreenCenter.Y), ImVec2((Config::System::m_ScreenCenter.X - Config::m_fCrosshairSize) + (Config::m_fCrosshairSize * 2), Config::System::m_ScreenCenter.Y), color, 1.2f);
		ImGui::GetForegroundDrawList()->AddLine(ImVec2(Config::System::m_ScreenCenter.X, Config::System::m_ScreenCenter.Y - Config::m_fCrosshairSize), ImVec2(Config::System::m_ScreenCenter.X, (Config::System::m_ScreenCenter.Y - Config::m_fCrosshairSize) + (Config::m_fCrosshairSize * 2)), color, 1.2f);
		break;
	case 1:
		ImGui::GetForegroundDrawList()->AddCircle(ImVec2(Config::System::m_ScreenCenter.X, Config::System::m_ScreenCenter.Y), Config::m_fCrosshairSize, color, 100, 1.2f);
		break;
	}
}

void MainLoop::FetchFromObjects(std::vector<SDK::ACharacter*>* list)
{

	list->clear();

	for (int i = 0; i < SDK::UObject::GObjects->Num(); i++)
	{
		SDK::UObject* obj = SDK::UObject::GObjects->GetByIndex(i);

		if (!obj || obj->IsDefaultObject())
			continue;
		
		//if(Config::m_bKillAll) obj->damage;

		//list->push_back(npc);

	}
}

/*void MainLoop::FetchFromActors(std::vector<SDK::AActor*>* list)
{

	if (Config::World->Levels.Num() == 0)
		return;

	SDK::ULevel* currLevel = Config::World->Levels[0];
	if (!currLevel)
		return;

	list->clear();

	for (int j = 0; j < currLevel->Actors.Num(); j++)
	{
		SDK::AActor* currActor = currLevel->Actors[j];//trash each index

		if (!currActor)
			continue;
		if (!currActor->RootComponent) //?? wWHEREEEEEE
			continue;

		//const auto location = currActor->K2_GetActorLocation();
		//if (location.X == 0.f || location.Y == 0.f || location.Z == 0.f) continue;

		//if (currActor->GetFullName().find("YOUR_NPC") != std::string::npos)
		if (currActor->GetFullName().find("BP_Enemy") != std::string::npos)
		{
			list->push_back(currActor);
		}

	}
}*/

void MainLoop::FetchFromPlayers(std::vector<SDK::ACharacter*>* list)
{
    if (!list)
        return;

    list->clear();

    // Validate the world pointer
    if (!Config::m_pWorld || Validity::IsBadPoint(Config::m_pWorld))
        return;

    SDK::TSubclassOf<SDK::ACharacter> PlayerBaseCharacterReference = SDK::ACharacter::StaticClass();
    if (!PlayerBaseCharacterReference)
        return;

    SDK::TArray<SDK::AActor*> PlayerCharacters;
    SDK::UGameplayStatics::GetAllActorsOfClass(Config::m_pWorld, PlayerBaseCharacterReference, &PlayerCharacters);

    if (PlayerCharacters.Num() <= 0)
        return;

    for (SDK::AActor* actor : PlayerCharacters)
    {
        if (!actor || Validity::IsBadPoint(actor))
            continue;

        if (!actor->IsA(PlayerBaseCharacterReference))
            continue;

        SDK::ACharacter* PlayerCharacter = reinterpret_cast<SDK::ACharacter*>(actor);
        if (!PlayerCharacter || Validity::IsBadPoint(PlayerCharacter))
            continue;

        //PlayerState
        SDK::APlayerState* ps = PlayerCharacter->PlayerState;
        if (!ps || Validity::IsBadPoint(ps))
            continue;

        //PlayerCharacter->name
        auto name = ps->GetPlayerName();
        if (!name.IsValid())
            continue;

        list->push_back(PlayerCharacter);
    }
}


void MainLoop::FetchEntities()
{
	do {
		if (!Config::System::m_bUpdateTargets)
		{
			std::lock_guard<std::mutex> lock(list_mutex);
			if (!Config::m_TargetsList.empty())
			{
				Config::m_TargetsList.clear();
			}

			
			
		}

		if (!Config::m_pWorld || Validity::IsBadPoint(Config::m_pWorld) ||
			!Config::m_pEngine || Validity::IsBadPoint(Config::m_pEngine) ||
			!Config::m_pMyController || Validity::IsBadPoint(Config::m_pMyController) ||
			!Config::m_pMyPawn || Validity::IsBadPoint(Config::m_pMyPawn))
		{
			
			
		}

		if (!Config::m_pWorld->GameState || Validity::IsBadPoint(Config::m_pWorld->GameState) ||
			Validity::IsBadPoint(Config::m_pWorld->OwningGameInstance))
		{
			
		}

		std::vector<SDK::ACharacter*> newTargets;

		switch (Config::m_nTargetFetch)
		{
			case 0:
				FetchFromObjects(&newTargets);
				break;

			case 1:
				//FetchFromActors(&newTargets);
				break;

			case 2:
				FetchFromPlayers(&newTargets);
				break;
		}

		{
			std::lock_guard<std::mutex> lock(list_mutex);
			Config::m_TargetsList = std::move(newTargets);
		}

		

	// if its in a thread run it continuously
	} while (Config::System::m_bUpdateTargetsInDifferentThread);
}

bool MainLoop::UpdateSDK(bool log) 
{
	Config::m_pWorld = SDK::UWorld::GetWorld();
	if (Validity::IsBadPoint(Config::m_pWorld) || Validity::IsBadPoint(Config::m_pWorld->GameState))
	{
		std::cerr << "Error: World not found" << std::endl;
		return false;
	}
	if (log) {
		std::cout << "World address: 0x" << std::hex << reinterpret_cast<uintptr_t>(Config::m_pWorld) << std::dec << std::endl;
	}

	Config::m_pEngine = SDK::UEngine::GetEngine();
	if (Validity::IsBadPoint(Config::m_pEngine)) 
	{
		std::cerr << "Error: Engine not found" << std::endl;
		return false;
	}
	if (log) {
		std::cout << "Engine address: 0x" << std::hex << reinterpret_cast<uintptr_t>(Config::m_pEngine) << std::dec << std::endl;
	}

	// Init PlayerController
	if (Validity::IsBadPoint(Config::m_pWorld->OwningGameInstance)) 
	{
		std::cerr << "Error: OwningGameInstance not found" << std::endl;
		return false;
	}
	if (Validity::IsBadPoint(Config::m_pWorld->OwningGameInstance->LocalPlayers[0])) 
	{
		std::cerr << "Error: LocalPlayers[0] not found" << std::endl;
		return false;
	}
	Config::m_pMyController = Config::m_pWorld->OwningGameInstance->LocalPlayers[0]->PlayerController;
	if (Validity::IsBadPoint(Config::m_pMyController)) 
	{
		std::cerr << "Error: MyController not found" << std::endl;
		return false;
	}
	if (log) {
		std::cout << "PlayerController address: 0x" << std::hex << reinterpret_cast<uintptr_t>(Config::m_pMyController) << std::dec << std::endl;
	}

	// Init Pawn
	Config::m_pMyPawn = Config::m_pMyController->AcknowledgedPawn;
	if (Config::m_pMyPawn == nullptr) 
	{
		std::cerr << "Error: MyPawn not found" << std::endl;
		return false;
	}	
	if (log) {
		std::cout << "MyPawn address: 0x" << std::hex << reinterpret_cast<uintptr_t>(Config::m_pMyPawn) << std::dec << std::endl;
	}

	Config::m_pMyCharacter = Config::m_pMyController->Character;
	if (Config::m_pMyCharacter == nullptr)
	{
		std::cerr << "Error: MyCharacter not found" << std::endl;
		return false;
	}
	if (log) {
		std::cout << "MyCharacter address: 0x" << std::hex << reinterpret_cast<uintptr_t>(Config::m_pMyCharacter) << std::dec << std::endl;
	}

	return true;

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <ShellAPI.h>
#include <psapi.h>

DWORD FindProcessId(const char* processName)
{
    DWORD processIds[1024], cbNeeded, count;
    if (!EnumProcesses(processIds, sizeof(processIds), &cbNeeded))
        return 0;

    count = cbNeeded / sizeof(DWORD);
    for (DWORD i = 0; i < count; i++)
    {
        if (processIds[i] != 0)
        {
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processIds[i]);
            if (hProcess != NULL)
            {
                char szProcessName[MAX_PATH] = { 0 };
                if (GetModuleBaseNameA(hProcess, NULL, szProcessName, sizeof(szProcessName)) != 0)
                {
                    if (strcmp(szProcessName, processName) == 0)
                    {
                        CloseHandle(hProcess);
                        return processIds[i];
                    }
                }
                CloseHandle(hProcess);
            }
        }
    }
    return 0;
}
DWORD targetProcessId = FindProcessId("HarshDoorstop-Win64-Shipping.exe");
void nofog()
{
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, targetProcessId);
    HMODULE hModule = GetModuleHandle("HarshDoorstop-Win64-Shipping.exe");
    LPVOID moduleBase = (LPVOID)hModule;
    LPVOID instructionAddress = (LPVOID)((BYTE*)moduleBase + 0x12E93BB);

    // Modify the instruction
    BYTE code[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
    ModifyInstruction(instructionAddress, code, sizeof(code));
    CloseHandle(hProcess);
}
void nightON()
{
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, targetProcessId);
    HMODULE hModule = GetModuleHandle("HarshDoorstop-Win64-Shipping.exe");
    LPVOID moduleBase = (LPVOID)hModule;
    LPVOID instructionAddress = (LPVOID)((BYTE*)moduleBase + 0x253B504);

    BYTE code[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
    ModifyInstruction(instructionAddress, code, sizeof(code));
    CloseHandle(hProcess);
}
void nightOFF()
{
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, targetProcessId);
    HMODULE hModule = GetModuleHandle("HarshDoorstop-Win64-Shipping.exe");
    LPVOID moduleBase = (LPVOID)hModule;


    LPVOID instructionAddress = (LPVOID)((BYTE*)moduleBase + 0x253B504);

   
    BYTE code[] = { 0x0F, 0x28, 0x05, 0xF5, 0x56, 0xA3, 0x00 };
    ModifyInstruction(instructionAddress, code, sizeof(code));
    CloseHandle(hProcess);
}
////////////////////////////////////////////////////////////////////////////////////////




















void MainLoop::Update(DWORD tick)
{
    auto IsValidObj = [](auto ptr) -> bool { return (ptr != nullptr); };
    if (!UpdateSDK(false)) return;
    if (!Config::System::m_bUpdateTargetsInDifferentThread)
    { FetchEntities(); }

#pragma region EXPLOIT CHEATS

   
    auto myController = Config::m_pMyController;
    auto myPawn = Config::m_pMyPawn;
    auto myCharacter = Config::m_pMyCharacter;
    auto world = Config::m_pWorld;


    if (Config::m_bNoSpread)
    {

    }

    if (Config::m_bGodMode)
    {
       
        if (IsValidObj(myController))
        {
            myController->SetLifeSpan(999);//no work
        }
    }
    

    if (Config::m_bNoClip)
    {
        if (IsValidObj(myPawn))
        {
            myPawn->SetActorEnableCollision(false);
        }
    }
    else
    {
        if (IsValidObj(myPawn))
        {
            // only call GetActorEnableCollision if pawn exists
            if (myPawn->GetActorEnableCollision())
                myPawn->SetActorEnableCollision(true);
        }
    }

    // seems universal
    if (Config::m_bCameraFovChanger)
    {
        if (IsValidObj(myController))
        {
            myController->FOV(Config::m_fCameraCustomFOV);
        }
    }

    if (Config::tp)
    {
        if (IsValidObj(myPawn))
        {
            myPawn->K2_SetActorLocation(Config::NewLocation, false, nullptr, true);
        }
        Config::tp = false;
    }
    /*
    * //GEE PEE TEE
    if (Config::bullettp)
    {
        printf("\n====================[ BULLETTP ENABLED ]====================\n");

        if (!world || Validity::IsBadPoint(world))
        {
            printf("[BULLETTP] ERROR: World pointer invalid (0x%p)\n", world);
            return;
        }

        printf("[BULLETTP] World OK: 0x%p\n", world);
        SDK::ULevel* persistentLevel = world->PersistentLevel;

        if (!IsValidObj(persistentLevel))
        {
            printf("[BULLETTP] ERROR: PersistentLevel invalid (0x%p)\n", persistentLevel);
            return;
        }

        printf("[BULLETTP] PersistentLevel: 0x%p\n", persistentLevel);

        uintptr_t actorsPtr = *(uintptr_t*)((uintptr_t)persistentLevel + 0x98);
        int32_t actorCount = *(int32_t*)((uintptr_t)persistentLevel + 0xA0);
        printf("[BULLETTP] Actor array at 0x%p, count = %d\n", (void*)actorsPtr, actorCount);

        if (!actorsPtr || actorCount <= 0)
        {
            printf("[BULLETTP] ERROR: No actors found!\n");
            return;
        }

        printf("[BULLETTP] MyCharacter = 0x%p | MyController = 0x%p\n", myCharacter, myController);
        printf("[BULLETTP] Scanning %zu targets...\n", currentTargets->size());

        int totalTeleports = 0;
        int totalBulletsScanned = 0;

        for (SDK::ACharacter* currTarget : *currentTargets)
        {
            printf("\n--------------------[ NEW TARGET ]--------------------\n");

            if (!currTarget || Validity::IsBadPoint(currTarget))
            {
                printf("[BULLETTP] Skipped: invalid currTarget (0x%p)\n", currTarget);
                continue;
            }

            printf("[BULLETTP] Target: 0x%p\n", currTarget);
            if (currTarget == myCharacter)
            {
                printf("[BULLETTP] Skipped self.\n");
                continue;
            }

            printf("[BULLETTP] Mesh: 0x%p | Controller: 0x%p | HealthAddr guess: 0x%p\n",
                currTarget->Mesh, currTarget->Controller, (uintptr_t)currTarget + 0x300);

            int boneId = 48; // default head
            switch (Config::AimBone)
            {
            case 0: boneId = 48; break;
            case 1: boneId = 5;  break;
            case 2: boneId = 58; break;
            default: break;
            }
            printf("[BULLETTP] Using boneID = %d\n", boneId);

            SDK::FVector aimLocation = { 0.f, 0.f, 0.f };
            if (IsValidObj(currTarget->Mesh))
            {
                SDK::FName boneName = currTarget->Mesh->GetBoneName(boneId);
                aimLocation = currTarget->Mesh->GetSocketLocation(boneName);
                
            }
            else
            {
                printf("[BULLETTP] ERROR: Target mesh invalid!\n");
                continue;
            }

            if (aimLocation.X == 0.f && aimLocation.Y == 0.f && aimLocation.Z == 0.f)
            {
                printf("[BULLETTP] WARNING: Aim location (0,0,0) — skipping.\n");
                continue;
            }

            int bulletsTeleported = 0;
            for (int iActor = 0; iActor < actorCount; ++iActor)
            {
                SDK::AActor* actor = *(SDK::AActor**)(actorsPtr + iActor * sizeof(uintptr_t));
                if (!actor)
                {
                    if (iActor % 200 == 0) printf("[BULLETTP] Skipped null actor index %d\n", iActor);
                    continue;
                }

                totalBulletsScanned++;

                // Optional: print every 100th actor to avoid flooding
                if (iActor % 100 == 0)
                    printf("[BULLETTP] Scanning actor %d / %d at 0x%p\n", iActor, actorCount, actor);

                // Verify if this actor is a projectile
                if (actor->IsA(SDK::UProjectileMovementComponent::StaticClass()))
                {
                    printf("[BULLETTP] Potential bullet at 0x%p\n", actor);
                    auto bullet = reinterpret_cast<SDK::AActor*>(actor);
                    if (!bullet) continue;

                    auto instigator = bullet->GetInstigatorController();
                    printf("[BULLETTP] Bullet instigator: 0x%p\n", instigator);

                    if (instigator == myController)
                    {
                        printf("[BULLETTP] -> Teleporting bullet 0x%p to target (%.1f, %.1f, %.1f)\n",
                            bullet, aimLocation.X, aimLocation.Y, aimLocation.Z);

                        bullet->K2_SetActorLocation(aimLocation, false, nullptr, true);
                        bulletsTeleported++;
                    }
                    else
                    {
                        printf("[BULLETTP] Skipped bullet 0x%p not owned by us.\n", bullet);
                    }
                }
            }

            printf("[BULLETTP] Finished scanning actors for target 0x%p. Teleported %d bullets.\n",
                currTarget, bulletsTeleported);
            totalTeleports += bulletsTeleported;
        }

        printf("\n====================[ BULLETTP SUMMARY ]====================\n");
        printf("Total targets processed: %zu\n", currentTargets->size());
        printf("Total actors scanned: %d\n", totalBulletsScanned);
        printf("Total bullets teleported: %d\n", totalTeleports);
        printf("============================================================\n\n");
    }
    */



   

 
    if (Config::m_bTimeScaleChanger)
    {
        if (IsValidObj(world))
        {
            //world->K2_GetWorldSettings()
            auto worldSettings = world->K2_GetWorldSettings();
            if (IsValidObj(worldSettings))
            {
                worldSettings->TimeDilation = Config::m_fTimeScale;
            }
        }
    }


    static bool wasCPressed = false;

    if (GetAsyncKeyState('C') & 0x8000)
    {
        if (!wasCPressed)
        {
            Config::m_bFly = !Config::m_bFly;
            wasCPressed = true;
        }
    }
    else
    {
        wasCPressed = false;
    }


    if (Config::m_bFly)
    {
        if (IsValidObj(myCharacter) && IsValidObj(myCharacter->CharacterMovement))
        {
            auto movement = myCharacter->CharacterMovement;
            movement->MaxFlySpeed = 20000.f;
            movement->MovementMode = SDK::EMovementMode::MOVE_Flying;

            if (GetAsyncKeyState(VK_SPACE) & 0x8000)
            {
                SDK::FVector posUP = { 0.f, 0.f, 10.f };
                movement->AddInputVector(posUP, true);
            }
            if (GetAsyncKeyState(VK_LCONTROL) & 0x8000)
            {
                SDK::FVector posDOWN = { 0.f, 0.f, -10.f };
                movement->AddInputVector(posDOWN, true);
            }
        }
    }
    else
    {
        if (IsValidObj(myCharacter) &&
            IsValidObj(myCharacter->CharacterMovement) &&
            myCharacter->CharacterMovement->MovementMode == SDK::EMovementMode::MOVE_Flying)
        {
            myCharacter->CharacterMovement->MovementMode = SDK::EMovementMode::MOVE_Falling;
        }
    }

    if (Config::setactoreyes)
    {
        const float RAD_TO_DEG = 180.0f / 3.14159265358979323846f;

        auto DoLookAt = [&](const SDK::FVector& fromLoc, const SDK::FRotator& fromRot) -> SDK::FRotator {
            SDK::FVector dir = {
                Config::NewLocation.X - fromLoc.X,
                Config::NewLocation.Y - fromLoc.Y,
                Config::NewLocation.Z - fromLoc.Z
            };

            float hyp = sqrtf(dir.X * dir.X + dir.Y * dir.Y);
            SDK::FRotator lookAt = { 0.f, 0.f, 0.f };

            if (hyp > 1e-6f)
            {
                lookAt.Pitch = atan2f(dir.Z, hyp) * RAD_TO_DEG;
                lookAt.Yaw = atan2f(dir.Y, dir.X) * RAD_TO_DEG;
            }
            else
            {
                lookAt.Pitch = (dir.Z > 0.f) ? 90.f : -90.f;
                lookAt.Yaw = fromRot.Yaw;
            }
            lookAt.Roll = 0.f;

            // Optionally clamp pitch to avoid flipping weirdness
            //lookAt.Pitch = FMath::Clamp(lookAt.Pitch, -89.9f, 89.9f);

            return lookAt;
            };

        if (IsValidObj(myController))
        {
            SDK::FVector eyeLocation = { 0.f, 0.f, 0.f };
            SDK::FRotator eyeRotation = { 0.f, 0.f, 0.f };

   
            myController->GetActorEyesViewPoint(&eyeLocation, &eyeRotation);

            SDK::FRotator targetRot = DoLookAt(eyeLocation, eyeRotation);


            myController->SetControlRotation(targetRot);

            if (IsValidObj(myPawn))
            {

                myPawn->K2_SetActorRotation(targetRot, false);
            }
        }
        else if (IsValidObj(myPawn))
        {
            SDK::FVector pawnLoc = myPawn->K2_GetActorLocation();
 
            SDK::FRotator pawnRot = myPawn->K2_GetActorRotation();

            SDK::FRotator targetRot = DoLookAt(pawnLoc, pawnRot);

            myPawn->K2_SetActorRotation(targetRot, false);
        }

        // Config::setactoreyes = false;
    }




    if (Config::m_bNoGravity)
    {
        if (IsValidObj(myCharacter) && IsValidObj(myCharacter->CharacterMovement))
        {
            myCharacter->CharacterMovement->GravityScale = 0.2f;
        }
    }
    else
    {
        if (IsValidObj(myCharacter) && IsValidObj(myCharacter->CharacterMovement) && myCharacter->CharacterMovement->GravityScale != 1.f)
        {
            myCharacter->CharacterMovement->GravityScale = 1.f;
        }
    }

    if (Config::m_bSpeedHack)
    {
        if (IsValidObj(myCharacter) && IsValidObj(myCharacter->CharacterMovement))
        {
            myCharacter->CharacterMovement->MaxWalkSpeed = Config::m_fSpeedValue;
            myCharacter->CharacterMovement->MaxAcceleration = Config::m_fSpeedValue;
        }
    }

#pragma endregion

    

    {
        std::lock_guard<std::mutex> lock(list_mutex);
        currentTargets = std::make_shared<std::vector<SDK::ACharacter*>>(Config::m_TargetsList);
    }

    
    if (!currentTargets || currentTargets->empty())
        return;

    for (SDK::ACharacter* currTarget : *currentTargets)
    {
        if (!currTarget || Validity::IsBadPoint(currTarget))
            continue;

      
        if (currTarget->Controller && currTarget->Controller->IsLocalPlayerController())
            continue;

        bool isVisible = false;

       
        if (myController )
        {
            auto camMgr = myController->PlayerCameraManager;
            if (camMgr && IsValidObj(camMgr))
            {
           
                isVisible = myController->LineOfSightTo(
                    currTarget,
                    camMgr->CameraCachePrivate.POV.Location,
                    false
                );
            }
            else
            {
               
                isVisible = false;
            }
        }

        if (Config::m_bPlayerChams && Config::m_pChamsMaterial)
        {
            std::cout << "DEBUG: Chams enabled for target at address: "
                << std::hex << currTarget << std::dec << std::endl;

          
            SDK::ASkeletalMeshActor* mesh = reinterpret_cast<SDK::ASkeletalMeshActor*>(currTarget);

            if (mesh && IsValidObj(mesh))
            {
                std::cout << "DEBUG: Cast to ASkeletalMeshActor successful. Mesh address: "
                    << std::hex << mesh << std::dec << std::endl;

               
                if (mesh->SkeletalMeshComponent && IsValidObj(mesh->SkeletalMeshComponent))
                {
                    std::cout << "DEBUG: Applying Chams. Mesh component address: "
                        << std::hex << mesh->SkeletalMeshComponent << std::dec << std::endl;

                    Utility::ApplyChams(mesh->SkeletalMeshComponent, true);
                }
                else
                {
                    std::cout << "DEBUG: SkeletalMeshComponent is NULL or invalid." << std::endl;
                }
            }
            else
            {
                std::cout << "DEBUG: Cast to ASkeletalMeshActor FAILED or invalid target." << std::endl;
            }
        }
        else
        {
            // std::cout << "DEBUG: Chams not applied (Feature disabled or Material missing)." << std::endl;
        }

        ImColor color(255, 255, 255);
    

#pragma region CHEATS FOR TARGETS
        auto IsValidObj = [](auto ptr) -> bool { return (ptr != nullptr); };

      
        if (!IsValidObj(currTarget) || Validity::IsBadPoint(currTarget))
            break; 

        bool isCurrentTarget = (Config::m_pCurrentTarget && currTarget == Config::m_pCurrentTarget);

        if (Config::m_bPlayersSnapline)
        {
            if (isCurrentTarget)
            {
                color = Config::m_bRainbowAimbotTargetColor ? Config::m_cRainbow : Config::m_cAimbotTargetColor;
            }
            else
            {
                if (isVisible)
                    color = Config::m_bRainbowPlayersSnapline ? Config::m_cRainbow : Config::m_cPlayersSnaplineColor;
                else
                    color = Config::m_bRainbowTargetNotVisibleColor ? Config::m_cRainbow : Config::m_cTargetNotVisibleColor;
            }

            if (IsValidObj(currTarget))
                ESP::GetInstance().RenderSnapline(currTarget, color);
        }

        // SKELETON
        if (Config::m_bPlayerSkeleton)
        {
            if (isCurrentTarget)
            {
                color = Config::m_bRainbowAimbotTargetColor ? Config::m_cRainbow : Config::m_cAimbotTargetColor;
            }
            else
            {
                if (isVisible)
                    color = Config::m_bRainbowPlayerSkeleton ? Config::m_cRainbow : Config::m_cPlayerSkeletonColor;
                else
                    color = Config::m_bRainbowTargetNotVisibleColor ? Config::m_cRainbow : Config::m_cTargetNotVisibleColor;
            }

            if (IsValidObj(currTarget))
                ESP::GetInstance().RenderSkeleton(currTarget, color);
        }

        // 2D BOX
        if (Config::m_bPlayersBox)
        {
            if (isCurrentTarget)
            {
                color = Config::m_bRainbowAimbotTargetColor ? Config::m_cRainbow : Config::m_cAimbotTargetColor;
            }
            else
            {
                if (isVisible)
                    color = Config::m_bRainbowPlayersBox ? Config::m_cRainbow : Config::m_cPlayersBoxColor;
                else
                    color = Config::m_bRainbowTargetNotVisibleColor ? Config::m_cRainbow : Config::m_cTargetNotVisibleColor;
            }

            if (IsValidObj(currTarget))
                ESP::GetInstance().RenderBox(currTarget, color);
        }
        /////////////////////////////////


        if (Config::entname)
        {

            if (isCurrentTarget)
            {
                color = Config::m_bRainbowAimbotTargetColor ? Config::m_cRainbow : Config::m_cAimbotTargetColor;
            }
            else
            {
                if (isVisible)
                    color = ImColor(255.0f, 0.0f, 0.0f);
                else
                    color = ImColor(0.0f, 0.0f, 0.0f);
            }


            SDK::APlayerState* ps = currTarget->PlayerState;
            if (!ps || Validity::IsBadPoint(ps))
                continue;
           
            SDK::FString playerName = ps->GetPlayerName();
            if (!playerName.IsValid())
                continue;


            std::string wName = playerName.ToString();

            ESP::GetInstance().RenderText(currTarget, wName, color);

        }



        ////////////////////////////////////

        // 3D BOX
        if (Config::m_bPlayersBox3D)
        {
            if (isCurrentTarget)
            {
                color = Config::m_bRainbowAimbotTargetColor ? Config::m_cRainbow : Config::m_cAimbotTargetColor;
            }
            else
            {
                if (isVisible)
                    color = Config::m_bRainbowPlayersBox ? Config::m_cRainbow : Config::m_cPlayersBoxColor;
                else
                    color = Config::m_bRainbowTargetNotVisibleColor ? Config::m_cRainbow : Config::m_cTargetNotVisibleColor;
            }

            if (IsValidObj(currTarget))
                ESP::GetInstance().Render3DBox(currTarget, color);
        }

        // AIMBOT
        if (Config::m_bEnableAimbot && isVisible)
        {
            if (IsValidObj(currTarget))
                Aimbot::GetInstance().RegularAimbot(currTarget);
        }

#pragma endregion
    }
}

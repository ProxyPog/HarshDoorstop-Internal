#include "dumper.h"

#include "../config.h"
#include "validity.h"

namespace Dumper {

    void DumpUObjects()
    {
        std::cout << "\n*********************" << std::endl;
        std::cout << "***DUMPING OBJECTS***" << std::endl;

        std::cout << Config::m_pEngine->ConsoleClass->GetFullName() << std::endl;

        for (int i = 0; i < SDK::UObject::GObjects->Num(); i++)
        {
            const SDK::UObject* Obj = SDK::UObject::GObjects->GetByIndex(i);

            if (!Obj)
                continue;

            if (!Obj->IsDefaultObject())
                continue;

            if (Obj->IsA(SDK::APawn::StaticClass()) || Obj->HasTypeFlag(SDK::EClassCastFlags::Pawn))
            {
                std::cout << Obj->GetFullName() << "\n";
            }
        }

        std::cout << "***END DUMPING OBJECTS***" << std::endl;
        std::cout << "*************************\n" << std::endl;
    }

    void DumpUActors()
    {
        std::cout << "\n********************" << std::endl;
        std::cout << "***DUMPING ACTORS***" << std::endl;

        if (Config::m_pWorld->Levels.Num() == 0)
            return;

        SDK::ULevel* currLevel = Config::m_pWorld->Levels[0];
        if (!currLevel)
            return;

        for (int j = 0; j < currLevel->Actors.Num(); j++)
        {
            SDK::AActor* currActor = currLevel->Actors[j];

            if (!currActor)
                continue;
            if (!currActor->RootComponent)
                continue;

            std::cout << currActor->GetFullName() << "\n";
        }

        std::cout << "***END DUMPING ACTORS***" << std::endl;
        std::cout << "************************\n" << std::endl;
    }

    void DumpUPlayers()
    {
        std::cout << "\n********************" << std::endl;
        std::cout << "***DUMPING PLAYERS***" << std::endl;

        SDK::TSubclassOf<SDK::ACharacter> PlayerBaseCharacterReference = SDK::ACharacter::StaticClass();
        SDK::TArray<SDK::AActor*> PlayerCharacters;
        SDK::UGameplayStatics::GetAllActorsOfClass(Config::m_pWorld, PlayerBaseCharacterReference, &PlayerCharacters);

        for (SDK::AActor* actor : PlayerCharacters)
        {
            if (!actor || Validity::IsBadPoint(actor) || !actor->IsA(PlayerBaseCharacterReference)) continue;

            SDK::ACharacter* PlayerCharacter = reinterpret_cast<SDK::ACharacter*>(actor);
            if (!PlayerCharacter || !PlayerCharacter->PlayerState || !PlayerCharacter->PlayerState->GetPlayerName() || !PlayerCharacter->PlayerState->GetPlayerName().IsValid())
                continue;

            std::cout << PlayerCharacter->PlayerState->GetPlayerName().ToString() << "\n";
        }

        std::cout << "***END DUMPING PLAYERS***" << std::endl;
        std::cout << "************************\n" << std::endl;
    }

    void DumpUBones()
    {
        if (!Config::m_pMyController) return;

        auto* target = Config::m_pMyController->Character;

        //auto* target = Config::TargetsList[0];

        if (!target || !target->Mesh) {
            std::cout << "MyController or Character or Mesh is null" << std::endl;
			return;
        }

        std::cout << "\n********************" << std::endl;
        std::cout << "***DUMPING BONES***" << std::endl;


        for (int i = 0; i < 300; i++)
        {
            std::cout << "Index: " << i << " - Name: " << Config::m_pMyController->Character->Mesh->GetSocketBoneName(Config::m_pMyController->Character->Mesh->GetBoneName(i)).GetRawString() << std::endl;
        }

        std::cout << "***END DUMPING BONES***" << std::endl;
        std::cout << "************************\n" << std::endl;
    }

    void DumpTests()
    {
        std::cout << "\n********************" << std::endl;
        std::cout << "***DUMPING TEST***" << std::endl;

        for (SDK::APlayerState* player : Config::m_pWorld->GameState->PlayerArray)
        {
            if (!player || Validity::IsBadPoint(player)) continue;

            SDK::ACharacter* PlayerCharacter = reinterpret_cast<SDK::ACharacter*>(player);
            if (!PlayerCharacter || !PlayerCharacter->PlayerState || !PlayerCharacter->PlayerState->GetPlayerName() || !PlayerCharacter->PlayerState->GetPlayerName().IsValid())
                continue;

            std::cout << PlayerCharacter->PlayerState->GetPlayerName().ToString() << "\n";
        }

        std::cout << "***END DUMPING TEST***" << std::endl;
        std::cout << "************************\n" << std::endl;
    }
}
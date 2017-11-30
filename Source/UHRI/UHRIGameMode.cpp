// Fill out your copyright notice in the Description page of Project Settings.

#include "UHRIGameMode.h"
#include "TimerManager.h"
#include "UHRICharacter.h"
#include "UHRIGameInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "GameLiftServerSDK.h"



AUHRIGameMode::AUHRIGameMode()
	: Super()
{

	UE_LOG(LogTemp, Warning, TEXT("UHRI Game Mode Constructor"));

	//Let's run this code only if GAMELIFT is enabled. Only with Server targets!
#if WITH_GAMELIFT

	//Getting the module first.
	FGameLiftServerSDKModule* GameLiftSdkModule = &FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>(FName("GameLiftServerSDK"));

	if (GameLiftSdkModule != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("gamelift seen"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("gamelift NOT seen"));
	}
	//InitSDK establishes a local connection with GameLift's agent to enable further communication.
	GameLiftSdkModule->InitSDK();

	//When a game session is created, GameLift sends an activation request to the game server and passes along the game session object containing game properties and other settings.
	//Here is where a game server should take action based on the game session object.
	//Once the game server is ready to receive incoming player connections, it should invoke GameLiftServerAPI.ActivateGameSession()
	auto onGameSession = [=](Aws::GameLift::Server::Model::GameSession gameSession)
	{
		GameLiftSdkModule->ActivateGameSession();
	};

	FProcessParameters* params = new FProcessParameters();
	params->OnStartGameSession.BindLambda(onGameSession);

	//OnProcessTerminate callback. GameLift invokes this callback before shutting down an instance hosting this game server.
	//It gives this game server a chance to save its state, communicate with services, etc., before being shut down.
	//In this case, we simply tell GameLift we are indeed going to shut down.
	params->OnTerminate.BindLambda([=]() {GameLiftSdkModule->ProcessEnding(); });

	//This is the HealthCheck callback.
	//GameLift invokes this callback every 60 seconds or so.
	//Here, a game server might want to check the health of dependencies and such.
	//Simply return true if healthy, false otherwise.
	//The game server has 60 seconds to respond with its health status. GameLift defaults to 'false' if the game server doesn't respond in time.
	//In this case, we're always healthy!
	params->OnHealthCheck.BindLambda([]() {return true; });

	//This game server tells GameLift that it listens on port 7777 for incoming player connections.
	params->port = 7777;

	//Here, the game server tells GameLift what set of files to upload when the game session ends.
	//GameLift uploads everything specified here for the developers to fetch later.
	TArray<FString> logfiles;
	logfiles.Add(TEXT("LogFile.txt"));
	params->logParameters = logfiles;

	//Calling ProcessReady tells GameLift this game server is ready to receive incoming game sessions!
	GameLiftSdkModule->ProcessReady(*params);
#endif

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	UE_LOG(LogTemp, Warning, TEXT("added default pawn"));
}

void AUHRIGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	++NumberOfPlayers;

	UE_LOG(LogTemp, Warning, TEXT("Added player, %d"), NumberOfPlayers);

	/*if (NumberOfPlayers >= 2)
	{
	GetWorldTimerManager().SetTimer(GameStartTimer, this, &AUHRIGameMode::StartGame, 6);
	}*/
}
void AUHRIGameMode::StartGame()
{/*
 auto GameInstance = Cast<UUHRIGameInstance>(GetGameInstance());
 if (GameInstance == nullptr) return;

 GameInstance->StartSession();
 */
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	bUseSeamlessTravel = true;
	World->ServerTravel("/Game/Maps/FirstPersonExampleMap?listen");

}
void AUHRIGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	--NumberOfPlayers;
}


// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "EngineUtils.h"
#include "MenuSystem/MenuInterface.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionInterface.h"
#include "UHRIGameInstance.generated.h"

/**
*
*/
UCLASS()
class UHRI_API UUHRIGameInstance : public UGameInstance, public IMenuInterface
{
	GENERATED_BODY()
	
public:
	UUHRIGameInstance(const FObjectInitializer & ObjectInitializer);

	virtual void Init();


	/*UPROPERTY()
	class UGameLiftClientObject* GameLiftClientObject;*/

	//// Create Game Session ///////////////////////////////////////////////////
	//void CreateGameSession();
	//UFUNCTION()
	//	void OnGameCreationSuccess(const FString& GameSessionID);
	//UFUNCTION()
	//	void OnGameCreationFailed(const FString& ErrorMessage);

	//// Describe Game Session /////////////////////////////////////////////////
	//void DescribeGameSession(const FString& GameSessionID);
	//UFUNCTION()
	//	void OnDescribeGameSessionSuccess(const FString& SessionID, EGameLiftGameSessionStatus SessionState);
	//UFUNCTION()
	//	void OnDescribeGameSessionFailed(const FString& ErrorMessage);

	//// Create Player Session /////////////////////////////////////////////////
	//void CreatePlayerSession(const FString& GameSessionID, const FString UniquePlayerID);
	//UFUNCTION()
	//	void OnPlayerSessionCreateSuccess(const FString& IPAddress, const FString& Port, const FString& PlayerSessionID);
	//UFUNCTION()
	//	void OnPlayerSessionCreateFail(const FString& ErrorMessage);



	UFUNCTION(BlueprintCallable)
		void LoadMenuWidget();

	UFUNCTION(BlueprintCallable)
		void InGameLoadMenu();

	UFUNCTION(Exec)
		void Host(FString ServerName) override;

	UFUNCTION(Exec)
		void Join(uint32 Index) override;

	void StartSession();

	virtual void LoadMainMenu() override;
	virtual void RequestServerList() override;

private:

	TSubclassOf<class UUserWidget> MenuClass;
	TSubclassOf<class UUserWidget> InGameMenuClass;

	class UMainMenu* Menu;

	IOnlineSessionPtr SessionInterface;
	IOnlineSubsystem* Subsystem;

	void OnCreateSessionComplete(FName SessionName, bool Success);
	void OnDestroySessionComplete(FName SessionName, bool Success);
	void OnFindSessionsComplete(bool Success);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	FString DesiredServerName;

	void CreateSession();

	TSharedPtr<class FOnlineSessionSearch> SessionSearch;
};


// Fill out your copyright notice in the Description page of Project Settings.C:\Users\49176\Documents\Unreal Projects\UE4_CoopPuzzleGame-master\CoopPuzzleGame\Config\DefaultEngine.ini

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "GenericPlatform/GenericPlatform.h"

#include "SoundStageGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class SOUNDSTAGE_API USoundStageGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	virtual void Init() override;

private:

	// Session
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

protected:

	UFUNCTION(BlueprintCallable, Category = "SteamConnection")
		void Host(FString ServerName);

	UFUNCTION(BlueprintCallable, Category = "SteamConnection")
		void JoinSession(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "SteamConnection")
		void FindSessions();

	UFUNCTION(BlueprintCallable, Category = "SteamConnection")
		void EndSession();

	UFUNCTION(BlueprintImplementableEvent)
		void BIE_OnFindSession(int idx);

private:


	// Session Events
	FString DesiredServerName;
	void OnCreateSessionComplete(FName SessionName, bool Success);
	void OnDestroySessionComplete(FName SessionName, bool Success);
	void OnFindSessionsComplete(bool Success);
	void OnJoinSessionsComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	void CreateSession();
};

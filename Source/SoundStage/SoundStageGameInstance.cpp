// Fill out your copyright notice in the Description page of Project Settings.


#include "SoundStageGameInstance.h"


#include "OnlineSessionSettings.h"
#include "OnlineSubsystemTypes.h"
#include "GenericPlatform/GenericPlatform.h"

#include "Templates/SharedPointer.h"

const static FName SESSION_NAME = TEXT("SoundStageGameSession");
const static FName SERVER_NAME_SETTINGS_KEY = TEXT("SoundStageServerName");

void USoundStageGameInstance::Init() {

	UE_LOG(LogTemp, Warning, TEXT("[SoundStage::Init] Init"));

	IOnlineSubsystem* SubSystem = IOnlineSubsystem::Get();

	if (SubSystem != nullptr)
	{
		SessionInterface = SubSystem->GetSessionInterface();

		//UE_LOG(LogTemp, Warning, TEXT("[SoundStage::Init] Found session interface"));

		if (SessionInterface.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("[SoundStage::Init] SessionInterface.IsValid"));
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &USoundStageGameInstance::OnCreateSessionComplete);

			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &USoundStageGameInstance::OnDestroySessionComplete);

			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &USoundStageGameInstance::OnFindSessionsComplete);

			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &USoundStageGameInstance::OnJoinSessionsComplete);
		}



	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoundStage::Init] Found NO SUBSYSTEM"));
	}
}


void USoundStageGameInstance::Host(FString ServerName)
{
	DesiredServerName = ServerName;

	if (SessionInterface.IsValid())
	{
		// Checks for an existing session
		auto ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);

		if (ExistingSession != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("[SoundStage::Host] There is an existing session about to remove the current one"));

			SessionInterface->DestroySession(SESSION_NAME);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[SoundStage::Host] About to create session"));

			// Create a new session
			CreateSession();

		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoundStage::Host] SessionInterface invalid"));
	}
}

void USoundStageGameInstance::JoinSession(int32 Index)
{
	if (!SessionInterface.IsValid() || (!SessionSearch.IsValid())) return;

	if (Index < (int32)(SessionSearch->SearchResults.Num()))
	{
		SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[Index]);
	}
}

void USoundStageGameInstance::FindSessions()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());

	if (SessionSearch.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[UCoopPuzzleGameInstance::OpenSessionListMenu] Session is valid"));
		//SessionSearch->bIsLanQuery = true;
		SessionSearch->MaxSearchResults = 100;
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	}
}

void USoundStageGameInstance::EndSession()
{

}




void USoundStageGameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{
	if (!Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoundStage::OnCreateSessionComplete] UNSUCESS"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[SoundStage::OnCreateSessionComplete] SUCESS SessionName: %s"), *SessionName.ToString());

	//// Teardown Menu and change levels
	//if (MainMenu != nullptr)
	//{
	//	MainMenu->Teardown();
	//}

	UEngine* Engine = GetEngine();

	if (Engine == nullptr) return;

	Engine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("[SoundStage::Host]"));

	UE_LOG(LogTemp, Warning, TEXT("[SoundStage::OnCreateSessionComplete] HOST TRAVEL TO LOBBY"));

	UWorld * World = GetWorld();

	if (World == nullptr) return;

	//bUseSeamlessTravel = true;
	World->ServerTravel("C:/SoundStage/Content/Maps/DefaultMap?listen");
}

void USoundStageGameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
	if (Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoundStage::OnDestroySessionComplete] Success "));
		CreateSession();
	}
	else
	{


		UE_LOG(LogTemp, Warning, TEXT("[SoundStage::OnDestroySessionComplete] NO Success "));
	}
}

void USoundStageGameInstance::OnFindSessionsComplete(bool Success)
{
	if (Success && SessionSearch.IsValid())
	{
		if (SessionSearch->SearchResults.Num() <= 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("[SoundStage::OnFindSessionsComplete] No Sessions Find"));
		}
		else
		{
			//TArray<FServerData> ServerData;
			int idx = 0;
			for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
			{
				UE_LOG(LogTemp, Warning, TEXT("[SoundStage::OnFindSessionsComplete] Session Name %s"), *SearchResult.GetSessionIdStr());

				//FServerData Data;
				FString ServerName;
				if (SearchResult.Session.SessionSettings.Get(SERVER_NAME_SETTINGS_KEY, ServerName))
				{
					UE_LOG(LogTemp, Warning, TEXT("[SoundStage::OnFindSessionsComplete] Data found in settings ServerName: %s,   PlayerName: %s   "), *ServerName, *SearchResult.Session.OwningUserName);
					//Data.Name = ServerName;

					//Provide the ID for connection
					BIE_OnFindSession(idx);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("[SoundStage::OnFindSessionsComplete] Data NOT found in settings"));

					//Data.Name = "Could not find name";
				}

				//Data.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;
				//Data.CurrentPlayers = Data.MaxPlayers - SearchResult.Session.NumOpenPublicConnections;
				//Data.HostUsername = SearchResult.Session.OwningUserName;

				//ServerData.Add(Data);

				idx++;
			}


			//MainMenu->InitializeSessionsList(ServerData);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoundStage::OnFindSessionsComplete] Error session not found"));
	}
}

void USoundStageGameInstance::OnJoinSessionsComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogTemp, Warning, TEXT("[SoundStage::OnJoinSessionsComplete]"));

	//if (MainMenu != nullptr)
	//{
	//	MainMenu->Teardown();
	//}
	if (!SessionInterface.IsValid()) return;

	FString Url;
	if (!SessionInterface->GetResolvedConnectString(SESSION_NAME, Url))
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoundStage::OnJoinSessionsComplete] Couldn't get Connect String"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[SoundStage::OnJoinSessionsComplete] Url: %s"), *Url);

	APlayerController* PlayerController = GetFirstLocalPlayerController();

	if (PlayerController == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoundStage::OnJoinSessionsComplete] Player Controller DOESN'T EXIST"));
		return;
	}


	PlayerController->ClientTravel(Url, ETravelType::TRAVEL_Absolute);
}

void USoundStageGameInstance::CreateSession()
{
	UE_LOG(LogTemp, Warning, TEXT("[SoundStage::CreateSession] Creating %s"), *SESSION_NAME.ToString());

	if (SessionInterface.IsValid())
	{
		FOnlineSessionSettings SessionSettings;

		// Switch between bIsLANMatch when using NULL subsystem
		//if (IOnlineSubsystem::Get()->GetSubsystemName().ToString() == "NULL")
		//{
		//	SessionSettings.bIsLANMatch = true;
		//}
		//else
		//{
		//	SessionSettings.bIsLANMatch = false;
		//}

		SessionSettings.bIsLANMatch = false;

		// Number of sessions
		SessionSettings.NumPublicConnections = 2;
		SessionSettings.bShouldAdvertise = true;
		SessionSettings.bUsesPresence = true;
		SessionSettings.Set(SERVER_NAME_SETTINGS_KEY, DesiredServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

		SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
	}
}

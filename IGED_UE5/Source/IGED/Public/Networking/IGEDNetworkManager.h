// IGEDNetworkManager.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "IGEDNetworkManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnConnected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDisconnected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDataReceived, const FString&, Data);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnJSONReceived, const TSharedPtr<FJsonObject>&, JsonObject);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHTTPResponseReceived, int32, ResponseCode, const FString&, ResponseString);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHTTPRequestFailed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClientConnected, const FString&, ClientAddress);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class IGED_API UIGEDNetworkManager : public UActorComponent
{
    GENERATED_BODY()

public:
    UIGEDNetworkManager();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // TCP Functions
    UFUNCTION(BlueprintCallable, Category = "IGED|Network")
    bool ConnectToServer(const FString& ServerAddress, int32 Port);

    UFUNCTION(BlueprintCallable, Category = "IGED|Network")
    void Disconnect();

    UFUNCTION(BlueprintCallable, Category = "IGED|Network")
    bool SendData(const FString& Data);

    UFUNCTION(BlueprintCallable, Category = "IGED|Network")
    bool SendJSON(const TSharedPtr<FJsonObject>& JsonObject);

    UFUNCTION(BlueprintCallable, Category = "IGED|Network")
    bool IsConnected() const;

    UFUNCTION(BlueprintCallable, Category = "IGED|Network")
    void SetAutoReconnect(bool bEnable);

    // HTTP Functions
    UFUNCTION(BlueprintCallable, Category = "IGED|Network")
    void SendHTTPRequest(const FString& URL, const FString& Verb, const FString& Content);

    // Server Functions
    UFUNCTION(BlueprintCallable, Category = "IGED|Network")
    void StartTCPListener(int32 Port);

    UFUNCTION(BlueprintCallable, Category = "IGED|Network")
    void BroadcastToClients(const FString& Message);

    // Events
    UPROPERTY(BlueprintAssignable, Category = "IGED|Network")
    FOnConnected OnConnected;

    UPROPERTY(BlueprintAssignable, Category = "IGED|Network")
    FOnDisconnected OnDisconnected;

    UPROPERTY(BlueprintAssignable, Category = "IGED|Network")
    FOnDataReceived OnDataReceived;

    UPROPERTY(BlueprintAssignable, Category = "IGED|Network")
    FOnJSONReceived OnJSONReceived;

    UPROPERTY(BlueprintAssignable, Category = "IGED|Network")
    FOnHTTPResponseReceived OnHTTPResponseReceived;

    UPROPERTY(BlueprintAssignable, Category = "IGED|Network")
    FOnHTTPRequestFailed OnHTTPRequestFailed;

    UPROPERTY(BlueprintAssignable, Category = "IGED|Network")
    FOnClientConnected OnClientConnected;

protected:
    void ReceiveData();
    void AcceptConnections();

private:
    class FSocket* TCPSocket;
    class FSocket* ListenerSocket;
    TArray<class FSocket*> ClientConnections;
    class ISocketSubsystem* SocketSubsystem;
    FHttpModule* HttpModule;
    
    FString LastServerAddress;
    int32 ServerPort;
    bool bIsConnected;
    bool bAutoReconnect;
    float ReconnectDelay;
    float ReconnectTimer;
    
    FTimerHandle AcceptTimer;
};
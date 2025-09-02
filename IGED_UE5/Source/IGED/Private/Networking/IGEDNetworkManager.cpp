// IGEDNetworkManager.cpp

#include "Networking/IGEDNetworkManager.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Networking.h"
#include "IPAddress.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "IGED.h"

UIGEDNetworkManager::UIGEDNetworkManager()
{
    PrimaryComponentTick.bCanEverTick = true;
    ServerPort = 8080;
    bIsConnected = false;
    bAutoReconnect = true;
    ReconnectDelay = 5.0f;
}

void UIGEDNetworkManager::BeginPlay()
{
    Super::BeginPlay();
    
    HttpModule = &FHttpModule::Get();
    
    // Initialize socket subsystem
    SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
    
    UE_LOG(LogIGED, Warning, TEXT("Network Manager initialized"));
}

void UIGEDNetworkManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Disconnect();
    
    Super::EndPlay(EndPlayReason);
}

void UIGEDNetworkManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    // Process incoming network data
    if (bIsConnected && TCPSocket)
    {
        ReceiveData();
    }
    
    // Handle reconnection
    if (!bIsConnected && bAutoReconnect)
    {
        ReconnectTimer += DeltaTime;
        if (ReconnectTimer >= ReconnectDelay)
        {
            ReconnectTimer = 0.0f;
            ConnectToServer(LastServerAddress, ServerPort);
        }
    }
}

bool UIGEDNetworkManager::ConnectToServer(const FString& ServerAddress, int32 Port)
{
    if (bIsConnected)
    {
        UE_LOG(LogIGED, Warning, TEXT("Already connected to server"));
        return true;
    }
    
    LastServerAddress = ServerAddress;
    ServerPort = Port;
    
    // Create socket
    TCPSocket = SocketSubsystem->CreateSocket(NAME_Stream, TEXT("IGED_TCP"), false);
    
    if (!TCPSocket)
    {
        UE_LOG(LogIGED, Error, TEXT("Failed to create socket"));
        return false;
    }
    
    // Set socket options
    TCPSocket->SetNonBlocking(true);
    TCPSocket->SetReuseAddr(true);
    
    // Parse IP address
    FIPv4Address IPAddress;
    if (!FIPv4Address::Parse(ServerAddress, IPAddress))
    {
        // Try to resolve hostname
        ISocketSubsystem* SocketSub = ISocketSubsystem::Get();
        FAddressInfoResult GAIResult = SocketSub->GetAddressInfo(*ServerAddress, nullptr, EAddressInfoFlags::Default, NAME_None);
        
        if (GAIResult.Results.Num() > 0)
        {
            TSharedRef<FInternetAddr> Addr = GAIResult.Results[0].Address;
            uint32 IP;
            Addr->GetIp(IP);
            IPAddress = FIPv4Address(IP);
        }
        else
        {
            UE_LOG(LogIGED, Error, TEXT("Failed to resolve hostname: %s"), *ServerAddress);
            TCPSocket->Close();
            SocketSubsystem->DestroySocket(TCPSocket);
            TCPSocket = nullptr;
            return false;
        }
    }
    
    // Create internet address
    TSharedRef<FInternetAddr> InternetAddr = SocketSubsystem->CreateInternetAddr();
    InternetAddr->SetIp(IPAddress.Value);
    InternetAddr->SetPort(Port);
    
    // Connect to server
    bool bConnected = TCPSocket->Connect(*InternetAddr);
    
    if (bConnected || TCPSocket->GetConnectionState() == SCS_Connected)
    {
        bIsConnected = true;
        UE_LOG(LogIGED, Warning, TEXT("Connected to server: %s:%d"), *ServerAddress, Port);
        OnConnected.Broadcast();
        return true;
    }
    else
    {
        UE_LOG(LogIGED, Error, TEXT("Failed to connect to server"));
        TCPSocket->Close();
        SocketSubsystem->DestroySocket(TCPSocket);
        TCPSocket = nullptr;
        return false;
    }
}

void UIGEDNetworkManager::Disconnect()
{
    if (TCPSocket)
    {
        TCPSocket->Close();
        SocketSubsystem->DestroySocket(TCPSocket);
        TCPSocket = nullptr;
    }
    
    bIsConnected = false;
    OnDisconnected.Broadcast();
    
    UE_LOG(LogIGED, Warning, TEXT("Disconnected from server"));
}

bool UIGEDNetworkManager::SendData(const FString& Data)
{
    if (!bIsConnected || !TCPSocket)
    {
        UE_LOG(LogIGED, Error, TEXT("Not connected to server"));
        return false;
    }
    
    // Convert string to bytes
    FTCHARToUTF8 Converter(*Data);
    int32 BytesSent = 0;
    
    bool bSuccess = TCPSocket->Send((uint8*)Converter.Get(), Converter.Length(), BytesSent);
    
    if (bSuccess)
    {
        UE_LOG(LogIGED, Warning, TEXT("Sent %d bytes to server"), BytesSent);
        return true;
    }
    else
    {
        UE_LOG(LogIGED, Error, TEXT("Failed to send data"));
        return false;
    }
}

bool UIGEDNetworkManager::SendJSON(const TSharedPtr<FJsonObject>& JsonObject)
{
    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    
    if (FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer))
    {
        return SendData(OutputString);
    }
    
    return false;
}

void UIGEDNetworkManager::ReceiveData()
{
    if (!TCPSocket) return;
    
    uint32 DataSize;
    while (TCPSocket->HasPendingData(DataSize))
    {
        TArray<uint8> ReceivedData;
        ReceivedData.SetNumUninitialized(FMath::Min(DataSize, 65507u));
        
        int32 BytesRead = 0;
        if (TCPSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), BytesRead))
        {
            if (BytesRead > 0)
            {
                // Convert bytes to string
                FString ReceivedString = FString(UTF8_TO_TCHAR(ReceivedData.GetData()));
                ReceivedString = ReceivedString.Left(BytesRead);
                
                UE_LOG(LogIGED, Warning, TEXT("Received: %s"), *ReceivedString);
                
                // Broadcast received data
                OnDataReceived.Broadcast(ReceivedString);
                
                // Try to parse as JSON
                TSharedPtr<FJsonObject> JsonObject;
                TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ReceivedString);
                
                if (FJsonSerializer::Deserialize(Reader, JsonObject))
                {
                    OnJSONReceived.Broadcast(JsonObject);
                }
            }
        }
    }
}

void UIGEDNetworkManager::SendHTTPRequest(const FString& URL, const FString& Verb, const FString& Content)
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = HttpModule->CreateRequest();
    
    Request->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
    {
        if (bWasSuccessful && Response.IsValid())
        {
            FString ResponseString = Response->GetContentAsString();
            int32 ResponseCode = Response->GetResponseCode();
            
            UE_LOG(LogIGED, Warning, TEXT("HTTP Response (%d): %s"), ResponseCode, *ResponseString);
            
            OnHTTPResponseReceived.Broadcast(ResponseCode, ResponseString);
        }
        else
        {
            UE_LOG(LogIGED, Error, TEXT("HTTP Request failed"));
            OnHTTPRequestFailed.Broadcast();
        }
    });
    
    Request->SetURL(URL);
    Request->SetVerb(Verb);
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetContentAsString(Content);
    Request->ProcessRequest();
    
    UE_LOG(LogIGED, Warning, TEXT("HTTP %s request sent to: %s"), *Verb, *URL);
}

void UIGEDNetworkManager::StartTCPListener(int32 Port)
{
    // Create listener socket
    ListenerSocket = SocketSubsystem->CreateSocket(NAME_Stream, TEXT("IGED_Listener"), false);
    
    if (!ListenerSocket)
    {
        UE_LOG(LogIGED, Error, TEXT("Failed to create listener socket"));
        return;
    }
    
    // Set socket options
    ListenerSocket->SetReuseAddr(true);
    ListenerSocket->SetNonBlocking(true);
    
    // Bind to port
    TSharedRef<FInternetAddr> ListenerAddr = SocketSubsystem->CreateInternetAddr();
    ListenerAddr->SetAnyAddress();
    ListenerAddr->SetPort(Port);
    
    if (!ListenerSocket->Bind(*ListenerAddr))
    {
        UE_LOG(LogIGED, Error, TEXT("Failed to bind listener to port %d"), Port);
        ListenerSocket->Close();
        SocketSubsystem->DestroySocket(ListenerSocket);
        ListenerSocket = nullptr;
        return;
    }
    
    // Start listening
    if (!ListenerSocket->Listen(8))
    {
        UE_LOG(LogIGED, Error, TEXT("Failed to start listening"));
        ListenerSocket->Close();
        SocketSubsystem->DestroySocket(ListenerSocket);
        ListenerSocket = nullptr;
        return;
    }
    
    UE_LOG(LogIGED, Warning, TEXT("TCP Listener started on port %d"), Port);
    
    // Start accepting connections
    GetWorld()->GetTimerManager().SetTimer(
        AcceptTimer,
        this,
        &UIGEDNetworkManager::AcceptConnections,
        0.1f,
        true
    );
}

void UIGEDNetworkManager::AcceptConnections()
{
    if (!ListenerSocket) return;
    
    TSharedRef<FInternetAddr> RemoteAddr = SocketSubsystem->CreateInternetAddr();
    FSocket* NewConnection = ListenerSocket->Accept(*RemoteAddr, TEXT("IGED_Client"));
    
    if (NewConnection)
    {
        NewConnection->SetNonBlocking(true);
        ClientConnections.Add(NewConnection);
        
        FString RemoteIP = RemoteAddr->ToString(false);
        UE_LOG(LogIGED, Warning, TEXT("Accepted connection from: %s"), *RemoteIP);
        
        OnClientConnected.Broadcast(RemoteIP);
    }
}

bool UIGEDNetworkManager::IsConnected() const
{
    return bIsConnected;
}

void UIGEDNetworkManager::SetAutoReconnect(bool bEnable)
{
    bAutoReconnect = bEnable;
}

void UIGEDNetworkManager::BroadcastToClients(const FString& Message)
{
    FTCHARToUTF8 Converter(*Message);
    
    for (FSocket* ClientSocket : ClientConnections)
    {
        if (ClientSocket && ClientSocket->GetConnectionState() == SCS_Connected)
        {
            int32 BytesSent = 0;
            ClientSocket->Send((uint8*)Converter.Get(), Converter.Length(), BytesSent);
        }
    }
}
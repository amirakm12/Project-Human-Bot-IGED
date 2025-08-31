; IGED UE5 - Inno Setup Installer Script
; Copyright (c) 2024 Project Human Bot

#define MyAppName "IGED UE5 - Advanced AI Assistant"
#define MyAppVersion "1.0.0"
#define MyAppPublisher "Project Human Bot"
#define MyAppURL "https://github.com/project-human-bot/IGED"
#define MyAppExeName "IGED_UE5.exe"
#define MyAppGUID "{F5E7D9A1-4B3C-4A2E-9F1D-8C3B7A6E5D4F}"

[Setup]
; Application Information
AppId={{#MyAppGUID}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}

; Installation Directories
DefaultDirName={autopf}\IGED_UE5
DefaultGroupName={#MyAppName}
DisableProgramGroupPage=no
DisableDirPage=no
DisableReadyPage=no
DisableFinishedPage=no

; Output Configuration
OutputDir=..\Output
OutputBaseFilename=IGED_UE5_Setup_{#MyAppVersion}
SetupIconFile=..\Resources\Icons\IGED.ico
Compression=lzma2/ultra64
SolidCompression=yes
CompressionThreads=auto
InternalCompressLevel=ultra64

; Windows Version Requirements
MinVersion=10.0
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64

; Visual Style
WizardStyle=modern
WizardImageFile=..\Resources\Installer\WizardImage.bmp
WizardSmallImageFile=..\Resources\Installer\WizardSmallImage.bmp
SetupLogging=yes

; Privileges
PrivilegesRequired=admin
PrivilegesRequiredOverridesAllowed=dialog

; Uninstaller
UninstallDisplayName={#MyAppName}
UninstallDisplayIcon={app}\{#MyAppExeName}
UninstallFilesDir={app}\Uninstall

; Other Settings
AllowNoIcons=yes
AlwaysShowDirOnReadyPage=yes
ShowLanguageDialog=auto
UsePreviousAppDir=yes
UsePreviousGroup=yes
UsePreviousSetupType=yes
UsePreviousTasks=yes
CreateAppDir=yes
RestartIfNeededByRun=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "german"; MessagesFile: "compiler:Languages\German.isl"
Name: "french"; MessagesFile: "compiler:Languages\French.isl"
Name: "spanish"; MessagesFile: "compiler:Languages\Spanish.isl"
Name: "japanese"; MessagesFile: "compiler:Languages\Japanese.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 6.1; Check: not IsAdminInstallMode
Name: "startupicon"; Description: "Launch IGED at Windows startup"; GroupDescription: "Startup Options"
Name: "firewall"; Description: "Add Windows Firewall exception"; GroupDescription: "Security Options"
Name: "vcredist"; Description: "Install Visual C++ Redistributables"; GroupDescription: "Prerequisites"; Flags: checkedonce

[Files]
; Main Application
Source: "..\Binaries\Win64\{#MyAppExeName}"; DestDir: "{app}\Binaries\Win64"; Flags: ignoreversion
Source: "..\Binaries\Win64\*.dll"; DestDir: "{app}\Binaries\Win64"; Flags: ignoreversion recursesubdirs
Source: "..\Binaries\Win64\*.pdb"; DestDir: "{app}\Binaries\Win64"; Flags: ignoreversion; Components: debug

; Content
Source: "..\Content\*"; DestDir: "{app}\Content"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\Config\*"; DestDir: "{app}\Config"; Flags: ignoreversion recursesubdirs createallsubdirs

; Shaders
Source: "..\Shaders\*"; DestDir: "{app}\Shaders"; Flags: ignoreversion recursesubdirs createallsubdirs

; Plugins
Source: "..\Plugins\*"; DestDir: "{app}\Plugins"; Flags: ignoreversion recursesubdirs createallsubdirs

; Resources
Source: "..\Resources\*"; DestDir: "{app}\Resources"; Flags: ignoreversion recursesubdirs createallsubdirs

; Documentation
Source: "..\README.md"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\LICENSE"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\CHANGELOG.md"; DestDir: "{app}"; Flags: ignoreversion

; Prerequisites
Source: "..\Prerequisites\vcredist_x64.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall; Tasks: vcredist
Source: "..\Prerequisites\DirectX\*"; DestDir: "{tmp}\DirectX"; Flags: deleteafterinstall recursesubdirs

[Dirs]
Name: "{app}\Saved"; Permissions: users-full
Name: "{app}\Logs"; Permissions: users-full
Name: "{app}\Memory"; Permissions: users-full
Name: "{app}\Output"; Permissions: users-full

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\Binaries\Win64\{#MyAppExeName}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{group}\Documentation"; Filename: "{app}\README.md"
Name: "{group}\License"; Filename: "{app}\LICENSE"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\Binaries\Win64\{#MyAppExeName}"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}"; Filename: "{app}\Binaries\Win64\{#MyAppExeName}"; Tasks: quicklaunchicon

[Registry]
; Application Registration
Root: HKLM; Subkey: "Software\ProjectHumanBot\IGED_UE5"; ValueType: string; ValueName: "InstallPath"; ValueData: "{app}"; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\ProjectHumanBot\IGED_UE5"; ValueType: string; ValueName: "Version"; ValueData: "{#MyAppVersion}"
Root: HKLM; Subkey: "Software\ProjectHumanBot\IGED_UE5"; ValueType: dword; ValueName: "InstalledDate"; ValueData: "{code:GetUnixTime}"

; File Associations
Root: HKCR; Subkey: ".iged"; ValueType: string; ValueName: ""; ValueData: "IGED.Document"; Flags: uninsdeletekey
Root: HKCR; Subkey: "IGED.Document"; ValueType: string; ValueName: ""; ValueData: "IGED Document"; Flags: uninsdeletekey
Root: HKCR; Subkey: "IGED.Document\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\Binaries\Win64\{#MyAppExeName},0"
Root: HKCR; Subkey: "IGED.Document\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\Binaries\Win64\{#MyAppExeName}"" ""%1"""

; Startup Entry
Root: HKLM; Subkey: "Software\Microsoft\Windows\CurrentVersion\Run"; ValueType: string; ValueName: "IGED_UE5"; ValueData: """{app}\Binaries\Win64\{#MyAppExeName}"" --startup"; Flags: uninsdeletevalue; Tasks: startupicon

[Run]
; Install Prerequisites
Filename: "{tmp}\vcredist_x64.exe"; Parameters: "/install /quiet /norestart"; StatusMsg: "Installing Visual C++ Redistributables..."; Tasks: vcredist
Filename: "{tmp}\DirectX\DXSETUP.exe"; Parameters: "/silent"; StatusMsg: "Installing DirectX Runtime..."; Check: DirectXNotInstalled

; Configure Firewall
Filename: "netsh"; Parameters: "advfirewall firewall add rule name=""IGED UE5"" dir=in action=allow program=""{app}\Binaries\Win64\{#MyAppExeName}"" enable=yes"; StatusMsg: "Configuring Windows Firewall..."; Flags: runhidden; Tasks: firewall

; Launch Application
Filename: "{app}\Binaries\Win64\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

[UninstallRun]
; Remove Firewall Rule
Filename: "netsh"; Parameters: "advfirewall firewall delete rule name=""IGED UE5"""; Flags: runhidden

[Code]
var
  DownloadPage: TDownloadWizardPage;

function InitializeSetup(): Boolean;
begin
  Result := True;
  
  // Check for .NET Framework
  if not IsDotNetInstalled(net462, 0) then
  begin
    MsgBox('Microsoft .NET Framework 4.6.2 or later is required.'#13#10'Please install it and run setup again.', mbError, MB_OK);
    Result := False;
  end;
  
  // Check for DirectX
  if not DirectXInstalled() then
  begin
    if MsgBox('DirectX Runtime is required but not installed.'#13#10'Setup will install it for you. Continue?', mbConfirmation, MB_YESNO) = IDNO then
      Result := False;
  end;
end;

procedure InitializeWizard;
begin
  // Create download page for additional prerequisites
  DownloadPage := CreateDownloadPage(SetupMessage(msgWizardPreparing), SetupMessage(msgPreparingDesc), nil);
end;

function PrepareToInstall(var NeedsRestart: Boolean): String;
begin
  // Check for running instances
  if IsAppRunning('{#MyAppExeName}') then
  begin
    Result := 'IGED UE5 is currently running. Please close it and try again.';
    Exit;
  end;
  
  Result := '';
end;

function GetUnixTime(Param: String): String;
begin
  Result := IntToStr(DateTimeToUnix(Now));
end;

function DirectXNotInstalled(): Boolean;
begin
  Result := not FileExists(ExpandConstant('{sys}\d3d11.dll'));
end;

function DirectXInstalled(): Boolean;
begin
  Result := FileExists(ExpandConstant('{sys}\d3d11.dll'));
end;

function IsDotNetInstalled(MinVersion: Integer; MinServicePack: Cardinal): Boolean;
var
  RegKey: String;
  ReleaseVer: Cardinal;
begin
  RegKey := 'SOFTWARE\Microsoft\NET Framework Setup\NDP\v4\Full';
  Result := RegQueryDWordValue(HKLM, RegKey, 'Release', ReleaseVer);
  if Result then
    Result := ReleaseVer >= MinVersion;
end;

function IsAppRunning(ExeName: String): Boolean;
var
  FSWbemLocator: Variant;
  FWMIService: Variant;
  FWbemObjectSet: Variant;
begin
  Result := False;
  try
    FSWbemLocator := CreateOleObject('WBEMScripting.SWBEMLocator');
    FWMIService := FSWbemLocator.ConnectServer('localhost', 'root\CIMV2', '', '');
    FWbemObjectSet := FWMIService.ExecQuery('SELECT Name FROM Win32_Process WHERE Name="' + ExeName + '"');
    Result := FWbemObjectSet.Count > 0;
  except
  end;
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssPostInstall then
  begin
    // Create initial configuration
    SaveStringToFile(ExpandConstant('{app}\Config\initial_setup.json'), 
      '{"first_run": true, "version": "{#MyAppVersion}"}', False);
  end;
end;

procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
begin
  if CurUninstallStep = usPostUninstall then
  begin
    // Clean up remaining files
    if MsgBox('Do you want to remove all user data and settings?', mbConfirmation, MB_YESNO) = IDYES then
    begin
      DelTree(ExpandConstant('{app}\Saved'), True, True, True);
      DelTree(ExpandConstant('{app}\Logs'), True, True, True);
      DelTree(ExpandConstant('{app}\Memory'), True, True, True);
    end;
  end;
end;

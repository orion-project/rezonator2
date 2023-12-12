;
; InnoSetup install script for reZonator
; script version 3.0 created 2023/12/12
;

; Use suffix 2 in order not to clash with the old version
; which also could be installed 
#define AppName      "reZonator2"

#define AppExe       "rezonator.exe"
#define AppPublisher "orion-project.org"
#define AppURL       "http://rezonator.orion-project.org"
#define BaseDir      "..\..\out\redist"
#define PrjRegId     AppName+".Schema"
#define PrjFileExt   ".rez"
#define PrjFileDesc  "reZonator project"

;Inno 6.2.2 can't extract version info from Qt-compiled exe file
;#define AppPublisher GetFileCompany(BaseDir + "\" + AppExe)
;#define AppVerFull GetFileVersionString(BaseDir + "\" + AppExe)
;#define AppVerShort GetFileProductVersion(BaseDir + "\" + AppExe)
#define FileHandle = FileOpen("..\..\release\version.txt")
#if FileHandle
  #define AppVerFull = FileRead(FileHandle)
  #define VerTmp = AppVerFull
  #define AppVerMajor = DeleteToFirstPeriod(VerTmp)
  #define AppVerMinor = DeleteToFirstPeriod(VerTmp)
  ;TODO: handle the case when there is no dash-suffix
  #define AppVerPatch = Copy(VerTmp, 1, Pos("-", VerTmp)-1)
  #define AppVerShort = AppVerMajor + "." + AppVerMinor
  #expr FileClose(FileHandle)
#else
  #error "Unable to open version file"
#endif

[Setup]
AppId={{6A7D721F-8EB4-4FD0-AA1F-0CC3ACD5F9CD}
AppName={#AppName}
AppVerName=reZonator {#AppVerShort}
AppVersion={#AppVerFull}
AppPublisher={#AppPublisher}
AppPublisherURL={#AppURL}
AppSupportURL={#AppURL}
AppUpdatesURL={#AppURL}
SetupMutex=reZonator2_SetupMutex
CloseApplications=yes
DefaultDirName={autopf64}\{#AppName}
DefaultGroupName={#AppName}
AllowNoIcons=yes
LicenseFile={#BaseDir}\..\..\LICENSE
;Output=no
OutputDir={#BaseDir}\..\install
OutputBaseFilename="rezonator-{#AppVerFull}"
Compression=lzma
SolidCompression=yes
PrivilegesRequired=none
ChangesAssociations=yes                                                 
VersionInfoVersion={#AppVerShort}.{#AppVerPatch}.0
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64
WizardImageFile=side.bmp

; Windows 10 (1809) is the minimum supported by Qt6
; https://doc.qt.io/qt-6/supported-platforms.html
MinVersion=10.0.17763

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "associations"; Description: "{cm:AssocFileExtension,{#AppName},{#PrjFileExt}}"; Flags: checkedonce
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: checkedonce
Name: "desktopicon\common"; Description: "{cm:ForAllUser}"; GroupDescription: "{cm:AdditionalIcons}"
Name: "desktopicon\user"; Description: "{cm:ForCurrentUser}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "{#BaseDir}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs

[Icons]
Name: "{group}\{#AppName}"; Filename: "{app}\{#AppExe}"
Name: "{group}\{cm:ProgramOnTheWeb,{#AppName}}"; Filename: "{#AppURL}"
Name: "{group}\{cm:UninstallProgram,{#AppName}}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\{#AppName}"; Filename: "{app}\{#AppExe}"; Tasks: desktopicon\common
Name: "{userdesktop}\{#AppName}"; Filename: "{app}\{#AppExe}"; Tasks: desktopicon\user

[Registry]
Root: HKCR; Subkey: {#PrjFileExt}; ValueType: string; ValueName: ""; ValueData: "{#PrjRegId}"; Flags: uninsdeletevalue; Tasks: associations
Root: HKCR; Subkey: "{#PrjRegId}"; ValueType: string; ValueName: ""; ValueData: {#PrjFileDesc}; Flags: uninsdeletekey; Tasks: associations
Root: HKCR; Subkey: "{#PrjRegId}\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#AppExe},1"; Flags: uninsdeletevalue; Tasks: associations
Root: HKCR; Subkey: "{#PrjRegId}\Shell\Open\Command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#AppExe}"" ""%1"""; Flags: uninsdeletekey; Tasks: associations

[UninstallDelete]
Type: files;          Name: "{userappdata}\{#AppPublisher}\reZonator.*"
Type: dirifempty;     Name: "{userappdata}\{#AppPublisher}"
Type: filesandordirs; Name: "{localappdata}\{#AppPublisher}\rezonator"
Type: dirifempty;     Name: "{localappdata}\{#AppPublisher}"

[Run]
Filename: "{app}\{#AppExe}"; Description: "{cm:LaunchProgram,{#AppName}}"; Flags: nowait postinstall skipifsilent

[CustomMessages]
english.ForAllUser=For all user
english.ForCurrentUser=For current user

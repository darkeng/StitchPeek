[Setup]
AppName=StitchPeek
AppVersion=1.0.0
AppPublisher=darkeng
AppUpdatesURL=https://github.com/darkeng/StitchPeek
DefaultDirName={localappdata}\StitchPeek
DefaultGroupName=StitchPeek
DisableProgramGroupPage=yes
UninstallDisplayIcon={app}\StitchPeek.dll
Compression=lzma2
SolidCompression=yes
ArchitecturesInstallIn64BitMode=x64
ArchitecturesAllowed=x64
OutputBaseFilename=StitchPeek_1.0.0_Installer
OutputDir=out
; Runs smoothly without annoying Administrator (UAC) prompts
PrivilegesRequired=lowest
; Tells Windows Explorer to refresh automatically so thumbnails show up instantly 
ChangesAssociations=yes

[Files]
; The actual extension DLL. It will be copied and automatically registered via regserver 
Source: "build\Release\StitchPeek.dll"; DestDir: "{app}"; Flags: ignoreversion regserver 64bit RestartReplace

[InstallDelete]
Type: files; Name: "{app}\StitchPeek_old*.dll"

[UninstallDelete]
Type: files; Name: "{app}\StitchPeek.dll"

[Icons]
Name: "{group}\Uninstall StitchPeek"; Filename: "{uninstallexe}"

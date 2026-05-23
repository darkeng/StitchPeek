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

[Code]
{ ============================================================================
  Pre-install cleanup of orphaned shell handlers.

  Many machines used for embroidery work have leftovers from previous
  software installs (Brother PE-Design / Active Graphics, Wilcom
  EmbroideryStudio) that registered ProgIDs and shell extensions pointing
  to CLSIDs whose DLLs no longer exist. Those orphaned ProgIDs intercept
  the shell's thumbnail lookup before it reaches StitchPeek:
  AssocQueryStringW resolves to a non-existent CLSID, returns
  REGDB_E_CLASSNOTREG, and every file gets a blank icon.

  Everything here is HKCU and per-user, so it does not need admin rights
  and matches the installer's PrivilegesRequired=lowest setup.
  ============================================================================ }

const
  Exts: array[0..6] of String = ('.pes', '.dst', '.exp', '.jef', '.vp3', '.xxx', '.pec');
  PropHandlerIID = '{00021500-0000-0000-C000-000000000046}';
  CacheDir = '{localappdata}\Microsoft\Windows\Explorer\';

procedure CleanGhostHandlers;
var
  I: Integer;
  Ext, FileExtsKey, ClassesKey: String;
begin
  for I := 0 to High(Exts) do
  begin
    Ext := Exts[I];
    ClassesKey := 'Software\Classes\' + Ext;
    FileExtsKey := 'Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts\' + Ext + '\OpenWithProgids';

    { Wilcom EmbroideryStudio leftover ProgID - the real culprit:
      its ShellEx points to a ghost CLSID that breaks our thumbnail lookup. }
    RegDeleteKeyIncludingSubkeys(HKCU, 'Software\Classes\Wilcom.StitchFile' + Ext);

    { Brother PE-Design / Active Graphics leftover ProgID. }
    RegDeleteKeyIncludingSubkeys(HKCU, 'Software\Classes\Embroidery.Design' + Ext);

    { Remove references to those dead ProgIDs from OpenWithProgids. }
    RegDeleteValue(HKCU, FileExtsKey, 'Wilcom.StitchFile' + Ext);
    RegDeleteValue(HKCU, FileExtsKey, 'Embroidery.Design' + Ext);

    { Stale per-extension handlers (IconHandler / PropertyHandler) that
      point to CLSIDs from uninstalled software. }
    RegDeleteKeyIncludingSubkeys(HKCU, ClassesKey + '\ShellEx\IconHandler');
    RegDeleteKeyIncludingSubkeys(HKCU, ClassesKey + '\ShellEx\' + PropHandlerIID);

    { Clear the ProgID assignment so the shell falls back to looking up
      the thumbnail provider on the extension directly. }
    RegDeleteValue(HKCU, ClassesKey, '');
  end;
end;

procedure WipeShellCache;
var
  Dir: String;
begin
  Dir := ExpandConstant(CacheDir);
  { Wildcards are honored when IsDir is False. Files locked by an active
    Explorer instance will silently stay - that is fine, Windows
    regenerates the cache from our newly registered provider on next access. }
  DelTree(Dir + 'thumbcache_*.db', False, True, False);
  DelTree(Dir + 'iconcache_*.db', False, True, False);
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  { Pre-install: clear ghost handlers before regsvr32 fires for our DLL. }
  if CurStep = ssInstall then
    CleanGhostHandlers;

  { Post-install: invalidate the cache so Explorer regenerates thumbnails
    from StitchPeek instead of showing stale blank icons. }
  if CurStep = ssPostInstall then
    WipeShellCache;
end;

procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
begin
  { After uninstalling, wipe the cache too so it does not retain
    StitchPeek-generated thumbnails after the DLL is gone. }
  if CurUninstallStep = usPostUninstall then
    WipeShellCache;
end;

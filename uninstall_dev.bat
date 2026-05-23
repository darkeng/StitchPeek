@echo off
::==============================================================================
:: uninstall_dev.bat - Unregister the development build of StitchPeek
::==============================================================================
:: Needs Administrator because install_dev.bat mirrors the registration to
:: HKLM. We do NOT restore the ghost ProgIDs cleaned during install
:: (Wilcom.StitchFile.* / Embroidery.Design.*) - they were broken pointers
:: to uninstalled software; if the user reinstalls Wilcom or PE-Design,
:: those installers will recreate their own entries.
::==============================================================================

:: --- Auto-elevate to Administrator -------------------------------------------
net session >nul 2>&1
if %errorLevel% NEQ 0 (
    echo [!] Requesting elevation via UAC to remove machine-wide registration...
    powershell -Command "Start-Process -FilePath '%~f0' -WorkingDirectory '%~dp0' -Verb RunAs"
    exit /b
)
cd /d "%~dp0"

:: --- Free the DLL ------------------------------------------------------------
echo [*] Stopping Explorer and any COM surrogate holding the DLL...
taskkill /f /im explorer.exe 2>nul
taskkill /f /im dllhost.exe 2>nul

:: --- Unregister per-user (HKCU) ----------------------------------------------
echo [*] Unregistering StitchPeek.dll (HKCU via regsvr32)...
if exist "build\Release\StitchPeek.dll" (
    regsvr32.exe /u /s "build\Release\StitchPeek.dll"
) else (
    echo     DLL not found, falling back to manual HKCU cleanup.
    reg delete "HKCU\Software\Classes\CLSID\{C6F4CCFA-8C37-4E64-A9D6-7EB76DC284FD}" /f >nul 2>&1
    for %%E in (.pes .dst .exp .jef .vp3 .xxx .pec) do (
        reg delete "HKCU\Software\Classes\%%E\ShellEx\{e357fccd-a995-4576-b01f-234630154e96}" /f >nul 2>&1
    )
)

:: --- Remove HKLM mirror ------------------------------------------------------
echo [*] Removing HKLM machine-wide mirror...
reg delete "HKLM\SOFTWARE\Classes\CLSID\{C6F4CCFA-8C37-4E64-A9D6-7EB76DC284FD}" /f >nul 2>&1
for %%E in (.pes .dst .exp .jef .vp3 .xxx .pec) do (
    reg delete "HKLM\SOFTWARE\Classes\%%E\ShellEx\{e357fccd-a995-4576-b01f-234630154e96}" /f >nul 2>&1
)

:: --- Wipe caches so Explorer regenerates thumbnails from scratch -------------
echo [*] Wiping Windows thumbnail and icon caches...
del /F /A /Q "%LocalAppData%\Microsoft\Windows\Explorer\thumbcache_*.db" 2>nul
del /F /A /Q "%LocalAppData%\Microsoft\Windows\Explorer\iconcache_*.db" 2>nul

:: --- Let Windows respawn Explorer at normal integrity ------------------------
:: Same reason as install_dev.bat: do NOT 'start explorer.exe' from this
:: elevated process. Windows respawns it automatically with the correct
:: integrity level.

echo.
echo [*] Done. StitchPeek has been unregistered.
echo     You can now safely test the Setup Installer (.exe) from a clean state.
echo.
pause

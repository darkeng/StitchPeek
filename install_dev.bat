@echo off
::==============================================================================
:: install_dev.bat - Register StitchPeek.dll for active development
::==============================================================================
:: Why this script needs Administrator:
::   Many machines used for embroidery work have leftovers from previous
::   software installs (Brother PE-Design / Active Graphics, Wilcom
::   EmbroideryStudio, etc.) that registered shell extensions for our same
::   file types but point to CLSIDs or DLLs that no longer exist. Those ghost
::   handlers intercept the shell's thumbnail lookup BEFORE it reaches our
::   provider (AssocQueryStringW resolves to a non-existent CLSID, returning
::   REGDB_E_CLASSNOTREG and leaving the file with a blank icon).
::
::   Cleaning those leftovers requires touching HKLM and PropertyHandlers,
::   hence the UAC prompt. The actual StitchPeek registration is per-user
::   (HKCU), mirrored to HKLM so packaged shell hosts on Win11 also see it.
::==============================================================================

:: --- Auto-elevate to Administrator -------------------------------------------
net session >nul 2>&1
if %errorLevel% NEQ 0 (
    echo [!] Requesting elevation via UAC to clean stale machine-wide handlers...
    powershell -Command "Start-Process -FilePath '%~f0' -WorkingDirectory '%~dp0' -Verb RunAs"
    exit /b
)
cd /d "%~dp0"

:: --- Sanity check: DLL must be built first -----------------------------------
if not exist "build\Release\StitchPeek.dll" (
    echo.
    echo [X] build\Release\StitchPeek.dll not found.
    echo     Run build.bat first to compile StitchPeek.dll, then re-run this script.
    echo.
    pause
    exit /b 1
)

:: --- Free the DLL so we can re-register it -----------------------------------
echo [*] Stopping Explorer and any COM surrogate holding the DLL...
taskkill /f /im explorer.exe 2>nul
taskkill /f /im dllhost.exe 2>nul

:: --- Clean third-party leftovers (HKLM, machine-wide) ------------------------
:: These ghost handlers are the main reason a fresh StitchPeek install does
:: not show thumbnails on machines that previously had PE-Design or Wilcom.
echo [*] Cleaning ghost shell handlers from previous embroidery software...
for %%E in (.pes .dst .exp .jef .vp3 .xxx .pec) do (
    reg delete "HKLM\SOFTWARE\Classes\%%E\ShellEx" /f >nul 2>&1
    reg delete "HKLM\SOFTWARE\Classes\%%E" /v "" /f >nul 2>&1
    reg delete "HKLM\SOFTWARE\Classes\Embroidery.Design%%E" /f >nul 2>&1
    reg delete "HKLM\SOFTWARE\Classes\Wilcom.StitchFile%%E" /f >nul 2>&1
    reg delete "HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\PropertySystem\PropertyHandlers\%%E" /f >nul 2>&1
)

:: --- Clean per-user leftovers (HKCU) -----------------------------------------
:: The Wilcom.StitchFile.<ext> ProgID in OpenWithProgids is the specific
:: residue that makes AssocQueryStringW resolve to a ghost CLSID.
echo [*] Cleaning per-user ghost handlers and ProgID associations...
for %%E in (.pes .dst .exp .jef .vp3 .xxx .pec) do (
    reg delete "HKCU\Software\Classes\%%E\ShellEx\IconHandler" /f >nul 2>&1
    reg delete "HKCU\Software\Classes\%%E\ShellEx\{00021500-0000-0000-C000-000000000046}" /f >nul 2>&1
    reg delete "HKCU\Software\Classes\%%E" /v "" /f >nul 2>&1
    reg delete "HKCU\Software\Classes\Embroidery.Design%%E" /f >nul 2>&1
    reg delete "HKCU\Software\Classes\Wilcom.StitchFile%%E" /f >nul 2>&1
    reg delete "HKCU\Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts\%%E\OpenWithProgids" /v "Wilcom.StitchFile%%E" /f >nul 2>&1
    reg delete "HKCU\Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts\%%E\OpenWithProgids" /v "Embroidery.Design%%E" /f >nul 2>&1
)

:: --- Wipe Windows thumbnail/icon caches --------------------------------------
:: Otherwise Explorer keeps showing the previously-cached blank icon.
echo [*] Wiping Windows thumbnail and icon caches...
del /F /A /Q "%LocalAppData%\Microsoft\Windows\Explorer\thumbcache_*.db" 2>nul
del /F /A /Q "%LocalAppData%\Microsoft\Windows\Explorer\iconcache_*.db" 2>nul

:: --- Register StitchPeek (per-user, via DllRegisterServer) -------------------
echo [*] Registering StitchPeek.dll (HKCU via regsvr32)...
regsvr32.exe /s "build\Release\StitchPeek.dll"

:: --- Mirror registration to HKLM (machine-wide) ------------------------------
:: Some Windows 11 shell hosts (especially packaged ones) only consult HKLM
:: for thumbnail providers. Mirroring the CLSID and per-extension handler
:: there ensures the provider is found in every context.
echo [*] Mirroring registration to HKLM (machine-wide)...
set "DLL_PATH=%~dp0build\Release\StitchPeek.dll"
reg add "HKLM\SOFTWARE\Classes\CLSID\{C6F4CCFA-8C37-4E64-A9D6-7EB76DC284FD}" /ve /t REG_SZ /d "StitchPeek Thumbnail Provider" /f >nul
reg add "HKLM\SOFTWARE\Classes\CLSID\{C6F4CCFA-8C37-4E64-A9D6-7EB76DC284FD}" /v "DisableProcessIsolation" /t REG_DWORD /d 1 /f >nul
reg add "HKLM\SOFTWARE\Classes\CLSID\{C6F4CCFA-8C37-4E64-A9D6-7EB76DC284FD}\InProcServer32" /ve /t REG_SZ /d "%DLL_PATH%" /f >nul
reg add "HKLM\SOFTWARE\Classes\CLSID\{C6F4CCFA-8C37-4E64-A9D6-7EB76DC284FD}\InProcServer32" /v "ThreadingModel" /t REG_SZ /d "Apartment" /f >nul
for %%E in (.pes .dst .exp .jef .vp3 .xxx .pec) do (
    reg add "HKLM\SOFTWARE\Classes\%%E" /v "PerceivedType" /t REG_SZ /d "image" /f >nul
    reg add "HKLM\SOFTWARE\Classes\%%E\ShellEx\{e357fccd-a995-4576-b01f-234630154e96}" /ve /t REG_SZ /d "{C6F4CCFA-8C37-4E64-A9D6-7EB76DC284FD}" /f >nul
)

:: --- Let Windows respawn Explorer at normal integrity ------------------------
:: IMPORTANT: do NOT call 'start explorer.exe' from this elevated script.
:: A child Explorer process would inherit the high integrity token and fail
:: to resolve shell extensions registered for the interactive user.
:: Windows automatically respawns Explorer after our taskkill above.

echo.
echo [*] Done. StitchPeek is registered.
echo     Wait a couple of seconds for Explorer to come back, then open a folder
echo     with .pes/.dst/.exp/.jef/.vp3/.xxx/.pec files to see thumbnails.
echo.
pause

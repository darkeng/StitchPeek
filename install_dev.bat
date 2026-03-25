@echo off
echo [*] Cleaning previous frozen DLL sessions in Explorer/dllhost...
taskkill /f /im explorer.exe 2>nul
taskkill /f /im dllhost.exe 2>nul

echo [*] Clearing Windows Thumbnail Cache database...
del /F /A /Q "%LocalAppData%\Microsoft\Windows\Explorer\thumbcache_*.db" 2>nul

echo [*] Registering StitchPeek.dll...
regsvr32.exe /s "build\Release\StitchPeek.dll"

echo [*] Reloading Windows Explorer...
start explorer.exe

echo [*] Success!
echo StitchPeek is now registered for your user. Open a folder with Embroidery files.
pause

@echo off
echo [*] Compiling Final Installer using Inno Setup...
echo.

"%ProgramFiles(x86)%\Inno Setup 6\ISCC.exe" "installer.iss"

if %ERRORLEVEL%==0 (
    echo.
    echo [*] Success! The final installer is available inside the 'out\' folder.
    echo     You can double-click the .exe there to try it out.
) else (
    echo.
    echo [!] An error occurred while attempting to compile installer.iss.
    echo     Make sure Inno Setup 6 is installed at "%ProgramFiles(x86)%\Inno Setup 6".
)
pause

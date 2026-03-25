@echo off
echo [*] Preparing Visual Studio Environment...
:: Dynamically find Visual Studio installation path using vswhere
for /f "usebackq tokens=*" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -property installationPath`) do (
  set InstallDir=%%i
)

if exist "%InstallDir%\Common7\Tools\VsDevCmd.bat" (
  call "%InstallDir%\Common7\Tools\VsDevCmd.bat" -arch=amd64
) else (
  echo [!] Could not find Visual Studio 2017 or newer. Please install the Desktop C++ workload.
  pause
  exit /b 1
)

echo [*] Generating CMake Build Files...
cmake -B build -A x64

echo [*] Compiling StitchPeek.dll Release...
cmake --build build --config Release

echo [*] Compiling test_thumbnail.exe...
cmake --build build --config Release --target test_thumbnail

echo [*] Build Complete! 
echo      - Your DLL is at: build\Release\StitchPeek.dll
echo      - Your Test program is at: build\Release\test_thumbnail.exe
echo.
echo [*] Pro-tip: Run install_dev.bat to register the DLL in your system!
pause

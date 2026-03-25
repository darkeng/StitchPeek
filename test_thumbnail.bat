@echo off
setlocal enabledelayedexpansion

set "TEST_DIR=%~dp0test"
set "EXE_PATH=%~dp0build\Release\test_thumbnail.exe"

echo =============================================
echo  StitchPeek Thumbnail Generator Test Suite
echo =============================================

if not exist "%EXE_PATH%" (
    echo Error: test_thumbnail.exe not found! Please run build.bat first.
    exit /b 1
)

set "exts=.pes .exp .jef .vp3 .xxx .pec .dst"

for %%e in (%exts%) do (
    set "FILE=%TEST_DIR%\sample%%e"
    
    if exist "!FILE!" (
        <nul set /p ="Testing format [%%e]... "
        
        "%EXE_PATH%" "!FILE!" >nul 2>&1
        
        if !errorlevel! equ 0 (
            if exist "!FILE!.png" (
                echo OK -^> Generated sample%%e.png
            ) else (
                echo Failed -^> No PNG generated.
            )
        ) else (
            echo Failed -^> Render error.
        )
    ) else (
        echo Skipped: No sample%%e in test folder.
    )
)

echo =============================================
echo Tests finished. Check the test folder.

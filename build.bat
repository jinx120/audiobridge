@echo off
REM AudioBridge build script for Windows

echo Checking dependencies...

REM Check for CMake
where cmake >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: CMake is not installed or not in PATH.
    echo Please install CMake and try again.
    goto :error
)

REM Check for Visual Studio or other compiler (basic check)
where cl >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo WARNING: Microsoft Visual C++ compiler not found in PATH.
    echo If the build fails, please make sure Visual Studio is installed with C++ components.
)

echo Creating build directory...
REM Create build directory if it doesn't exist
if not exist build mkdir build
cd build

echo Configuring with CMake...
REM Configure with CMake
cmake .. || goto :error

echo Building the project...
REM Build the project
cmake --build . --config Release || goto :error

echo.
echo Build completed successfully!
echo.
echo To run AudioBridge, execute:
echo   .\Release\AudioBridge.exe
echo.
echo Note: You may need to add icon files to resources\icons\ before building.
echo See resources\icons\README.md for details.
echo.
echo If you encounter any issues, please check the INSTALL.md file for troubleshooting tips.
goto :end

:error
echo.
echo Build failed. Please check the error messages above.
echo See INSTALL.md for installation instructions and troubleshooting tips.

:end
pause

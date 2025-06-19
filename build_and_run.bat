@echo off
REM Build and run the ColorWarSDL project using CMake and MinGW/Visual Studio

REM Set build directory
set BUILD_DIR=build

REM Create build directory if it doesn't exist
if not exist %BUILD_DIR% mkdir %BUILD_DIR%
cd %BUILD_DIR%

REM Run CMake to generate build files
cmake ..
if errorlevel 1 (
    echo CMake configuration failed.
    exit /b 1
)

REM Build the project
cmake --build .
if errorlevel 1 (
    echo Build failed.
    exit /b 1
)

REM Run the executable if build succeeded
if exist ColorWarSDL.exe (
    echo Running ColorWarSDL.exe ...
    ColorWarSDL.exe
) else (
    echo Executable not found.
    exit /b 1
)

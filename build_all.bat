@echo off

REM ===== x64 =====
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

cmake -S . -B out\build\x64-Debug -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build out\build\x64-Debug

cmake -S . -B out\build\x64-Release -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build out\build\x64-Release

REM ===== x86 =====
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat"

cmake -S . -B out\build\x86-Debug -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build out\build\x86-Debug

cmake -S . -B out\build\x86-Release -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build out\build\x86-Release

REM ===== install =====
cmake --install out\build\x64-Debug   --prefix out\install\x64-Debug
cmake --install out\build\x64-Release   --prefix out\install\x64-Release
cmake --install out\build\x86-Debug   --prefix out\install\x86-Debug
cmake --install out\build\x86-Release   --prefix out\install\x86-Release

echo ===== ALL BUILD DONE =====
pause
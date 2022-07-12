call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat"
@call "%VS160COMNTOOLS%VsDevCmd.bat"

SET mypath=%~dp0

set target_path=%1

mkdir build
cd build
robocopy %mypath%assets %target_path%assets /E /mt /z
robocopy %mypath%assets_editor %target_path%assets_editor /E /mt /z
devenv RaceEngine.sln /build Shipping
robocopy ./bin/Shipping %target_path% /E /mt /z
cd %target_path%
del game.exe
ren editor.exe game.exe
mkdir render
cd render
mkdir sources
cd sources
mkdir shaders
cd ../../../
robocopy %mypath%render/sources/shaders/ %target_path%render/sources/shaders *.spv /mt /z
robocopy %mypath% %target_path% imgui.ini /mt /z
robocopy %mypath%vendor/glfw/lib/release/ %target_path% *.dll /mt /z
robocopy %mypath%vendor/Physx/lib/release/ %target_path% *.dll /mt /z
robocopy %mypath%vendor/lua/lib/ %target_path% *.dll /mt /z
robocopy %mypath%vendor/FMOD/lib/ %target_path% *.dll /mt /z
robocopy %mypath%vendor/assimp/lib/ %target_path% *.dll /mt /z
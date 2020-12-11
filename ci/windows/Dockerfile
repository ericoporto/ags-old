# base will already have Chocolatey installed
FROM  ericoporto/min-ags-dev-env:1.0.0

# no temp folder exists by default
RUN IF exist %TEMP%\nul ( echo %TEMP% ) ELSE ( mkdir %TEMP% )

# Windows 8.0 SDK (but only install the .NET 4.5 SDK)
#
RUN cd %TEMP% && \
    curl -fLO http://download.microsoft.com/download/F/1/3/F1300C9C-A120-4341-90DF-8A52509B23AC/standalonesdk/sdksetup.exe && \
    start /wait sdksetup /ceip off /features OptionID.NetFxSoftwareDevelopmentKit /quiet /norestart  && \
    powershell -NoLogo -NoProfile -Command \
    Remove-Item C:\Users\ContainerAdministrator\AppData\Local\Temp\*.* -Force -Recurse

# Install Nuget and Innosetup
# Download, build and install vcpkg
# Install Xiph libraries for OGG, Theora and Vorbis
# Download and places in the rights places Directx and Irrklang 
#
ARG NUGET_VERSION=5.7.0
ARG INNO_SETUP_VERSION=6.0.5
ARG IRRKLANG_VERSION=1.6.0
RUN cinst nuget.commandline --version %NUGET_VERSION% -y && \
    cinst innosetup --version %INNO_SETUP_VERSION% -y && \
    pushd %TEMP% && \
    git clone https://github.com/Microsoft/vcpkg.git && \
    pushd vcpkg && \
    if not exist vcpkg.exe call bootstrap-vcpkg.bat && \
    vcpkg install libogg:x86-windows-static libtheora:x86-windows-static libvorbis:x86-windows-static && \
    popd && \
    popd && \
    mkdir Lib\Xiph && \
    copy %TEMP%\vcpkg\installed\x86-windows-static\lib\ogg.lib Lib\Xiph\libogg_static.lib && \
    copy %TEMP%\vcpkg\installed\x86-windows-static\lib\theora.lib Lib\Xiph\libtheora_static.lib && \
    copy %TEMP%\vcpkg\installed\x86-windows-static\lib\vorbis.lib Lib\Xiph\libvorbis_static.lib && \
    copy %TEMP%\vcpkg\installed\x86-windows-static\lib\vorbisfile.lib Lib\Xiph\libvorbisfile_static.lib && \
    curl -fLSs http://www.ambiera.at/downloads/irrKlang-32bit-%IRRKLANG_VERSION%.zip | tar -f - -xvzC %TEMP% irrKlang-%IRRKLANG_VERSION%/bin/dotnet-4/*.dll && \
    mkdir Lib\irrKlang && \
    move %TEMP%\irrKlang-%IRRKLANG_VERSION%\bin\dotnet-4\*.dll Lib\irrKlang\ && \
    rd /s /q %TEMP%\irrKlang-%IRRKLANG_VERSION% && \
    curl -fLSs https://download.microsoft.com/download/3/3/f/33f1af6e-c61b-4f14-a0de-3e9096ed4b3a/dxsdk_aug2007.exe | tar -f - -xvzC %TEMP% && \
    tar -f %TEMP%\dxsdk_aug2007.exe -xvzC %TEMP% Lib/x86/*.lib && \
    mkdir Lib\DirectX && \
    move %TEMP%\Lib\x86\*.lib Lib\DirectX\ && \
    del %TEMP%\dxsdk_aug2007.exe && \
    rd /s /q %TEMP%\Lib && \
    powershell -NoLogo -NoProfile -Command \
    Remove-Item C:\ProgramData\chocolatey\logs\*.* -Force -Recurse ; \
    Remove-Item C:\Users\ContainerAdministrator\AppData\Local\Temp\*.* -Force -Recurse

RUN mkdir Redist && \
    cd Redist && \
    curl -fLOJ https://download.microsoft.com/download/6/A/A/6AA4EDFF-645B-48C5-81CC-ED5963AEAD48/vc_redist.x86.exe

# Downloads SDL binaries for Visual C development and download and build SDL Sound
#
ARG SDL_VERSION=2.0.12
ARG SDL_SOUND_VERSION=997e90562b35 
RUN mkdir Lib\SDL2 && \
    curl -fLOJ "https://www.libsdl.org/release/SDL2-devel-%SDL_VERSION%-VC.zip" && \
    tar -f SDL2-devel-%SDL_VERSION%-VC.zip -xvzC Lib\SDL2\ --strip-components 1  && \
    del /f SDL2-devel-%SDL_VERSION%-VC.zip && \
    echo set^(SDL2_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/include"^) > "Lib\SDL2\sdl2-config.cmake" && \
    echo # Support both 32 and 64 bit builds >> "Lib\SDL2\sdl2-config.cmake" && \
    echo if ^(${CMAKE_SIZEOF_VOID_P} MATCHES 8^) >> "Lib\SDL2\sdl2-config.cmake" && \
    echo   set^(SDL2_LIBRARIES "${CMAKE_CURRENT_LIST_DIR}/lib/x64/SDL2.lib;${CMAKE_CURRENT_LIST_DIR}/lib/x64/SDL2main.lib"^)  >> "Lib\SDL2\sdl2-config.cmake" && \
    echo else ^(^)  >> "Lib\SDL2\sdl2-config.cmake" && \
    echo   set^(SDL2_LIBRARIES "${CMAKE_CURRENT_LIST_DIR}/lib/x86/SDL2.lib;${CMAKE_CURRENT_LIST_DIR}/lib/x86/SDL2main.lib"^) >> "Lib\SDL2\sdl2-config.cmake" && \
    echo endif ^(^)  >> "Lib\SDL2\sdl2-config.cmake" && \
    echo string^(STRIP "${SDL2_LIBRARIES}" SDL2_LIBRARIES^) >> "Lib\SDL2\sdl2-config.cmake" && \
    echo "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x86 ^&^& pushd Lib\SDL_sound\build ^&^& msbuild SDL_sound.sln /p:PlatformToolset=v140 /p:Configuration=Release /p:Platform=Win32 /maxcpucount /nologo ^&^& popd > sdlsoundbuild.bat && \
    mkdir Lib\SDL_sound && \
    mkdir Lib\SDL_sound\build  && \	
    curl -fLSs "https://hg.icculus.org/icculus/SDL_sound/archive/%SDL_SOUND_VERSION%.tar.gz" | tar -f - -xvzC Lib\SDL_sound --strip-components 1 && \
    set SDL2_DIR=%cd%\Lib\SDL2 && \
    cmake -DCMAKE_SYSTEM_VERSION=8.1 -S Lib\SDL_sound -B Lib\SDL_sound\build -G "Visual Studio 16 2019" -T"v140" -A"Win32" -DCMAKE_PREFIX_PATH=Lib\SDL2 && \
    sdlsoundbuild.bat && \
    copy Lib\SDL_sound\build\Release\SDL2_sound-static.lib Lib\SDL_sound\build\Release\SDL_sound.lib && \
    mkdir empty && \
    robocopy empty %TEMP% /MIR > nul & \
    rd /s /q empty

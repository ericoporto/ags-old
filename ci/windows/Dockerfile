# base will already have Chocolatey installed
FROM cirrusci/windowsservercore:cmake
#FROM cirrusci/windowsservercore:2019

# Microsoft .NET Framework 4.8 4.8.0.20190930 is broken and causes the
# installation of visualstudio2019-workload-vctools to fail. To workaround
# the issue request Microsoft .NET Framework 4.7.2.20180712 instead, which
# is already installed and thus speeds up installation as well.
# this is just to prevent visualstudio2019 to force a reset in the machine
#RUN cinst -y --no-progress dotnetfx --version=4.7.2.20180712

#RUN cinst -y --no-progress --version=16.8.2.0 visualstudio2019buildtools 
#RUN cinst -y --no-progress --version=1.0.0 visualstudio2019-workload-vctools 
#RUN cinst -y --no-progress --version=1.0.0 visualstudio2019-workload-nativedesktop || echo "ignore reboot request"

# no temp folder exists by default
RUN IF exist %TEMP%\nul ( echo %TEMP% ) ELSE ( mkdir %TEMP% )

ARG VISUALCPP_BUILD_TOOLS_VERSION=14.0.25420.1
RUN cinst visualcpp-build-tools --version %VISUALCPP_BUILD_TOOLS_VERSION% -y

# Windows 8.0 SDK (but only install the .NET 4.5 SDK)
RUN cd %TEMP% && \
  curl -fLO http://download.microsoft.com/download/F/1/3/F1300C9C-A120-4341-90DF-8A52509B23AC/standalonesdk/sdksetup.exe && \
  start /wait sdksetup /ceip off /features OptionID.NetFxSoftwareDevelopmentKit /quiet /norestart

ARG NUGET_VERSION=5.7.0
RUN cinst nuget.commandline --version %NUGET_VERSION% -y

ARG INNO_SETUP_VERSION=6.0.5
RUN cinst innosetup --version %INNO_SETUP_VERSION% -y

# ARG CMAKE_VERSION=3.18.4
# RUN cinst cmake --version %CMAKE_VERSION% --installargs ADD_CMAKE_TO_PATH=System -y

RUN pushd %TEMP% && \
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
  copy %TEMP%\vcpkg\installed\x86-windows-static\lib\vorbisfile.lib Lib\Xiph\libvorbisfile_static.lib

ARG IRRKLANG_VERSION=1.6.0
RUN curl -fLSs http://www.ambiera.at/downloads/irrKlang-32bit-%IRRKLANG_VERSION%.zip | tar -f - -xvzC %TEMP% irrKlang-%IRRKLANG_VERSION%/bin/dotnet-4/*.dll && \
  mkdir Lib\irrKlang && \
  move %TEMP%\irrKlang-%IRRKLANG_VERSION%\bin\dotnet-4\*.dll Lib\irrKlang\ && \
  rd /s /q %TEMP%\irrKlang-%IRRKLANG_VERSION%

RUN curl -fLSs https://download.microsoft.com/download/3/3/f/33f1af6e-c61b-4f14-a0de-3e9096ed4b3a/dxsdk_aug2007.exe | tar -f - -xvzC %TEMP% && \
  tar -f %TEMP%\dxsdk_aug2007.exe -xvzC %TEMP% Lib/x86/*.lib && \
  mkdir Lib\DirectX && \
  move %TEMP%\Lib\x86\*.lib Lib\DirectX\ && \
  del %TEMP%\dxsdk_aug2007.exe && \
  rd /s /q %TEMP%\Lib

RUN mkdir Redist && \
  cd Redist && \
  curl -fLOJ https://download.microsoft.com/download/6/A/A/6AA4EDFF-645B-48C5-81CC-ED5963AEAD48/vc_redist.x86.exe

ARG SDL_VERSION=2.0.12
RUN mkdir Lib\SDL2 && \
  curl -fLOJ "https://www.libsdl.org/release/SDL2-devel-%SDL_VERSION%-VC.zip" && \
  tar -f SDL2-devel-%SDL_VERSION%-VC.zip -xvzC Lib\SDL2\ --strip-components 1  && \
  del /f SDL2-devel-%SDL_VERSION%-VC.zip
    
RUN echo set^(SDL2_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/include"^) > "Lib\SDL2\sdl2-config.cmake"
RUN echo # Support both 32 and 64 bit builds >> "Lib\SDL2\sdl2-config.cmake"
RUN echo if ^(${CMAKE_SIZEOF_VOID_P} MATCHES 8^) >> "Lib\SDL2\sdl2-config.cmake"
RUN echo   set^(SDL2_LIBRARIES "${CMAKE_CURRENT_LIST_DIR}/lib/x64/SDL2.lib;${CMAKE_CURRENT_LIST_DIR}/lib/x64/SDL2main.lib"^)  >> "Lib\SDL2\sdl2-config.cmake"
RUN echo else ^(^)  >> "Lib\SDL2\sdl2-config.cmake"
RUN echo   set^(SDL2_LIBRARIES "${CMAKE_CURRENT_LIST_DIR}/lib/x86/SDL2.lib;${CMAKE_CURRENT_LIST_DIR}/lib/x86/SDL2main.lib"^) >> "Lib\SDL2\sdl2-config.cmake"
RUN echo endif ^(^)  >> "Lib\SDL2\sdl2-config.cmake"
RUN echo string^(STRIP "${SDL2_LIBRARIES}" SDL2_LIBRARIES^) >> "Lib\SDL2\sdl2-config.cmake"
 
RUN dir Lib\SDL2
RUN type Lib\SDL2\sdl2-config.cmake

ARG SDL_SOUND_VERSION=9262f9205898 
RUN mkdir Lib\SDL_sound && \
  mkdir Lib\SDL_sound\build  && \	
  curl -fLSs "https://hg.icculus.org/icculus/SDL_sound/archive/%SDL_SOUND_VERSION%.tar.gz" | tar -f - -xvzC Lib\SDL_sound --strip-components 1 
  
# RUN echo "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\BuildTools\\VC\\Auxiliary\\Build\\vcvarsall.bat" x86 ^&^& cd Lib\SDL_sound\build ^&^& msbuild SDL_sound.sln /p:PlatformToolset=v140 /p:Configuration=Release /p:Platform=Win32 /maxcpucount /nologo > vcbars.bat
# RUN type vcbars.bat

RUN set SDL2_DIR=%cd%\Lib\SDL2 && \
  cmake -DC_STANDARD=99 -S Lib\SDL_sound -B Lib\SDL_sound\build -G "Visual Studio 14 2015" -T v140 -A Win32 -DCMAKE_PREFIX_PATH=Lib\SDL2 && \
  cd Lib\SDL_sound\build && \
  cmake --build . --config Release

RUN echo "ok!"
  
# delete all temp Files
RUN mkdir empty && \
  robocopy empty %TEMP% /MIR > nul & \
  rd /s /q empty

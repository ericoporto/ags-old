//=============================================================================
//
// Adventure Game Studio (AGS)
//
// Copyright (C) 1999-2011 Chris Jones and 2011-20xx others
// The full list of copyright holders can be found in the Copyright.txt
// file, which is part of this source code distribution.
//
// The AGS source code is provided under the Artistic License 2.0.
// A copy of this license can be found in the file License.txt and at
// http://www.opensource.org/licenses/artistic-license-2.0.php
//
//=============================================================================

#include "core/platform.h"

#if AGS_PLATFORM_OS_EMSCRIPTEN

// ********* EMSCRIPTEN PLACEHOLDER DRIVER *********

#include <stdio.h>
#include <allegro.h>
#include "SDL.h"
#include "ac/runtime_defines.h"
#include "gfx/gfxdefines.h"
#include "platform/base/agsplatformdriver.h"
#include "plugin/agsplugin.h"
#include "util/string.h"
#include <libcda.h>

#include <pwd.h>
#include <sys/stat.h>

using AGS::Common::String;

String CommonDataDirectory;
String UserDataDirectory;

struct AGSEmscripten : AGSPlatformDriver {

  int  CDPlayerCommand(int cmdd, int datt) override;
  void DisplayAlert(const char*, ...) override;
  const char *GetAllUsersDataDirectory() override;
  const char *GetUserSavedgamesDirectory() override;
  const char *GetUserConfigDirectory() override;
  const char *GetUserGlobalConfigDirectory() override;
  const char *GetAppOutputDirectory() override;
  unsigned long GetDiskFreeSpaceMB() override;
  const char* GetNoMouseErrorString() override;
  const char* GetAllegroFailUserHint() override;
  eScriptSystemOSID GetSystemOSID() override;
  int  InitializeCDPlayer() override;
  void PostAllegroExit() override;
  void SetGameWindowIcon() override;
  void ShutdownCDPlayer() override;
  bool LockMouseToWindow() override;
  void UnlockMouse() override;
};


int AGSEmscripten::CDPlayerCommand(int cmdd, int datt) {
  return 0;
}

void AGSEmscripten::DisplayAlert(const char *text, ...) {
  char displbuf[2000];
  va_list ap;
  va_start(ap, text);
  vsprintf(displbuf, text, ap);
  va_end(ap);
  if (_logToStdErr)
    fprintf(stderr, "%s\n", displbuf);
  else
    fprintf(stdout, "%s\n", displbuf);
}

void DetermineDataDirectories()
{
  if (!UserDataDirectory.IsEmpty())
    return;

  UserDataDirectory.Format("/home/web_user/ags");
  mkdir(UserDataDirectory.GetCStr(), 0755);
  CommonDataDirectory.Format("/home/web_user/common");
  mkdir(CommonDataDirectory.GetCStr(), 0755);
}

const char *AGSEmscripten::GetAllUsersDataDirectory()
{
  DetermineDataDirectories();
  return CommonDataDirectory;
}

const char *AGSEmscripten::GetUserSavedgamesDirectory()
{
  DetermineDataDirectories();
  return UserDataDirectory;
}

const char *AGSEmscripten::GetUserConfigDirectory()
{
  return GetUserSavedgamesDirectory();
}

const char *AGSEmscripten::GetUserGlobalConfigDirectory()
{
  return GetUserSavedgamesDirectory();
}

const char *AGSEmscripten::GetAppOutputDirectory()
{
  DetermineDataDirectories();
  return UserDataDirectory;
}

unsigned long AGSEmscripten::GetDiskFreeSpaceMB() {
  // placeholder
  return 100;
}

const char* AGSEmscripten::GetNoMouseErrorString() {
  return "This game requires a mouse. You need to configure and setup your mouse to play this game.\n";
}

const char* AGSEmscripten::GetAllegroFailUserHint()
{
  return "Make sure you have latest version of Allegro 4 libraries installed, and X server is running.";
}

eScriptSystemOSID AGSEmscripten::GetSystemOSID() {
  return eOS_Linux;
}

int AGSEmscripten::InitializeCDPlayer() {
  return 0;
}

void AGSEmscripten::PostAllegroExit() {
  // do nothing
}

void AGSEmscripten::SetGameWindowIcon() {
  // do nothing
}

void AGSEmscripten::ShutdownCDPlayer() {

}

AGSPlatformDriver* AGSPlatformDriver::GetDriver() {
  if (instance == nullptr)
    instance = new AGSEmscripten();
  return instance;
}

bool AGSEmscripten::LockMouseToWindow()
{
    return SDL_SetRelativeMouseMode(SDL_TRUE) == 0;
}

void AGSEmscripten::UnlockMouse()
{
    SDL_SetRelativeMouseMode(SDL_FALSE);
}


#endif

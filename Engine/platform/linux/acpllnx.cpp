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

#if AGS_PLATFORM_OS_LINUX

// ********* LINUX PLACEHOLDER DRIVER *********

#include <stdio.h>
#include <allegro.h>
#include "SDL.h"
//#include <xalleg.h>
#include "ac/runtime_defines.h"
#include "gfx/gfxdefines.h"
#include "platform/base/agsplatformdriver.h"
#include "plugin/agsplugin.h"
#include "util/string.h"
#include <libcda.h>

#include <pwd.h>
#include <sys/stat.h>

using AGS::Common::String;


// Replace the default Allegro icon. The original defintion is in the
// Allegro 4.4 source under "src/x/xwin.c".
#include "icon.xpm"
void* allegro_icon = icon_xpm;
String CommonDataDirectory;
String UserDataDirectory;

struct AGSLinux : AGSPlatformDriver {

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
//  void GetSystemDisplayModes(std::vector<Engine::DisplayMode> &dms) override;
};


int AGSLinux::CDPlayerCommand(int cmdd, int datt) {
  return 0;// cd_player_control(cmdd, datt);
}

void AGSLinux::DisplayAlert(const char *text, ...) {
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

size_t BuildXDGPath(char *destPath, size_t destSize)
{
  // Check to see if XDG_DATA_HOME is set in the enviroment
#if AGS_PLATFORM_OS_EMSCRIPTEN
    return 0;
#endif

  const char* home_dir = getenv("XDG_DATA_HOME");
  size_t l = 0;
  if (home_dir)
  {
    l = snprintf(destPath, destSize, "%s", home_dir);
  }
  else
  {
    // No evironment variable, so we fall back to home dir in /etc/passwd
    struct passwd *p = getpwuid(getuid());
    l = snprintf(destPath, destSize, "%s/.local", p->pw_dir);
    if (mkdir(destPath, 0755) != 0 && errno != EEXIST)
      return 0;
    l += snprintf(destPath + l, destSize - l, "/share");
    if (mkdir(destPath, 0755) != 0 && errno != EEXIST)
      return 0;
  }
  return l;
}

void DetermineDataDirectories()
{
  if (!UserDataDirectory.IsEmpty())
    return;
  char xdg_path[256];
  if (BuildXDGPath(xdg_path, sizeof(xdg_path)) == 0)
    sprintf(xdg_path, "%s", "/tmp");
  UserDataDirectory.Format("%s/ags", xdg_path);
  mkdir(UserDataDirectory.GetCStr(), 0755);
  CommonDataDirectory.Format("%s/ags-common", xdg_path);
  mkdir(CommonDataDirectory.GetCStr(), 0755);
}

const char *AGSLinux::GetAllUsersDataDirectory()
{
  DetermineDataDirectories();
  return CommonDataDirectory;
}

const char *AGSLinux::GetUserSavedgamesDirectory()
{
  DetermineDataDirectories();
  return UserDataDirectory;
}

const char *AGSLinux::GetUserConfigDirectory()
{
  return GetUserSavedgamesDirectory();
}

const char *AGSLinux::GetUserGlobalConfigDirectory()
{
  return GetUserSavedgamesDirectory();
}

const char *AGSLinux::GetAppOutputDirectory()
{
  DetermineDataDirectories();
  return UserDataDirectory;
}

unsigned long AGSLinux::GetDiskFreeSpaceMB() {
  // placeholder
  return 100;
}

const char* AGSLinux::GetNoMouseErrorString() {
  return "This game requires a mouse. You need to configure and setup your mouse to play this game.\n";
}

const char* AGSLinux::GetAllegroFailUserHint()
{
  return "Make sure you have latest version of Allegro 4 libraries installed, and X server is running.";
}

eScriptSystemOSID AGSLinux::GetSystemOSID() {
  return eOS_Linux;
}

int AGSLinux::InitializeCDPlayer() {
  return 0;// cd_player_init();
}

void AGSLinux::PostAllegroExit() {
  // do nothing
}

void AGSLinux::SetGameWindowIcon() {
  // do nothing
}

void AGSLinux::ShutdownCDPlayer() {
  //cd_exit();
}

AGSPlatformDriver* AGSPlatformDriver::GetDriver() {
  if (instance == nullptr)
    instance = new AGSLinux();
  return instance;
}

bool AGSLinux::LockMouseToWindow()
{
    return SDL_SetRelativeMouseMode(SDL_TRUE) == 0;
//    return XGrabPointer(_xwin.display, _xwin.window, False,
  //      PointerMotionMask | ButtonPressMask | ButtonReleaseMask,
//        GrabModeAsync, GrabModeAsync, _xwin.window, None, CurrentTime) == GrabSuccess;
}

void AGSLinux::UnlockMouse()
{
    SDL_SetRelativeMouseMode(SDL_FALSE);
  //  XUngrabPointer(_xwin.display, CurrentTime);
}


#endif

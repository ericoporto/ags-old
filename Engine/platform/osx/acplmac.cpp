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

#if AGS_PLATFORM_OS_MACOS

// ********* MacOS PLACEHOLDER DRIVER *********

//#include "util/wgt2allg.h"
//#include "gfx/ali3d.h"
//#include "ac/runtime_defines.h"
//#include "main/config.h"
//#include "plugin/agsplugin.h"
//#include <pwd.h>
//#include <sys/stat.h>
#include "platform/base/agsplatformdriver.h"
#include "util/directory.h"
#include "ac/common.h"
#include "main/main.h"

void AGSMacInitPaths(char appdata[PATH_MAX]);
void AGSMacGetBundleDir(char gamepath[PATH_MAX]);
//bool PlayMovie(char const *name, int skipType);

static char libraryApplicationSupport[PATH_MAX];
static char commonDataPath[PATH_MAX];

struct AGSMac : AGSPlatformDriver
{
  AGSMac();
  virtual void DisplayAlert(const char*, ...) override;
  virtual unsigned long GetDiskFreeSpaceMB() override;
  virtual eScriptSystemOSID GetSystemOSID() override;
    
  virtual const char *GetUserSavedgamesDirectory() override;
  virtual const char *GetAllUsersDataDirectory() override;
  virtual const char *GetUserConfigDirectory() override;
  virtual const char *GetAppOutputDirectory() override;
  virtual const char *GetIllegalFileChars() override;
};

AGSMac::AGSMac()
{
  AGSMacInitPaths(libraryApplicationSupport);
  
  snprintf(commonDataPath, PATH_MAX, "%s/uk.co.adventuregamestudio", libraryApplicationSupport);
  AGS::Common::Directory::CreateDirectory(commonDataPath);
}

void AGSMac::DisplayAlert(const char *text, ...) {
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

unsigned long AGSMac::GetDiskFreeSpaceMB() {
  // placeholder
  return 100;
}

eScriptSystemOSID AGSMac::GetSystemOSID() {
  // override performed if `override.os` is set in config.
  return eOS_Mac;
}

const char* AGSMac::GetAllUsersDataDirectory()
{
  return commonDataPath;
}

const char *AGSMac::GetUserSavedgamesDirectory()
{
  return libraryApplicationSupport;
}

const char *AGSMac::GetUserConfigDirectory()
{
  return libraryApplicationSupport;
}

const char *AGSMac::GetAppOutputDirectory()
{
  return commonDataPath;
}

const char *AGSMac::GetIllegalFileChars()
{
  return "\\/:?\"<>|*"; // keep same as Windows so we can sync.
}

AGSPlatformDriver* AGSPlatformDriver::GetDriver() {
  if (instance == NULL)
    instance = new AGSMac();
  return instance;
}

#endif

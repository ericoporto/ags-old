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
#include "ac/common.h"
#include "ac/display.h"
#include "ac/draw.h"
#include "ac/game_version.h"
#include "ac/gamesetup.h"
#include "ac/gamesetupstruct.h"
#include "ac/gamestate.h"
#include "ac/mouse.h"
#include "ac/runtime_defines.h"
#include "ac/walkbehind.h"
#include "ac/dynobj/scriptsystem.h"
#include "debug/out.h"
#include "device/mousew32.h"
#include "font/fonts.h"
#include "gfx/ali3dexception.h"
#include "gfx/graphicsdriver.h"
#include "gui/guimain.h"
#include "gui/guiinv.h"
#include "main/graphics_mode.h"
#include "main/engine_setup.h"
#include "media/video/video.h"
#include "platform/base/agsplatformdriver.h"
#include "platform/base/sys_main.h"

using namespace AGS::Common;
using namespace AGS::Engine;

extern GameSetupStruct game;
extern ScriptSystem scsystem;
extern int _places_r, _places_g, _places_b;
extern IGraphicsDriver *gfxDriver;

// CLNUP most likely remove these
int convert_16bit_bgr = 0;

void engine_setup_system_gamesize()
{
    scsystem.width = game.GetGameRes().Width;
    scsystem.height = game.GetGameRes().Height;
    scsystem.viewport_width = play.GetMainViewport().GetWidth();
    scsystem.viewport_height = play.GetMainViewport().GetHeight();
}

// CLNUP IsHiRes was used to double lowres assets, better remove now and decide a new implementation later, if ever
void engine_init_resolution_settings(const Size game_size)
{
    Debug::Printf("Initializing resolution settings");
    usetup.textheight = getfontheight_outlined(0) + 1;

    Debug::Printf(kDbgMsg_Info, "Game native resolution: %d x %d (%d bit)%s", game_size.Width, game_size.Height, game.color_depth * 8,
        game.IsLegacyLetterbox() ? " letterbox-by-design" : "");

    Rect viewport = RectWH(game_size);
    play.SetMainViewport(viewport);
    play.SetUIViewport(viewport);
    engine_setup_system_gamesize();
}

// Setup gfx driver callbacks and options
void engine_post_gfxmode_driver_setup()
{
    gfxDriver->SetCallbackForPolling(update_polled_stuff_if_runtime);
    gfxDriver->SetCallbackToDrawScreen(draw_game_screen_callback, construct_engine_overlay);
    gfxDriver->SetCallbackForNullSprite(GfxDriverNullSpriteCallback);
}

// Reset gfx driver callbacks
void engine_pre_gfxmode_driver_cleanup()
{
    gfxDriver->SetCallbackForPolling(nullptr);
    gfxDriver->SetCallbackToDrawScreen(nullptr, nullptr);
    gfxDriver->SetCallbackForNullSprite(nullptr);
    gfxDriver->SetMemoryBackBuffer(nullptr);
}

// Setup virtual screen
void engine_post_gfxmode_screen_setup(const DisplayMode &dm, bool recreate_bitmaps)
{
    if (recreate_bitmaps)
    {
        // TODO: find out if 
        // - we need to support this case at all;
        // - if yes then which bitmaps need to be recreated (probably only video bitmaps and textures?)
    }
}

void engine_pre_gfxmode_screen_cleanup()
{
}

// Release virtual screen
void engine_pre_gfxsystem_screen_destroy()
{
}

// Setup color conversion parameters
// CLNUP we want only 32 bit for the future, color conversion should be dropped I guess
void engine_setup_color_conversions(int coldepth)
{
    // default shifts for how we store the sprite data1
    _rgb_r_shift_32 = 16;
    _rgb_g_shift_32 = 8;
    _rgb_b_shift_32 = 0;
    _rgb_r_shift_16 = 11;
    _rgb_g_shift_16 = 5;
    _rgb_b_shift_16 = 0;
    _rgb_r_shift_15 = 10;
    _rgb_g_shift_15 = 5;
    _rgb_b_shift_15 = 0;

    // Most cards do 5-6-5 RGB, which is the format the files are saved in
    // Some do 5-6-5 BGR, or  6-5-5 RGB, in which case convert the gfx
    if ((coldepth == 16) && ((_rgb_b_shift_16 != 0) || (_rgb_r_shift_16 != 11)))
    {
        convert_16bit_bgr = 1;
        if (_rgb_r_shift_16 == 10) {
            // some very old graphics cards lie about being 16-bit when they
            // are in fact 15-bit ... get around this
            _places_r = 3;
            _places_g = 3;
        }
    }
    if (coldepth > 16)
    {
        // when we're using 32-bit colour, it converts hi-color images
        // the wrong way round - so fix that

        _rgb_r_shift_16 = 11;
        _rgb_g_shift_16 = 5;
        _rgb_b_shift_16 = 0;
    }
    else if (coldepth == 16)
    {
        // ensure that any 32-bit graphics displayed are converted
        // properly to the current depth
        _rgb_r_shift_32 = 16;
        _rgb_g_shift_32 = 8;
        _rgb_b_shift_32 = 0;
    }
    else if (coldepth < 16)
    {
        // ensure that any 32-bit graphics displayed are converted
        // properly to the current depth
#if AGS_PLATFORM_OS_WINDOWS
        _rgb_r_shift_32 = 16;
        _rgb_g_shift_32 = 8;
        _rgb_b_shift_32 = 0;
#else
        _rgb_r_shift_32 = 0;
        _rgb_g_shift_32 = 8;
        _rgb_b_shift_32 = 16;

        _rgb_b_shift_15 = 0;
        _rgb_g_shift_15 = 5;
        _rgb_r_shift_15 = 10;
#endif
    }

    set_color_conversion(COLORCONV_MOST | COLORCONV_EXPAND_256);
}

// Setup drawing modes and color conversions;
// they depend primarily on gfx driver capabilities and new color depth
void engine_post_gfxmode_draw_setup(const DisplayMode &dm)
{
    engine_setup_color_conversions(dm.ColorDepth); // CLNUP proabably remove
    init_draw_method();
}

// Cleanup auxiliary drawing objects
void engine_pre_gfxmode_draw_cleanup()
{
    dispose_draw_method();
}

// Setup mouse control mode and graphic area
void engine_post_gfxmode_mouse_setup(const DisplayMode &dm, const Size &init_desktop)
{
    // Assign mouse control parameters.
    //
    // NOTE that we setup speed and other related properties regardless of
    // whether mouse control was requested because it may be enabled later.
    Mouse::SetSpeedUnit(1.f);
    if (usetup.mouse_speed_def == kMouseSpeed_CurrentDisplay)
    {
        Size cur_desktop;
        if (sys_get_desktop_resolution(cur_desktop.Width, cur_desktop.Height) == 0)
            Mouse::SetSpeedUnit(Math::Max((float)cur_desktop.Width / (float)init_desktop.Width,
            (float)cur_desktop.Height / (float)init_desktop.Height));
    }

    Mouse_EnableControl(usetup.mouse_ctrl_enabled);
    Debug::Printf(kDbgMsg_Info, "Mouse speed control: %s, unit: %f, user value: %f",
        usetup.mouse_ctrl_enabled ? "enabled" : "disabled", Mouse::GetSpeedUnit(), Mouse::GetSpeed());

    on_coordinates_scaling_changed();

    // If auto lock option is set, lock mouse to the game window
    if (usetup.mouse_auto_lock && scsystem.windowed != 0)
        Mouse::TryLockToWindow();
}

// Reset mouse controls before changing gfx mode
void engine_pre_gfxmode_mouse_cleanup()
{
    // Always disable mouse control and unlock mouse when releasing down gfx mode
    Mouse::SetMovementControl(false);
    Mouse::UnlockFromWindow();
}

// Fill in scsystem struct with display mode parameters
void engine_setup_scsystem_screen(const DisplayMode &dm)
{
    scsystem.coldepth = dm.ColorDepth;
    scsystem.windowed = dm.Windowed;
    scsystem.vsync = dm.Vsync;
}

void engine_post_gfxmode_setup(const Size &init_desktop)
{
    DisplayMode dm = gfxDriver->GetDisplayMode();
    // If color depth has changed (or graphics mode was inited for the
    // very first time), we also need to recreate bitmaps
    bool has_driver_changed = scsystem.coldepth != dm.ColorDepth;

    engine_setup_scsystem_screen(dm);
    engine_post_gfxmode_driver_setup();
    if (has_driver_changed)
    {
        engine_post_gfxmode_draw_setup(dm);
    }
    engine_post_gfxmode_screen_setup(dm, has_driver_changed);   
    engine_post_gfxmode_mouse_setup(dm, init_desktop);

    video_on_gfxmode_changed();
    invalidate_screen();
}

void engine_pre_gfxmode_release()
{
    engine_pre_gfxmode_mouse_cleanup();
    engine_pre_gfxmode_driver_cleanup();
    engine_pre_gfxmode_screen_cleanup();
}

void engine_pre_gfxsystem_shutdown()
{
    engine_pre_gfxmode_release();
    engine_pre_gfxmode_draw_cleanup();
    engine_pre_gfxsystem_screen_destroy();
}

void on_coordinates_scaling_changed()
{
    // Reset mouse graphic area and bounds
    Mouse::UpdateGraphicArea();
    // If mouse bounds do not have valid values yet, then limit cursor to viewport
    if (play.mboundx1 == 0 && play.mboundy1 == 0 && play.mboundx2 == 0 && play.mboundy2 == 0)
        Mouse::SetMoveLimit(play.GetMainViewport());
    else
        Mouse::SetMoveLimit(Rect(play.mboundx1, play.mboundy1, play.mboundx2, play.mboundy2));
}

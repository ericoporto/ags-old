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

#include <ctype.h> // for toupper

#include "core/platform.h"
#include "util/string_utils.h" //strlwr()
#include "ac/common.h"
#include "ac/charactercache.h"
#include "ac/characterextras.h"
#include "ac/draw.h"
#include "ac/event.h"
#include "ac/game.h"
#include "ac/gamesetup.h"
#include "ac/gamesetupstruct.h"
#include "ac/gamestate.h"
#include "ac/global_audio.h"
#include "ac/global_character.h"
#include "ac/global_game.h"
#include "ac/global_object.h"
#include "ac/global_translation.h"
#include "ac/movelist.h"
#include "ac/mouse.h"
#include "ac/objectcache.h"
#include "ac/overlay.h"
#include "ac/properties.h"
#include "ac/region.h"
#include "ac/sys_events.h"
#include "ac/room.h"
#include "ac/roomobject.h"
#include "ac/roomstatus.h"
#include "ac/screen.h"
#include "ac/string.h"
#include "ac/system.h"
#include "ac/walkablearea.h"
#include "ac/walkbehind.h"
#include "ac/dynobj/scriptobject.h"
#include "ac/dynobj/scripthotspot.h"
#include "gui/guimain.h"
#include "script/cc_instance.h"
#include "debug/debug_log.h"
#include "debug/debugger.h"
#include "debug/out.h"
#include "game/room_version.h"
#include "platform/base/agsplatformdriver.h"
#include "plugin/agsplugin.h"
#include "plugin/plugin_engine.h"
#include "script/cc_error.h"
#include "script/script.h"
#include "script/script_runtime.h"
#include "ac/spritecache.h"
#include "util/stream.h"
#include "gfx/graphicsdriver.h"
#include "core/assetmanager.h"
#include "ac/dynobj/all_dynamicclasses.h"
#include "gfx/bitmap.h"
#include "gfx/gfxfilter.h"
#include "util/math.h"
#include "media/audio/audio_system.h"

using namespace AGS::Common;
using namespace AGS::Engine;

extern GameSetup usetup;
extern GameSetupStruct game;
extern GameState play;
extern RoomStatus*croom;
extern RoomStatus troom;    // used for non-saveable rooms, eg. intro
extern int displayed_room;
extern RoomObject*objs;
extern ccInstance *roominst;
extern AGSPlatformDriver *platform;
extern int numevents;
extern CharacterCache *charcache;
extern ObjectCache objcache[MAX_ROOM_OBJECTS];
extern CharacterExtras *charextra;
extern int done_es_error;
extern int our_eip;
extern Bitmap *walkareabackup, *walkable_areas_temp;
extern ScriptObject scrObj[MAX_ROOM_OBJECTS];
extern SpriteCache spriteset;
extern int in_new_room, new_room_was;  // 1 in new room, 2 first time in new room, 3 loading saved game
extern ScriptHotspot scrHotspot[MAX_ROOM_HOTSPOTS];
extern int in_leaves_screen;
extern CharacterInfo*playerchar;
extern int starting_room;
extern unsigned int loopcounter;
extern IDriverDependantBitmap* roomBackgroundBmp;
extern IGraphicsDriver *gfxDriver;
extern Bitmap *raw_saved_screen;
extern int actSpsCount;
extern Bitmap **actsps;
extern IDriverDependantBitmap* *actspsbmp;
extern Bitmap **actspswb;
extern IDriverDependantBitmap* *actspswbbmp;
extern CachedActSpsData* actspswbcache;
extern RGB palette[256];
extern int mouse_z_was;

extern Bitmap **guibg;
extern IDriverDependantBitmap **guibgbmp;

extern CCHotspot ccDynamicHotspot;
extern CCObject ccDynamicObject;

RGB_MAP rgb_table;  // for 256-col antialiasing
int new_room_flags=0;
int gs_to_newroom=-1;

ScriptDrawingSurface* Room_GetDrawingSurfaceForBackground(int backgroundNumber)
{
    if (displayed_room < 0)
        quit("!Room.GetDrawingSurfaceForBackground: no room is currently loaded");

    if (backgroundNumber == SCR_NO_VALUE)
    {
        backgroundNumber = play.bg_frame;
    }

    if ((backgroundNumber < 0) || ((size_t)backgroundNumber >= thisroom.BgFrameCount))
        quit("!Room.GetDrawingSurfaceForBackground: invalid background number specified");


    ScriptDrawingSurface *surface = new ScriptDrawingSurface();
    surface->roomBackgroundNumber = backgroundNumber;
    ccRegisterManagedObject(surface, surface);
    return surface;
}

ScriptDrawingSurface* Room_GetDrawingSurfaceForMask(RoomAreaMask mask)
{
    if (displayed_room < 0)
        quit("!Room_GetDrawingSurfaceForMask: no room is currently loaded");
    ScriptDrawingSurface *surface = new ScriptDrawingSurface();
    surface->roomMaskType = mask;
    ccRegisterManagedObject(surface, surface);
    return surface;
}

int Room_GetObjectCount() {
    return croom->numobj;
}

int Room_GetWidth() {
    return thisroom.Width;
}

int Room_GetHeight() {
    return thisroom.Height;
}

int Room_GetColorDepth() {
    return thisroom.BgFrames[0].Graphic->GetColorDepth();
}

int Room_GetLeftEdge() {
    return thisroom.Edges.Left;
}

int Room_GetRightEdge() {
    return thisroom.Edges.Right;
}

int Room_GetTopEdge() {
    return thisroom.Edges.Top;
}

int Room_GetBottomEdge() {
    return thisroom.Edges.Bottom;
}

int Room_GetMusicOnLoad() {
    return thisroom.Options.StartupMusic;
}

int Room_GetProperty(const char *property)
{
    return get_int_property(thisroom.Properties, croom->roomProps, property);
}

const char* Room_GetTextProperty(const char *property)
{
    return get_text_property_dynamic_string(thisroom.Properties, croom->roomProps, property);
}

bool Room_SetProperty(const char *property, int value)
{
    return set_int_property(croom->roomProps, property, value);
}

bool Room_SetTextProperty(const char *property, const char *value)
{
    return set_text_property(croom->roomProps, property, value);
}

const char* Room_GetMessages(int index) {
    if ((index < 0) || ((size_t)index >= thisroom.MessageCount)) {
        return nullptr;
    }
    char buffer[STD_BUFFER_SIZE];
    buffer[0]=0;
    replace_tokens(get_translation(thisroom.Messages[index].GetCStr()), buffer, STD_BUFFER_SIZE);
    return CreateNewScriptString(buffer);
}

bool Room_Exists(int room)
{
    String room_filename;
    room_filename.Format("room%d.crm", room);
    return AssetMgr->DoesAssetExist(room_filename);
}

//=============================================================================

void save_room_data_segment () {
    croom->FreeScriptData();
    
    croom->tsdatasize = roominst->globaldatasize;
    if (croom->tsdatasize > 0) {
        croom->tsdata=(char*)malloc(croom->tsdatasize+10);
        memcpy(croom->tsdata,&roominst->globaldata[0],croom->tsdatasize);
    }

}

void unload_old_room() {
    int ff;

    // if switching games on restore, don't do this
    if (displayed_room < 0)
        return;

    debug_script_log("Unloading room %d", displayed_room);

    current_fade_out_effect();

    dispose_room_drawdata();

    for (ff=0;ff<croom->numobj;ff++)
        objs[ff].moving = 0;

    if (!play.ambient_sounds_persist) {
        for (ff = 1; ff < MAX_SOUND_CHANNELS; ff++)
            StopAmbientSound(ff);
    }

    cancel_all_scripts();
    numevents = 0;  // cancel any pending room events

    if (roomBackgroundBmp != nullptr)
    {
        gfxDriver->DestroyDDB(roomBackgroundBmp);
        roomBackgroundBmp = nullptr;
    }

    if (croom==nullptr) ;
    else if (roominst!=nullptr) {
        save_room_data_segment();
        delete roominstFork;
        delete roominst;
        roominstFork = nullptr;
        roominst=nullptr;
    }
    else croom->tsdatasize=0;
    memset(&play.walkable_areas_on[0],1,MAX_WALK_AREAS+1);
    play.bg_frame=0;
    play.bg_frame_locked=0;
    remove_screen_overlay(-1);
    delete raw_saved_screen;
    raw_saved_screen = nullptr;
    for (ff = 0; ff < MAX_ROOM_BGFRAMES; ff++)
        play.raw_modified[ff] = 0;

    // wipe the character cache when we change rooms
    for (ff = 0; ff < game.numcharacters; ff++) {
        if (charcache[ff].inUse) {
            delete charcache[ff].image;
            charcache[ff].image = nullptr;
            charcache[ff].inUse = 0;
        }
        // ensure that any half-moves (eg. with scaled movement) are stopped
        charextra[ff].xwas = INVALID_X;
    }

    play.swap_portrait_lastchar = -1;
    play.swap_portrait_lastlastchar = -1;

    for (ff = 0; ff < croom->numobj; ff++) {
        // un-export the object's script object
        if (objectScriptObjNames[ff].IsEmpty())
            continue;

        ccRemoveExternalSymbol(objectScriptObjNames[ff]);
    }

    for (ff = 0; ff < MAX_ROOM_HOTSPOTS; ff++) {
        if (thisroom.Hotspots[ff].ScriptName.IsEmpty())
            continue;

        ccRemoveExternalSymbol(thisroom.Hotspots[ff].ScriptName);
    }

    croom_ptr_clear();

    // clear the object cache
    for (ff = 0; ff < MAX_ROOM_OBJECTS; ff++) {
        delete objcache[ff].image;
        objcache[ff].image = nullptr;
    }
    // clear the actsps buffers to save memory, since the
    // objects/characters involved probably aren't on the
    // new screen. this also ensures all cached data is flushed
    for (ff = 0; ff < MAX_ROOM_OBJECTS + game.numcharacters; ff++) {
        delete actsps[ff];
        actsps[ff] = nullptr;

        if (actspsbmp[ff] != nullptr)
            gfxDriver->DestroyDDB(actspsbmp[ff]);
        actspsbmp[ff] = nullptr;

        delete actspswb[ff];
        actspswb[ff] = nullptr;

        if (actspswbbmp[ff] != nullptr)
            gfxDriver->DestroyDDB(actspswbbmp[ff]);
        actspswbbmp[ff] = nullptr;

        actspswbcache[ff].valid = 0;
    }

    // if Hide Player Character was ticked, restore it to visible
    if (play.temporarily_turned_off_character >= 0) {
        game.chars[play.temporarily_turned_off_character].on = 1;
        play.temporarily_turned_off_character = -1;
    }

}

extern int convert_16bit_bgr;

void update_letterbox_mode()
{
    const Size real_room_sz = Size(thisroom.Width, thisroom.Height);
    const Rect game_frame = RectWH(game.GetGameRes());
    Rect new_main_view = game_frame;
    // In the original engine the letterbox feature only allowed viewports of
    // either 200 or 240 (400 and 480) pixels, if the room height was equal or greater than 200 (400).
    // Also, the UI viewport should be matching room viewport in that case.
    // NOTE: if "OPT_LETTERBOX" is false, altsize.Height = size.Height always.
    const int viewport_height =
        real_room_sz.Height < game.GetLetterboxSize().Height ? real_room_sz.Height :
        (real_room_sz.Height >= game.GetLetterboxSize().Height && real_room_sz.Height < game.GetGameRes().Height) ? game.GetLetterboxSize().Height :
        game.GetGameRes().Height;
    new_main_view.SetHeight(viewport_height);

    play.SetMainViewport(CenterInRect(game_frame, new_main_view));
    play.SetUIViewport(new_main_view);
}

// Automatically reset primary room viewport and camera to match the new room size
static void adjust_viewport_to_room()
{
    const Size real_room_sz = Size(thisroom.Width, thisroom.Height);
    const Rect main_view = play.GetMainViewport();
    Rect new_room_view = RectWH(Size::Clamp(real_room_sz, Size(1, 1), main_view.GetSize()));

    auto view = play.GetRoomViewport(0);
    view->SetRect(new_room_view);
    auto cam = view->GetCamera();
    if (cam)
    {
        cam->SetSize(new_room_view.GetSize());
        cam->SetAt(0, 0);
        cam->Release();
    }
}

// Run through all viewports and cameras to make sure they can work in new room's bounds
static void update_all_viewcams_with_newroom()
{
    for (int i = 0; i < play.GetRoomCameraCount(); ++i)
    {
        auto cam = play.GetRoomCamera(i);
        const Rect old_pos = cam->GetRect();
        cam->SetSize(old_pos.GetSize());
        cam->SetAt(old_pos.Left, old_pos.Top);
    }
}

// forchar = playerchar on NewRoom, or NULL if restore saved game
void load_new_room(int newnum, CharacterInfo*forchar) {

    debug_script_log("Loading room %d", newnum);

    String room_filename;
    int cc;
    done_es_error = 0;
    play.room_changes ++;
    // TODO: find out why do we need to temporarily lower color depth to 8-bit.
    // Or do we? There's a serious usability problem in this: if any bitmap is
    // created meanwhile it will have this color depth by default, which may
    // lead to unexpected errors.
    set_color_depth(8);
    displayed_room=newnum;

    room_filename.Format("room%d.crm", newnum);

    update_polled_stuff_if_runtime();

    // load the room from disk
    our_eip=200;
    thisroom.GameID = NO_GAME_ID_IN_ROOM_FILE;
    load_room(room_filename, &thisroom, game.SpriteInfos);

    if ((thisroom.GameID != NO_GAME_ID_IN_ROOM_FILE) &&
        (thisroom.GameID != game.uniqueid)) {
            quitprintf("!Unable to load '%s'. This room file is assigned to a different game.", room_filename.GetCStr());
    }

    update_polled_stuff_if_runtime();
    our_eip=201;
    /*  // apparently, doing this stops volume spiking between tracks
    if (thisroom.Options.StartupMusic>0) {
    stopmusic();
    delay(100);
    }*/

    play.room_width = thisroom.Width;
    play.room_height = thisroom.Height;
    play.anim_background_speed = thisroom.BgAnimSpeed;
    play.bg_anim_delay = play.anim_background_speed;

    // do the palette
    for (cc=0;cc<256;cc++) {
        if (game.paluses[cc]==PAL_BACKGROUND)
            palette[cc]=thisroom.Palette[cc];
        else {
            // copy the gamewide colours into the room palette
            for (size_t i = 0; i < thisroom.BgFrameCount; ++i)
                thisroom.BgFrames[i].Palette[cc] = palette[cc];
        }
    }

    for (size_t i = 0; i < thisroom.BgFrameCount; ++i) {
        update_polled_stuff_if_runtime();
        thisroom.BgFrames[i].Graphic = PrepareSpriteForUse(thisroom.BgFrames[i].Graphic, false);
    }

    update_polled_stuff_if_runtime();

    our_eip=202;
    // Update game viewports
    if (game.IsLegacyLetterbox())
        update_letterbox_mode();
    SetMouseBounds(0, 0, 0, 0);

    our_eip=203;
    in_new_room=1;

    set_color_depth(game.GetColorDepth());

    // walkable_areas_temp is used by the pathfinder to generate a
    // copy of the walkable areas - allocate it here to save time later
    delete walkable_areas_temp;
    walkable_areas_temp = BitmapHelper::CreateBitmap(thisroom.WalkAreaMask->GetWidth(), thisroom.WalkAreaMask->GetHeight(), 8);

    // Make a backup copy of the walkable areas prior to
    // any RemoveWalkableArea commands
    delete walkareabackup;
    // copy the walls screen
    walkareabackup=BitmapHelper::CreateBitmapCopy(thisroom.WalkAreaMask.get());

    our_eip=204;
    update_polled_stuff_if_runtime();
    redo_walkable_areas();
    update_polled_stuff_if_runtime();
    recache_walk_behinds();
    update_polled_stuff_if_runtime();

    our_eip=205;
    // setup objects
    if (forchar != nullptr) {
        // if not restoring a game, always reset this room
        troom.beenhere=0;  
        troom.FreeScriptData();
        troom.FreeProperties();
        memset(&troom.hotspot_enabled[0],1,MAX_ROOM_HOTSPOTS);
        memset(&troom.region_enabled[0], 1, MAX_ROOM_REGIONS);
    }
    if ((newnum>=0) & (newnum<MAX_ROOMS))
        croom = getRoomStatus(newnum);
    else croom=&troom;

    if (croom->beenhere==0) {
        croom->numobj=thisroom.ObjectCount;
        croom->tsdatasize=0;
        for (cc=0;cc<croom->numobj;cc++) {
            croom->obj[cc].x=thisroom.Objects[cc].X;
            croom->obj[cc].y=thisroom.Objects[cc].Y;
            croom->obj[cc].num = Math::InRangeOrDef<uint16_t>(thisroom.Objects[cc].Sprite, 0);
            croom->obj[cc].on=thisroom.Objects[cc].IsOn;
            croom->obj[cc].view=-1;
            croom->obj[cc].loop=0;
            croom->obj[cc].frame=0;
            croom->obj[cc].wait=0;
            croom->obj[cc].transparent=0;
            croom->obj[cc].moving=-1;
            croom->obj[cc].flags = thisroom.Objects[cc].Flags;
            croom->obj[cc].baseline=-1;
            croom->obj[cc].zoom = 100;
            croom->obj[cc].last_width = 0;
            croom->obj[cc].last_height = 0;
            croom->obj[cc].blocking_width = 0;
            croom->obj[cc].blocking_height = 0;
            if (thisroom.Objects[cc].Baseline>=0)
                croom->obj[cc].baseline=thisroom.Objects[cc].Baseline;
            croom->obj[cc].blend_mode = thisroom.Objects[cc].BlendMode;
            croom->obj[cc].UpdateGraphicSpace();
            if (thisroom.Objects[cc].Sprite > UINT16_MAX)
                debug_script_warn("Warning: object's (id %d) sprite %d outside of internal range (%d), reset to 0",
                    cc, thisroom.Objects[cc].Sprite, UINT16_MAX);
        }
        for (size_t i = 0; i < (size_t)MAX_WALK_BEHINDS; ++i)
            croom->walkbehind_base[i] = thisroom.WalkBehinds[i].Baseline;
        for (cc=0;cc<MAX_FLAGS;cc++) croom->flagstates[cc]=0;

        /*    // we copy these structs for the Score column to work
        croom->misccond=thisroom.misccond;
        for (cc=0;cc<MAX_ROOM_HOTSPOTS;cc++)
        croom->hscond[cc]=thisroom.hscond[cc];
        for (cc=0;cc<MAX_ROOM_OBJECTS;cc++)
        croom->objcond[cc]=thisroom.objcond[cc];*/

        for (cc=0;cc < MAX_ROOM_HOTSPOTS;cc++) {
            croom->hotspot_enabled[cc] = 1;
        }
        for (cc = 0; cc < MAX_ROOM_REGIONS; cc++) {
            croom->region_enabled[cc] = 1;
        }

        croom->beenhere=1;
        in_new_room=2;
    }

    update_polled_stuff_if_runtime();

    objs=&croom->obj[0];

    for (cc = 0; cc < MAX_ROOM_OBJECTS; cc++) {
        // 64 bit: Using the id instead
        // scrObj[cc].obj = &croom->obj[cc];
        objectScriptObjNames[cc].Free();
    }

    for (cc = 0; cc < croom->numobj; cc++) {
        // export the object's script object
        if (thisroom.Objects[cc].ScriptName.IsEmpty())
            continue;
        objectScriptObjNames[cc] = thisroom.Objects[cc].ScriptName;
        ccAddExternalDynamicObject(objectScriptObjNames[cc], &scrObj[cc], &ccDynamicObject);
    }

    for (cc = 0; cc < MAX_ROOM_HOTSPOTS; cc++) {
        if (thisroom.Hotspots[cc].ScriptName.IsEmpty())
            continue;

        ccAddExternalDynamicObject(thisroom.Hotspots[cc].ScriptName, &scrHotspot[cc], &ccDynamicHotspot);
    }

    our_eip=206;
    /*  THIS IS DONE IN THE EDITOR NOW
    thisroom.BgFrames.IsPaletteShared[0] = 1;
    for (dd = 1; dd < thisroom.BgFrameCount; dd++) {
    if (memcmp (&thisroom.BgFrames.Palette[dd][0], &palette[0], sizeof(color) * 256) == 0)
    thisroom.BgFrames.IsPaletteShared[dd] = 1;
    else
    thisroom.BgFrames.IsPaletteShared[dd] = 0;
    }
    // only make the first frame shared if the last is
    if (thisroom.BgFrames.IsPaletteShared[thisroom.BgFrameCount - 1] == 0)
    thisroom.BgFrames.IsPaletteShared[0] = 0;*/

    update_polled_stuff_if_runtime();

    our_eip = 210;
    if (IS_ANTIALIAS_SPRITES) {
        // sometimes the palette has corrupt entries, which crash
        // the create_rgb_table call
        // so, fix them
        for (int ff = 0; ff < 256; ff++) {
            if (palette[ff].r > 63)
                palette[ff].r = 63;
            if (palette[ff].g > 63)
                palette[ff].g = 63;
            if (palette[ff].b > 63)
                palette[ff].b = 63;
        }
        create_rgb_table (&rgb_table, palette, nullptr);
        rgb_map = &rgb_table;
    }
    our_eip = 211;
    if (forchar!=nullptr) {
        // if it's not a Restore Game

        // if a following character is still waiting to come into the
        // previous room, force it out so that the timer resets
        for (int ff = 0; ff < game.numcharacters; ff++) {
            if ((game.chars[ff].following >= 0) && (game.chars[ff].room < 0)) {
                if ((game.chars[ff].following == game.playercharacter) &&
                    (forchar->prevroom == newnum))
                    // the player went back to the previous room, so make sure
                    // the following character is still there
                    game.chars[ff].room = newnum;
                else
                    game.chars[ff].room = game.chars[game.chars[ff].following].room;
            }
        }

        forchar->prevroom=forchar->room;
        forchar->room=newnum;
        // only stop moving if it's a new room, not a restore game
        for (cc=0;cc<game.numcharacters;cc++)
            StopMoving(cc);

    }

    update_polled_stuff_if_runtime();

    roominst=nullptr;
    if (debug_flags & DBG_NOSCRIPT) ;
    else if (thisroom.CompiledScript!=nullptr) {
        compile_room_script();
        if (croom->tsdatasize>0) {
            if (croom->tsdatasize != roominst->globaldatasize)
                quit("room script data segment size has changed");
            memcpy(&roominst->globaldata[0],croom->tsdata,croom->tsdatasize);
        }
    }
    our_eip=207;
    play.entered_edge = -1;

    if ((new_room_x != SCR_NO_VALUE) && (forchar != nullptr))
    {
        forchar->x = new_room_x;
        forchar->y = new_room_y;

		if (new_room_loop != SCR_NO_VALUE)
			forchar->loop = new_room_loop;
    }
    new_room_x = SCR_NO_VALUE;
	new_room_loop = SCR_NO_VALUE;

    if ((new_room_pos>0) & (forchar!=nullptr)) {
        if (new_room_pos>=4000) {
            play.entered_edge = 3;
            forchar->y = thisroom.Edges.Top + 1;
            forchar->x=new_room_pos%1000;
            if (forchar->x==0) forchar->x=thisroom.Width/2;
            if (forchar->x <= thisroom.Edges.Left)
                forchar->x = thisroom.Edges.Left + 3;
            if (forchar->x >= thisroom.Edges.Right)
                forchar->x = thisroom.Edges.Right - 3;
            forchar->loop=0;
        }
        else if (new_room_pos>=3000) {
            play.entered_edge = 2;
            forchar->y = thisroom.Edges.Bottom - 1;
            forchar->x=new_room_pos%1000;
            if (forchar->x==0) forchar->x=thisroom.Width/2;
            if (forchar->x <= thisroom.Edges.Left)
                forchar->x = thisroom.Edges.Left + 3;
            if (forchar->x >= thisroom.Edges.Right)
                forchar->x = thisroom.Edges.Right - 3;
            forchar->loop=3;
        }
        else if (new_room_pos>=2000) {
            play.entered_edge = 1;
            forchar->x = thisroom.Edges.Right - 1;
            forchar->y=new_room_pos%1000;
            if (forchar->y==0) forchar->y=thisroom.Height/2;
            if (forchar->y <= thisroom.Edges.Top)
                forchar->y = thisroom.Edges.Top + 3;
            if (forchar->y >= thisroom.Edges.Bottom)
                forchar->y = thisroom.Edges.Bottom - 3;
            forchar->loop=1;
        }
        else if (new_room_pos>=1000) {
            play.entered_edge = 0;
            forchar->x = thisroom.Edges.Left + 1;
            forchar->y=new_room_pos%1000;
            if (forchar->y==0) forchar->y=thisroom.Height/2;
            if (forchar->y <= thisroom.Edges.Top)
                forchar->y = thisroom.Edges.Top + 3;
            if (forchar->y >= thisroom.Edges.Bottom)
                forchar->y = thisroom.Edges.Bottom - 3;
            forchar->loop=2;
        }
        // if starts on un-walkable area
        if (get_walkable_area_pixel(forchar->x, forchar->y) == 0) {
            if (new_room_pos>=3000) { // bottom or top of screen
                int tryleft=forchar->x - 1,tryright=forchar->x + 1;
                while (1) {
                    if (get_walkable_area_pixel(tryleft, forchar->y) > 0) {
                        forchar->x=tryleft; break; }
                    if (get_walkable_area_pixel(tryright, forchar->y) > 0) {
                        forchar->x=tryright; break; }
                    int nowhere=0;
                    if (tryleft>thisroom.Edges.Left) { tryleft--; nowhere++; }
                    if (tryright<thisroom.Edges.Right) { tryright++; nowhere++; }
                    if (nowhere==0) break;  // no place to go, so leave him
                }
            }
            else if (new_room_pos>=1000) { // left or right
                int tryleft=forchar->y - 1,tryright=forchar->y + 1;
                while (1) {
                    if (get_walkable_area_pixel(forchar->x, tryleft) > 0) {
                        forchar->y=tryleft; break; }
                    if (get_walkable_area_pixel(forchar->x, tryright) > 0) {
                        forchar->y=tryright; break; }
                    int nowhere=0;
                    if (tryleft>thisroom.Edges.Top) { tryleft--; nowhere++; }
                    if (tryright<thisroom.Edges.Bottom) { tryright++; nowhere++; }
                    if (nowhere==0) break;  // no place to go, so leave him
                }
            }
        }
        new_room_pos=0;
    }
    if (forchar!=nullptr) {
        play.entered_at_x=forchar->x;
        play.entered_at_y=forchar->y;
        if (forchar->x >= thisroom.Edges.Right)
            play.entered_edge = 1;
        else if (forchar->x <= thisroom.Edges.Left)
            play.entered_edge = 0;
        else if (forchar->y >= thisroom.Edges.Bottom)
            play.entered_edge = 2;
        else if (forchar->y <= thisroom.Edges.Top)
            play.entered_edge = 3;
    }
    if (thisroom.Options.StartupMusic>0)
        PlayMusicResetQueue(thisroom.Options.StartupMusic);

    our_eip=208;
    if (forchar!=nullptr) {
        if (thisroom.Options.PlayerCharOff==0) { forchar->on=1;
        enable_cursor_mode(0); }
        else {
            forchar->on=0;
            disable_cursor_mode(0);
            // remember which character we turned off, in case they
            // use SetPlyaerChracter within this room (so we re-enable
            // the correct character when leaving the room)
            play.temporarily_turned_off_character = game.playercharacter;
        }
        if (forchar->flags & CHF_FIXVIEW) ;
        else if (thisroom.Options.PlayerView==0) forchar->view=forchar->defview;
        else forchar->view=thisroom.Options.PlayerView-1;
        forchar->frame=0;   // make him standing
    }
    color_map = nullptr;

    our_eip = 209;
    update_polled_stuff_if_runtime();
    generate_light_table();
    update_music_volume();

    // If we are not restoring a save, update cameras to accomodate for this
    // new room; otherwise this is done later when cameras are recreated.
    if (forchar != nullptr)
    {
        if (play.IsAutoRoomViewport())
            adjust_viewport_to_room();
        update_all_viewcams_with_newroom();
        play.UpdateRoomCameras(); // update auto tracking
    }
    init_room_drawdata();

    our_eip = 212;
    invalidate_screen();
    for (cc=0;cc<croom->numobj;cc++) {
        if (objs[cc].on == 2)
            MergeObject(cc);
    }
    new_room_flags=0;
    play.gscript_timer=-1;  // avoid screw-ups with changing screens
    play.player_on_region = 0;
    // trash any input which they might have done while it was loading
    ags_clear_input_buffer();
    // no fade in, so set the palette immediately in case of 256-col sprites
    if (game.color_depth > 1)
        setpal();

    our_eip=220;
    update_polled_stuff_if_runtime();
    debug_script_log("Now in room %d", displayed_room);
    GUI::MarkAllGUIForUpdate();
    pl_run_plugin_hooks(AGSE_ENTERROOM, displayed_room);
    //  MoveToWalkableArea(game.playercharacter);
    //  MSS_CHECK_ALL_BLOCKS;
}

extern int psp_clear_cache_on_room_change;

// new_room: changes the current room number, and loads the new room from disk
void new_room(int newnum,CharacterInfo*forchar) {
    EndSkippingUntilCharStops();

    debug_script_log("Room change requested to room %d", newnum);

    update_polled_stuff_if_runtime();

    // we are currently running Leaves Screen scripts
    in_leaves_screen = newnum;

    // player leaves screen event
    run_room_event(8);
    // Run the global OnRoomLeave event
    run_on_event (GE_LEAVE_ROOM, RuntimeScriptValue().SetInt32(displayed_room));

    pl_run_plugin_hooks(AGSE_LEAVEROOM, displayed_room);

    // update the new room number if it has been altered by OnLeave scripts
    newnum = in_leaves_screen;
    in_leaves_screen = -1;

    if ((playerchar->following >= 0) &&
        (game.chars[playerchar->following].room != newnum)) {
            // the player character is following another character,
            // who is not in the new room. therefore, abort the follow
            playerchar->following = -1;
    }
    update_polled_stuff_if_runtime();

    // change rooms
    unload_old_room();

    if (psp_clear_cache_on_room_change)
    {
        // Delete all cached sprites
        spriteset.DisposeAll();

        // Delete all gui background images
        for (int i = 0; i < game.numgui; i++)
        {
            delete guibg[i];
            guibg[i] = nullptr;

            if (guibgbmp[i])
                gfxDriver->DestroyDDB(guibgbmp[i]);
            guibgbmp[i] = nullptr;
        }
        GUI::MarkAllGUIForUpdate();
    }

    update_polled_stuff_if_runtime();

    load_new_room(newnum,forchar);
}

int find_highest_room_entered() {
    int qq,fndas=-1;
    for (qq=0;qq<MAX_ROOMS;qq++) {
        if (isRoomStatusValid(qq) && (getRoomStatus(qq)->beenhere != 0))
            fndas = qq;
    }
    // This is actually legal - they might start in room 400 and save
    //if (fndas<0) quit("find_highest_room: been in no rooms?");
    return fndas;
}

void first_room_initialization() {
    starting_room = displayed_room;
    set_loop_counter(0);
    mouse_z_was = sys_mouse_z;
}

void check_new_room() {
    // if they're in a new room, run Player Enters Screen and on_event(ENTER_ROOM)
    if ((in_new_room>0) & (in_new_room!=3)) {
        EventHappened evh;
        evh.type = EV_RUNEVBLOCK;
        evh.data1 = EVB_ROOM;
        evh.data2 = 0;
        evh.data3 = 5;
        evh.player=game.playercharacter;
        // make sure that any script calls don't re-call enters screen
        int newroom_was = in_new_room;
        in_new_room = 0;
        play.disabled_user_interface ++;
        process_event(&evh);
        play.disabled_user_interface --;
        in_new_room = newroom_was;
        //    setevent(EV_RUNEVBLOCK,EVB_ROOM,0,5);
    }
}

void compile_room_script() {
    ccError = 0;

    roominst = ccInstance::CreateFromScript(thisroom.CompiledScript);

    if ((ccError!=0) || (roominst==nullptr)) {
        quitprintf("Unable to create local script: %s", ccErrorString.GetCStr());
    }

    roominstFork = roominst->Fork();
    if (roominstFork == nullptr)
        quitprintf("Unable to create forked room instance: %s", ccErrorString.GetCStr());

    repExecAlways.roomHasFunction = true;
    lateRepExecAlways.roomHasFunction = true;
    getDialogOptionsDimensionsFunc.roomHasFunction = true;
}

int bg_just_changed = 0;

void on_background_frame_change () {

    invalidate_screen();
    mark_current_background_dirty();
    invalidate_cached_walkbehinds();

    // get the new frame's palette
    memcpy (palette, thisroom.BgFrames[play.bg_frame].Palette, sizeof(RGB) * 256);

    // hi-colour, update the palette. It won't have an immediate effect
    // but will be drawn properly when the screen fades in
    if (game.color_depth > 1)
        setpal();

    if (in_enters_screen)
        return;

    // Don't update the palette if it hasn't changed
    if (thisroom.BgFrames[play.bg_frame].IsPaletteShared)
        return;

    // 256-colours, tell it to update the palette (will actually be done as
    // close as possible to the screen update to prevent flicker problem)
    if (game.color_depth == 1)
        bg_just_changed = 1;
}

void croom_ptr_clear()
{
    croom = nullptr;
    objs = nullptr;
}


AGS_INLINE int room_to_mask_coord(int coord)
{
    return coord / thisroom.MaskResolution;
}

AGS_INLINE int mask_to_room_coord(int coord)
{
    return coord * thisroom.MaskResolution;
}

void convert_move_path_to_room_resolution(MoveList *ml)
{
    if (thisroom.MaskResolution <= 1)
        return;

    ml->fromx = mask_to_room_coord(ml->fromx);
    ml->fromy = mask_to_room_coord(ml->fromy);
    ml->lastx = mask_to_room_coord(ml->lastx);
    ml->lasty = mask_to_room_coord(ml->lasty);

    for (int i = 0; i < ml->numstage; i++)
    {
        uint16_t lowPart = mask_to_room_coord(ml->pos[i] & 0x0000ffff);
        uint16_t highPart = mask_to_room_coord((ml->pos[i] >> 16) & 0x0000ffff);
        ml->pos[i] = ((int)highPart << 16) | (lowPart & 0x0000ffff);
    }

    if (game.options[OPT_WALKSPEEDABSOLUTE] == 0)
    { // Speeds are scaling with MaskResolution
        for (int i = 0; i < ml->numstage; i++)
        {
            ml->xpermove[i] = mask_to_room_coord(ml->xpermove[i]);
            ml->ypermove[i] = mask_to_room_coord(ml->ypermove[i]);
        }
    }
}

//=============================================================================
//
// Script API Functions
//
//=============================================================================

#include "debug/out.h"
#include "script/script_api.h"
#include "script/script_runtime.h"
#include "ac/dynobj/scriptstring.h"

extern ScriptString myScriptStringImpl;

// ScriptDrawingSurface* (int backgroundNumber)
RuntimeScriptValue Sc_Room_GetDrawingSurfaceForBackground(const RuntimeScriptValue *params, int32_t param_count)
{
    API_SCALL_OBJAUTO_PINT(ScriptDrawingSurface, Room_GetDrawingSurfaceForBackground);
}

// int (const char *property)
RuntimeScriptValue Sc_Room_GetProperty(const RuntimeScriptValue *params, int32_t param_count)
{
    API_SCALL_INT_POBJ(Room_GetProperty, const char);
}

// const char* (const char *property)
RuntimeScriptValue Sc_Room_GetTextProperty(const RuntimeScriptValue *params, int32_t param_count)
{
    API_SCALL_OBJ_POBJ(const char, myScriptStringImpl, Room_GetTextProperty, const char);
}

RuntimeScriptValue Sc_Room_SetProperty(const RuntimeScriptValue *params, int32_t param_count)
{
    API_SCALL_BOOL_POBJ_PINT(Room_SetProperty, const char);
}

// const char* (const char *property)
RuntimeScriptValue Sc_Room_SetTextProperty(const RuntimeScriptValue *params, int32_t param_count)
{
    API_SCALL_BOOL_POBJ2(Room_SetTextProperty, const char, const char);
}

// int ()
RuntimeScriptValue Sc_Room_GetBottomEdge(const RuntimeScriptValue *params, int32_t param_count)
{
    API_SCALL_INT(Room_GetBottomEdge);
}

// int ()
RuntimeScriptValue Sc_Room_GetColorDepth(const RuntimeScriptValue *params, int32_t param_count)
{
    API_SCALL_INT(Room_GetColorDepth);
}

// int ()
RuntimeScriptValue Sc_Room_GetHeight(const RuntimeScriptValue *params, int32_t param_count)
{
    API_SCALL_INT(Room_GetHeight);
}

// int ()
RuntimeScriptValue Sc_Room_GetLeftEdge(const RuntimeScriptValue *params, int32_t param_count)
{
    API_SCALL_INT(Room_GetLeftEdge);
}

// const char* (int index)
RuntimeScriptValue Sc_Room_GetMessages(const RuntimeScriptValue *params, int32_t param_count)
{
    API_SCALL_OBJ_PINT(const char, myScriptStringImpl, Room_GetMessages);
}

// int ()
RuntimeScriptValue Sc_Room_GetMusicOnLoad(const RuntimeScriptValue *params, int32_t param_count)
{
    API_SCALL_INT(Room_GetMusicOnLoad);
}

// int ()
RuntimeScriptValue Sc_Room_GetObjectCount(const RuntimeScriptValue *params, int32_t param_count)
{
    API_SCALL_INT(Room_GetObjectCount);
}

// int ()
RuntimeScriptValue Sc_Room_GetRightEdge(const RuntimeScriptValue *params, int32_t param_count)
{
    API_SCALL_INT(Room_GetRightEdge);
}

// int ()
RuntimeScriptValue Sc_Room_GetTopEdge(const RuntimeScriptValue *params, int32_t param_count)
{
    API_SCALL_INT(Room_GetTopEdge);
}

// int ()
RuntimeScriptValue Sc_Room_GetWidth(const RuntimeScriptValue *params, int32_t param_count)
{
    API_SCALL_INT(Room_GetWidth);
}

// void (int xx,int yy,int mood)
RuntimeScriptValue Sc_RoomProcessClick(const RuntimeScriptValue *params, int32_t param_count)
{
    API_SCALL_VOID_PINT3(RoomProcessClick);
}

// bool (int room)
RuntimeScriptValue Sc_Room_Exists(const RuntimeScriptValue *params, int32_t param_count)
{
    API_SCALL_BOOL_PINT(Room_Exists);
}

void RegisterRoomAPI()
{
    ccAddExternalStaticFunction("Room::GetDrawingSurfaceForBackground^1",   Sc_Room_GetDrawingSurfaceForBackground);
    ccAddExternalStaticFunction("Room::GetProperty^1",                      Sc_Room_GetProperty);
    ccAddExternalStaticFunction("Room::GetTextProperty^1",                  Sc_Room_GetTextProperty);
    ccAddExternalStaticFunction("Room::SetProperty^2",                      Sc_Room_SetProperty);
    ccAddExternalStaticFunction("Room::SetTextProperty^2",                  Sc_Room_SetTextProperty);
    ccAddExternalStaticFunction("Room::ProcessClick^3",                     Sc_RoomProcessClick);
    ccAddExternalStaticFunction("ProcessClick",                             Sc_RoomProcessClick);
    ccAddExternalStaticFunction("Room::get_BottomEdge",                     Sc_Room_GetBottomEdge);
    ccAddExternalStaticFunction("Room::get_ColorDepth",                     Sc_Room_GetColorDepth);
    ccAddExternalStaticFunction("Room::get_Height",                         Sc_Room_GetHeight);
    ccAddExternalStaticFunction("Room::get_LeftEdge",                       Sc_Room_GetLeftEdge);
    ccAddExternalStaticFunction("Room::geti_Messages",                      Sc_Room_GetMessages);
    ccAddExternalStaticFunction("Room::get_MusicOnLoad",                    Sc_Room_GetMusicOnLoad);
    ccAddExternalStaticFunction("Room::get_ObjectCount",                    Sc_Room_GetObjectCount);
    ccAddExternalStaticFunction("Room::get_RightEdge",                      Sc_Room_GetRightEdge);
    ccAddExternalStaticFunction("Room::get_TopEdge",                        Sc_Room_GetTopEdge);
    ccAddExternalStaticFunction("Room::get_Width",                          Sc_Room_GetWidth);
    ccAddExternalStaticFunction("Room::Exists",                             Sc_Room_Exists);

    /* ----------------------- Registering unsafe exports for plugins -----------------------*/

    ccAddExternalFunctionForPlugin("Room::GetDrawingSurfaceForBackground^1",   (void*)Room_GetDrawingSurfaceForBackground);
    ccAddExternalFunctionForPlugin("Room::GetProperty^1",                      (void*)Room_GetProperty);
    ccAddExternalFunctionForPlugin("Room::GetTextProperty^1",                  (void*)Room_GetTextProperty);
    ccAddExternalFunctionForPlugin("Room::get_BottomEdge",                     (void*)Room_GetBottomEdge);
    ccAddExternalFunctionForPlugin("Room::get_ColorDepth",                     (void*)Room_GetColorDepth);
    ccAddExternalFunctionForPlugin("Room::get_Height",                         (void*)Room_GetHeight);
    ccAddExternalFunctionForPlugin("Room::get_LeftEdge",                       (void*)Room_GetLeftEdge);
    ccAddExternalFunctionForPlugin("Room::geti_Messages",                      (void*)Room_GetMessages);
    ccAddExternalFunctionForPlugin("Room::get_MusicOnLoad",                    (void*)Room_GetMusicOnLoad);
    ccAddExternalFunctionForPlugin("Room::get_ObjectCount",                    (void*)Room_GetObjectCount);
    ccAddExternalFunctionForPlugin("Room::get_RightEdge",                      (void*)Room_GetRightEdge);
    ccAddExternalFunctionForPlugin("Room::get_TopEdge",                        (void*)Room_GetTopEdge);
    ccAddExternalFunctionForPlugin("Room::get_Width",                          (void*)Room_GetWidth);
    ccAddExternalFunctionForPlugin("Room::Exists",                             (void*)Room_Exists);
}

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
#include "ac/character.h"
#include "ac/charactercache.h"
#include "ac/dialog.h"
#include "ac/draw.h"
#include "ac/file.h"
#include "ac/game.h"
#include "ac/gamesetup.h"
#include "ac/gamesetupstruct.h"
#include "ac/gamestate.h"
#include "ac/gui.h"
#include "ac/movelist.h"
#include "ac/dynobj/all_dynamicclasses.h"
#include "ac/dynobj/all_scriptclasses.h"
#include "ac/statobj/agsstaticobject.h"
#include "ac/statobj/staticarray.h"
#include "debug/debug_log.h"
#include "debug/out.h"
#include "font/agsfontrenderer.h"
#include "font/fonts.h"
#include "game/game_init.h"
#include "gfx/bitmap.h"
#include "gfx/ddb.h"
#include "gui/guilabel.h"
#include "platform/base/agsplatformdriver.h"
#include "plugin/plugin_engine.h"
#include "script/cc_error.h"
#include "script/exports.h"
#include "script/script.h"
#include "script/script_runtime.h"
#include "util/string_utils.h"
#include "media/audio/audio_system.h"

using namespace Common;
using namespace Engine;

extern GameSetupStruct game;
extern int actSpsCount;
extern Bitmap **actsps;
extern IDriverDependantBitmap* *actspsbmp;
extern Bitmap **actspswb;
extern IDriverDependantBitmap* *actspswbbmp;
extern CachedActSpsData* actspswbcache;
extern CharacterCache *charcache;

extern CCGUIObject ccDynamicGUIObject;
extern CCCharacter ccDynamicCharacter;
extern CCHotspot   ccDynamicHotspot;
extern CCRegion    ccDynamicRegion;
extern CCInventory ccDynamicInv;
extern CCGUI       ccDynamicGUI;
extern CCObject    ccDynamicObject;
extern CCDialog    ccDynamicDialog;
extern CCAudioChannel ccDynamicAudio;
extern CCAudioClip ccDynamicAudioClip;
extern ScriptString myScriptStringImpl;
extern ScriptObject scrObj[MAX_ROOM_OBJECTS];
extern ScriptGUI    *scrGui;
extern ScriptHotspot scrHotspot[MAX_ROOM_HOTSPOTS];
extern ScriptRegion scrRegion[MAX_ROOM_REGIONS];
extern ScriptInvItem scrInv[MAX_INV];
extern ScriptAudioChannel scrAudioChannel[MAX_SOUND_CHANNELS + 1];

extern ScriptDialogOptionsRendering ccDialogOptionsRendering;
extern ScriptDrawingSurface* dialogOptionsRenderingSurface;

extern AGSStaticObject GlobalStaticManager;

extern StaticArray StaticCharacterArray;
extern StaticArray StaticObjectArray;
extern StaticArray StaticGUIArray;
extern StaticArray StaticHotspotArray;
extern StaticArray StaticRegionArray;
extern StaticArray StaticInventoryArray;
extern StaticArray StaticDialogArray;

extern std::vector<ccInstance *> moduleInst;
extern std::vector<RuntimeScriptValue> moduleRepExecAddr;

StaticArray StaticCharacterArray;
StaticArray StaticObjectArray;
StaticArray StaticGUIArray;
StaticArray StaticHotspotArray;
StaticArray StaticRegionArray;
StaticArray StaticInventoryArray;
StaticArray StaticDialogArray;


namespace AGS
{
namespace Engine
{

String GetGameInitErrorText(GameInitErrorType err)
{
    switch (err)
    {
    case kGameInitErr_NoError:
        return "No error.";
    case kGameInitErr_NoFonts:
        return "No fonts specified to be used in this game.";
    case kGameInitErr_TooManyAudioTypes:
        return "Too many audio types for this engine to handle.";
    case kGameInitErr_EntityInitFail:
        return "Failed to initialize game entities.";
    case kGameInitErr_TooManyPlugins:
        return "Too many plugins for this engine to handle.";
    case kGameInitErr_PluginNameInvalid:
        return "Plugin name is invalid.";
    case kGameInitErr_ScriptLinkFailed:
        return "Script link failed.";
    }
    return "Unknown error.";
}

// Initializes audio channels and clips and registers them in the script system
void InitAndRegisterAudioObjects()
{
    for (int i = 0; i <= MAX_SOUND_CHANNELS; ++i)
    {
        scrAudioChannel[i].id = i;

        ManagedObjectInfo objinfo;
        objinfo.obj_type = kScValDynamicObject;
        objinfo.object_manager = &ccDynamicAudio;
        objinfo.address = &scrAudioChannel[i];
        objinfo.buffer = nullptr;
        objinfo.buffer_size = 0;
        ccRegisterManagedObject2(objinfo); // NO DATA
    }

    for (size_t i = 0; i < game.audioClips.size(); ++i)
    {
        // Note that as of 3.5.0 data format the clip IDs are still restricted
        // to actual item index in array, so we don't make any difference
        // between game versions, for now.
        game.audioClips[i].id = i;

        ManagedObjectInfo objinfo;
        objinfo.obj_type = kScValDynamicObject;
        objinfo.object_manager = &ccDynamicAudioClip;
        objinfo.address = &game.audioClips[i];
        objinfo.buffer = nullptr;
        objinfo.buffer_size = 0;
        ccRegisterManagedObject2(objinfo); // NO DATA

/// GENERATED CODE EXT
        ccAddExternalDynamicObject(game.audioClips[i].scriptName, &game.audioClips[i], &ccDynamicAudioClip);
    }
}

// Initializes characters and registers them in the script system
void InitAndRegisterCharacters(const LoadedGameEntities &ents)
{
    characterScriptObjNames.resize(game.numcharacters);
    for (int i = 0; i < game.numcharacters; ++i)
    {
        game.chars[i].walking = 0;
        game.chars[i].animating = 0;
        game.chars[i].pic_xoffs = 0;
        game.chars[i].pic_yoffs = 0;
        game.chars[i].blinkinterval = 140;
        game.chars[i].blinktimer = game.chars[i].blinkinterval;
        game.chars[i].index_id = i;
        game.chars[i].blocking_width = 0;
        game.chars[i].blocking_height = 0;
        game.chars[i].prevroom = -1;
        game.chars[i].loop = 0;
        game.chars[i].frame = 0;
        game.chars[i].walkwait = -1;

        ManagedObjectInfo objinfo;
        objinfo.obj_type = kScValDynamicObject;
        objinfo.object_manager = &ccDynamicCharacter;
        objinfo.address = &game.chars[i];
        objinfo.buffer = &game.chars[i];
        objinfo.buffer_size = sizeof(CharacterInfo);
        ccRegisterManagedObject2(objinfo);

        // export the character's script object
        characterScriptObjNames[i] = game.chars[i].scrname;

        /// GENERATED CODE EXT
        ccAddExternalDynamicObject(characterScriptObjNames[i], &game.chars[i], &ccDynamicCharacter);
    }

    // extra character properties (because the characters are split into 2 structs now)
    if (ents.CharEx.size() > 0)
    {
        for (int i = 0; i < game.numcharacters; ++i)
        {
            charextra[i].blend_mode = ents.CharEx[i].BlendMode;
        }
    }
}

// Initializes dialog and registers them in the script system
void InitAndRegisterDialogs()
{
    scrDialog = new ScriptDialog[game.numdialog];
    for (int i = 0; i < game.numdialog; ++i)
    {
        scrDialog[i].id = i;
        scrDialog[i].reserved = 0;

        ManagedObjectInfo objinfo;
        objinfo.obj_type = kScValDynamicObject;
        objinfo.object_manager = &ccDynamicDialog;
        objinfo.address = &scrDialog[i];
        objinfo.buffer = &scrDialog[i];
        objinfo.buffer_size = sizeof(ScriptDialog);
        ccRegisterManagedObject2(objinfo);

        if (!game.dialogScriptNames[i].IsEmpty())
/// GENERATED CODE EXT
            ccAddExternalDynamicObject(game.dialogScriptNames[i], &scrDialog[i], &ccDynamicDialog);
    }
}

// Initializes dialog options rendering objects and registers them in the script system
void InitAndRegisterDialogOptions()
{
    {
    ManagedObjectInfo objinfo;
    objinfo.obj_type = kScValDynamicObject;
    objinfo.object_manager = &ccDialogOptionsRendering;
    objinfo.address = &ccDialogOptionsRendering;
    objinfo.buffer = nullptr;
    objinfo.buffer_size = 0;
    ccRegisterManagedObject2(objinfo);   // NO DATA
    }

    {
    dialogOptionsRenderingSurface = new ScriptDrawingSurface();
    dialogOptionsRenderingSurface->isLinkedBitmapOnly = true;

    ManagedObjectInfo objinfo;
    objinfo.obj_type = kScValDynamicObject;
    objinfo.object_manager = dialogOptionsRenderingSurface;
    objinfo.address = dialogOptionsRenderingSurface;
    objinfo.buffer = nullptr;
    objinfo.buffer_size = 0;
    auto dorsHandle = ccRegisterManagedObject2(objinfo);   // NO DATA

    ccAddObjectReference(dorsHandle);
    }
}

// Initializes gui and registers them in the script system
HError InitAndRegisterGUI()
{
    scrGui = (ScriptGUI*)malloc(sizeof(ScriptGUI) * game.numgui);
    for (int i = 0; i < game.numgui; ++i)
    {
        scrGui[i].id = -1;
    }

    guiScriptObjNames.resize(game.numgui);
    for (int i = 0; i < game.numgui; ++i)
    {
        // link controls to their parent guis
        HError err = guis[i].RebuildArray();
        if (!err)
            return err;
        // export all the GUI's controls
        export_gui_controls(i);
        // copy the script name to its own memory location
        // because ccAddExtSymbol only keeps a reference
        guiScriptObjNames[i] = guis[i].Name;
        scrGui[i].id = i;

        ManagedObjectInfo objinfo;
        objinfo.obj_type = kScValDynamicObject;
        objinfo.object_manager = &ccDynamicGUI;
        objinfo.address = &scrGui[i];
        objinfo.buffer = &scrGui[i];
        objinfo.buffer_size = sizeof(ScriptGUI);
        ccRegisterManagedObject2(objinfo);

        /// GENERATED CODE EXT
        ccAddExternalDynamicObject(guiScriptObjNames[i], &scrGui[i], &ccDynamicGUI);
    }
    return HError::None();
}

// Initializes inventory items and registers them in the script system
void InitAndRegisterInvItems()
{
    for (int i = 0; i < MAX_INV; ++i)
    {
        scrInv[i].id = i;
        scrInv[i].reserved = 0;

        ManagedObjectInfo objinfo;
        objinfo.obj_type = kScValDynamicObject;
        objinfo.object_manager = &ccDynamicInv;
        objinfo.address = &scrInv[i];
        objinfo.buffer = &scrInv[i];
        objinfo.buffer_size = sizeof(ScriptInvItem);
        ccRegisterManagedObject2(objinfo);

        if (!game.invScriptNames[i].IsEmpty())
            /// GENERATED CODE EXT
            ccAddExternalDynamicObject(game.invScriptNames[i], &scrInv[i], &ccDynamicInv);
    }
}

// Initializes room hotspots and registers them in the script system
void InitAndRegisterHotspots()
{
    for (int i = 0; i < MAX_ROOM_HOTSPOTS; ++i)
    {
        scrHotspot[i].id = i;
        scrHotspot[i].reserved = 0;

        ManagedObjectInfo objinfo;
        objinfo.obj_type = kScValDynamicObject;
        objinfo.object_manager = &ccDynamicHotspot;
        objinfo.address = &scrHotspot[i];
        objinfo.buffer = &scrHotspot[i];
        objinfo.buffer_size = sizeof(ScriptHotspot);
        ccRegisterManagedObject2(objinfo);
    }
}

// Initializes room objects and registers them in the script system
void InitAndRegisterRoomObjects()
{
    for (int i = 0; i < MAX_ROOM_OBJECTS; ++i)
    {
        ManagedObjectInfo objinfo;
        objinfo.obj_type = kScValDynamicObject;
        objinfo.object_manager = &ccDynamicObject;
        objinfo.address = &scrObj[i];
        objinfo.buffer = &scrObj[i];
        objinfo.buffer_size = sizeof(ScriptObject);
        ccRegisterManagedObject2(objinfo);
    }
}

// Initializes room regions and registers them in the script system
void InitAndRegisterRegions()
{
    for (int i = 0; i < MAX_ROOM_REGIONS; ++i)
    {
        scrRegion[i].id = i;
        scrRegion[i].reserved = 0;

        ManagedObjectInfo objinfo;
        objinfo.obj_type = kScValDynamicObject;
        objinfo.object_manager = &ccDynamicRegion;
        objinfo.address = &scrRegion[i];
        objinfo.buffer = &scrRegion[i];
        objinfo.buffer_size = sizeof(ScriptRegion);
        ccRegisterManagedObject2(objinfo);
    }
}

// Registers static entity arrays in the script system
void RegisterStaticArrays()
{
    // We need to know sizes of related script structs to convert memory offsets into object indexes.
    // These sized are calculated by the compiler based on script struct declaration.
    // Note, that only regular variables count to the struct size, NOT properties and NOT methods.
    // Currently there is no way to read the type sizes from script, so we have to define them by hand.
    // If the struct size changes in script, we must change the numbers here.
    // If we are going to support multiple different versions of same struct, then the "script size"
    // should be chosen depending on the script api version.
    const int charScriptSize = sizeof(int32_t) * 28 + sizeof(int16_t) * MAX_INV + sizeof(int32_t) + 61
        + 1; // + 1 for mem align
    const int dummyScriptSize = sizeof(int32_t) * 2; // 32-bit id + reserved int32

    // The current implementation of the StaticArray assumes we are dealing with regular C-arrays.
    // Therefore we need to know real struct size too. If we will change to std containers, then
    // (templated) telling real size will no longer be necessary.
    StaticCharacterArray.Create(&ccDynamicCharacter, charScriptSize, sizeof(CharacterInfo));
    StaticObjectArray.Create(&ccDynamicObject, dummyScriptSize, sizeof(ScriptObject));
    StaticGUIArray.Create(&ccDynamicGUI, dummyScriptSize, sizeof(ScriptGUI));
    StaticHotspotArray.Create(&ccDynamicHotspot, dummyScriptSize, sizeof(ScriptHotspot));
    StaticRegionArray.Create(&ccDynamicRegion, dummyScriptSize, sizeof(ScriptRegion));
    StaticInventoryArray.Create(&ccDynamicInv, dummyScriptSize, sizeof(ScriptInvItem));
    StaticDialogArray.Create(&ccDynamicDialog, dummyScriptSize, sizeof(ScriptDialog));

    ccAddExternalStaticArray("character",&game.chars[0], &StaticCharacterArray);
    coreExecutor.AddMemoryWindow(game.chars, sizeof(CharacterInfo)*(game.numcharacters+5), false);

    ccAddExternalStaticArray("object",&scrObj[0], &StaticObjectArray);
    coreExecutor.AddMemoryWindow(scrObj, sizeof(scrObj), false);

    ccAddExternalStaticArray("gui",&scrGui[0], &StaticGUIArray);
    coreExecutor.AddMemoryWindow(scrGui, sizeof(ScriptGUI)*game.numgui, false);

    ccAddExternalStaticArray("hotspot",&scrHotspot[0], &StaticHotspotArray);
    coreExecutor.AddMemoryWindow(scrHotspot, sizeof(scrHotspot), false);

    ccAddExternalStaticArray("region",&scrRegion[0], &StaticRegionArray);
    coreExecutor.AddMemoryWindow(scrRegion, sizeof(scrRegion), false);

    ccAddExternalStaticArray("inventory",&scrInv[0], &StaticInventoryArray);
    coreExecutor.AddMemoryWindow(scrInv, sizeof(scrInv), false);

    ccAddExternalStaticArray("dialog", &scrDialog[0], &StaticDialogArray);
    coreExecutor.AddMemoryWindow(scrDialog, sizeof(ScriptDialog)*game.numdialog, false);
}

// Initializes various game entities and registers them in the script system
HError InitAndRegisterGameEntities(const LoadedGameEntities &ents)
{
    InitAndRegisterAudioObjects();
    InitAndRegisterCharacters(ents);
    InitAndRegisterDialogs();
    InitAndRegisterDialogOptions();
    HError err = InitAndRegisterGUI();
    if (!err)
        return err;
    InitAndRegisterInvItems();

    InitAndRegisterHotspots();
    InitAndRegisterRegions();
    InitAndRegisterRoomObjects();
    play.CreatePrimaryViewportAndCamera();

    RegisterStaticArrays();

    setup_player_character(game.playercharacter);

    ccAddExternalStaticObject("player", &_sc_PlayerCharPtr, &GlobalStaticManager);
    coreExecutor.AddMemoryWindow(&_sc_PlayerCharPtr, sizeof(_sc_PlayerCharPtr), false);

    return HError::None();
}

void LoadFonts(GameDataVersion data_ver)
{
    for (int i = 0; i < game.numfonts; ++i)
    {
        if (!wloadfont_size(i, game.fonts[i]))
        // CLNUP decide what to do about arbitrary font scaling, might become an option
        /*
        */
            quitprintf("Unable to load font %d, no renderer could load a matching file", i);
    }
}

void AllocScriptModules()
{
    moduleInst.resize(numScriptModules, nullptr);
    moduleRepExecAddr.resize(numScriptModules);
    repExecAlways.moduleHasFunction.resize(numScriptModules, true);
    lateRepExecAlways.moduleHasFunction.resize(numScriptModules, true);
    getDialogOptionsDimensionsFunc.moduleHasFunction.resize(numScriptModules, true);
    renderDialogOptionsFunc.moduleHasFunction.resize(numScriptModules, true);
    getDialogOptionUnderCursorFunc.moduleHasFunction.resize(numScriptModules, true);
    runDialogOptionMouseClickHandlerFunc.moduleHasFunction.resize(numScriptModules, true);
    runDialogOptionKeyPressHandlerFunc.moduleHasFunction.resize(numScriptModules, true);
    runDialogOptionRepExecFunc.moduleHasFunction.resize(numScriptModules, true);
    for (int i = 0; i < numScriptModules; ++i)
    {
        moduleRepExecAddr[i].Invalidate();
    }
}

HGameInitError InitGameState(const LoadedGameEntities &ents, GameDataVersion data_ver)
{
    const ScriptAPIVersion base_api = (ScriptAPIVersion)game.options[OPT_BASESCRIPTAPI];
    const ScriptAPIVersion compat_api = (ScriptAPIVersion)game.options[OPT_SCRIPTCOMPATLEV];
    if (data_ver >= kGameVersion_341)
    {
        const char *base_api_name = GetScriptAPIName(base_api);
        const char *compat_api_name = GetScriptAPIName(compat_api);
        Debug::Printf(kDbgMsg_Info, "Requested script API: %s (%d), compat level: %s (%d)",
                    base_api >= 0 && base_api <= kScriptAPI_Current ? base_api_name : "unknown", base_api,
                    compat_api >= 0 && compat_api <= kScriptAPI_Current ? compat_api_name : "unknown", compat_api);
    }
    // If the game was compiled using unsupported version of the script API,
    // we warn about potential incompatibilities but proceed further.
    if (game.options[OPT_BASESCRIPTAPI] > kScriptAPI_Current)
        platform->DisplayAlert("Warning: this game requests a higher version of AGS script API, it may not run correctly or run at all.");

    //
    // 1. Check that the loaded data is valid and compatible with the current
    // engine capabilities.
    //
    if (game.numfonts == 0)
        return new GameInitError(kGameInitErr_NoFonts);
    if (game.audioClipTypes.size() > MAX_AUDIO_TYPES)
        return new GameInitError(kGameInitErr_TooManyAudioTypes, String::FromFormat("Required: %u, max: %d", game.audioClipTypes.size(), MAX_AUDIO_TYPES));

    //
    // CLNUP: this stage is removed
    // 3. Allocate and init game objects
    //
    charextra = (CharacterExtras*)calloc(game.numcharacters, sizeof(CharacterExtras));
    charcache = (CharacterCache*)calloc(1,sizeof(CharacterCache)*game.numcharacters+5);
    mls = (MoveList*)calloc(game.numcharacters + MAX_ROOM_OBJECTS + 1, sizeof(MoveList));
    actSpsCount = game.numcharacters + MAX_ROOM_OBJECTS + 2;
    actsps = (Bitmap **)calloc(actSpsCount, sizeof(Bitmap *));
    actspsbmp = (IDriverDependantBitmap**)calloc(actSpsCount, sizeof(IDriverDependantBitmap*));
    actspswb = (Bitmap **)calloc(actSpsCount, sizeof(Bitmap *));
    actspswbbmp = (IDriverDependantBitmap**)calloc(actSpsCount, sizeof(IDriverDependantBitmap*));
    actspswbcache = (CachedActSpsData*)calloc(actSpsCount, sizeof(CachedActSpsData));
    play.charProps.resize(game.numcharacters);
    HError err = InitAndRegisterGameEntities(ents);
    if (!err)
        return new GameInitError(kGameInitErr_EntityInitFail, err);
    LoadFonts(data_ver);

    //
    // 4. Initialize certain runtime variables
    //
    game_paused = 0;  // reset the game paused flag
    ifacepopped = -1;

    String svg_suffix;
    if (game.saveGameFileExtension[0] != 0)
        svg_suffix.Format(".%s", game.saveGameFileExtension);
    set_save_game_suffix(svg_suffix);

    play.score_sound = game.scoreClipID;
    play.fade_effect = game.options[OPT_FADETYPE];

    //
    // 5. Initialize runtime state of certain game objects
    //
    for (int i = 0; i < numguilabels; ++i)
    {
        // labels are not clickable by default
        guilabels[i].SetClickable(false);
    }
    play.gui_draw_order = (int*)calloc(game.numgui * sizeof(int), 1);
    update_gui_zorder();
    calculate_reserved_channel_count();

    //
    // 6. Register engine API exports
    // NOTE: we must do this before plugin start, because some plugins may
    // require access to script API at initialization time.
    //
    ccSetScriptAliveTimer(150000);
    ccSetStringClassImpl(&myScriptStringImpl);
    setup_script_exports(base_api, compat_api);

    //
    // 7. Start up plugins
    //
    pl_register_plugins(ents.PluginInfos);
    pl_startup_plugins();

    //
    // 8. Create script modules
    // NOTE: we must do this after plugins, because some plugins may export
    // script symbols too.
    //
    gamescript = ents.GlobalScript;
    dialogScriptsScript = ents.DialogScript;
    numScriptModules = ents.ScriptModules.size();
    scriptModules = ents.ScriptModules;
    AllocScriptModules();
    if (create_global_script())
        return new GameInitError(kGameInitErr_ScriptLinkFailed, ccErrorString);

    return HGameInitError::None();
}

} // namespace Engine
} // namespace AGS

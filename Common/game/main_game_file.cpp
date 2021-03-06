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
#include <cstdio>
#include "ac/audiocliptype.h"
#include "ac/dialogtopic.h"
#include "ac/gamesetupstruct.h"
#include "ac/spritecache.h"
#include "ac/view.h"
#include "ac/wordsdictionary.h"
#include "ac/dynobj/scriptaudioclip.h"
#include "core/asset.h"
#include "core/assetmanager.h"
#include "debug/out.h"
#include "game/main_game_file.h"
#include "gui/guimain.h"
#include "script/cc_error.h"
#include "util/alignedstream.h"
#include "util/data_ext.h"
#include "util/path.h"
#include "util/string_compat.h"
#include "util/string_utils.h"
#include "font/fonts.h"

namespace AGS
{
namespace Common
{

const String MainGameSource::DefaultFilename_v3 = "game28.dta";
const String MainGameSource::DefaultFilename_v2 = "ac2game.dta";
const String MainGameSource::Signature = "Adventure Creator Game File v2";

MainGameSource::MainGameSource()
    : DataVersion(kGameVersion_Undefined)
{
}

String GetMainGameFileErrorText(MainGameFileErrorType err)
{
    switch (err)
    {
    case kMGFErr_NoError:
        return "No error.";
    case kMGFErr_FileOpenFailed:
        return "Main game file not found or could not be opened.";
    case kMGFErr_SignatureFailed:
        return "Not an AGS main game file or unsupported format.";
    case kMGFErr_FormatVersionTooOld:
        return "Format version is too old; this engine can only run games made with AGS 3.4.1 or later";
    case kMGFErr_FormatVersionNotSupported:
        return "Format version not supported.";
    case kMGFErr_CapsNotSupported:
        return "The game requires extended capabilities which aren't supported by the engine.";
    case kMGFErr_InvalidNativeResolution:
        return "Unable to determine native game resolution.";
    case kMGFErr_TooManySprites:
        return "Too many sprites for this engine to handle.";
    case kMGFErr_TooManyCursors:
        return "Too many cursors for this engine to handle.";
    case kMGFErr_InvalidPropertySchema:
        return "Failed to deserialize custom properties schema.";
    case kMGFErr_InvalidPropertyValues:
        return "Errors encountered when reading custom properties.";
    case kMGFErr_NoGlobalScript:
        return "No global script in game.";
    case kMGFErr_CreateGlobalScriptFailed:
        return "Failed to load global script.";
    case kMGFErr_CreateDialogScriptFailed:
        return "Failed to load dialog script.";
    case kMGFErr_CreateScriptModuleFailed:
        return "Failed to load script module.";
    case kMGFErr_GameEntityFailed:
        return "Failed to load one or more game entities.";
    case kMGFErr_PluginDataFmtNotSupported:
        return "Format version of plugin data is not supported.";
    case kMGFErr_PluginDataSizeTooLarge:
        return "Plugin data size is too large.";
    case kMGFErr_ExtListFailed:
        return "There was error reading game data extensions.";
    case kMGFErr_ExtUnknown:
        return "Unknown extension.";
    }
    return "Unknown error.";
}

LoadedGameEntities::LoadedGameEntities(GameSetupStruct &game, DialogTopic *&dialogs, ViewStruct *&views)
    : Game(game)
    , Dialogs(dialogs)
    , Views(views)
    , SpriteCount(0)
{
}

LoadedGameEntities::~LoadedGameEntities() = default;

bool IsMainGameLibrary(const String &filename)
{
    // We must not only detect if the given file is a correct AGS data library,
    // we also have to assure that this library contains main game asset.
    // Library may contain some optional data (digital audio, speech etc), but
    // that is not what we want.
    AssetLibInfo lib;
    if (AssetManager::ReadDataFileTOC(filename, lib) != kAssetNoError)
        return false;
    for (size_t i = 0; i < lib.AssetInfos.size(); ++i)
    {
        if (lib.AssetInfos[i].FileName.CompareNoCase(MainGameSource::DefaultFilename_v3) == 0 ||
            lib.AssetInfos[i].FileName.CompareNoCase(MainGameSource::DefaultFilename_v2) == 0)
        {
            return true;
        }
    }
    return false;
}

// Scans given directory for game data libraries, returns first found or none.
// Tracks files with standard AGS package names:
// - *.ags is a standart cross-platform file pattern for AGS games,
// - ac2game.dat is a legacy file name for very old games,
// - *.exe is a MS Win executable; it is included to this case because
//   users often run AGS ports with Windows versions of games.
String FindGameData(const String &path, std::function<bool(const String&)> fn_testfile)
{
    al_ffblk ff;
    String test_file;
    String pattern = path;
    pattern.Append("/*");

    Debug::Printf("Searching for game data in: %s", path.GetCStr());
    if (al_findfirst(pattern.GetCStr(), &ff, FA_ALL & ~(FA_DIREC)) != 0)
        return "";
    do
    {
        test_file = ff.name;
        if (test_file.CompareRightNoCase(".ags") == 0 ||
            test_file.CompareNoCase("ac2game.dat") == 0 ||
            test_file.CompareRightNoCase(".exe") == 0)
        {
            test_file = Path::ConcatPaths(path, test_file);
            if (IsMainGameLibrary(test_file) && fn_testfile(path))
            {
                Debug::Printf("Found game data pak: %s", test_file.GetCStr());
                al_findclose(&ff);
                return test_file;
            }
        }
    } while (al_findnext(&ff) == 0);
    al_findclose(&ff);
    return "";
}

String FindGameData(const String &path)
{
    return FindGameData(path, [](const String&){ return true; });
}

// Begins reading main game file from a generic stream
static HGameFileError OpenMainGameFileBase(Stream *in, MainGameSource &src)
{
    // Check data signature
    String data_sig = String::FromStreamCount(in, MainGameSource::Signature.GetLength());
    if (data_sig.Compare(MainGameSource::Signature))
        return new MainGameFileError(kMGFErr_SignatureFailed);
    // Read data format version and requested engine version
    src.DataVersion = (GameDataVersion)in->ReadInt32();
    if (src.DataVersion >= kGameVersion_230)
        src.CompiledWith = StrUtil::ReadString(in);
    if (src.DataVersion < kGameVersion_341)
        return new MainGameFileError(kMGFErr_FormatVersionTooOld, String::FromFormat("Required format version: %d, supported %d - %d", src.DataVersion, kGameVersion_341, kGameVersion_Current));
    if (src.DataVersion > kGameVersion_Current)
        return new MainGameFileError(kMGFErr_FormatVersionNotSupported,
            String::FromFormat("Game was compiled with %s. Required format version: %d, supported %d - %d", src.CompiledWith.GetCStr(), src.DataVersion, kGameVersion_341, kGameVersion_Current));
    // Read required capabilities
    if (src.DataVersion >= kGameVersion_341)
    {
        size_t count = in->ReadInt32();
        for (size_t i = 0; i < count; ++i)
            src.Caps.insert(StrUtil::ReadString(in));
    }
    // Remember loaded game data version
    // NOTE: this global variable is embedded in the code too much to get
    // rid of it too easily; the easy way is to set it whenever the main
    // game file is opened.
    loaded_game_file_version = src.DataVersion;
    return HGameFileError::None();
}

HGameFileError OpenMainGameFile(const String &filename, MainGameSource &src)
{
    // Cleanup source struct
    src = MainGameSource();
    // Try to open given file
    Stream *in = File::OpenFileRead(filename);
    if (!in)
        return new MainGameFileError(kMGFErr_FileOpenFailed, String::FromFormat("Filename: %s.", filename.GetCStr()));
    src.Filename = filename;
    src.InputStream.reset(in);
    return OpenMainGameFileBase(in, src);
}

HGameFileError OpenMainGameFileFromDefaultAsset(MainGameSource &src)
{
    // Cleanup source struct
    src = MainGameSource();
    // Try to find and open main game file
    String filename = MainGameSource::DefaultFilename_v3;
    Stream *in = AssetMgr->OpenAsset(filename);
    if (!in)
    {
        filename = MainGameSource::DefaultFilename_v2;
        in = AssetMgr->OpenAsset(filename);
    }
    if (!in)
        return new MainGameFileError(kMGFErr_FileOpenFailed, String::FromFormat("Filename: %s.", filename.GetCStr()));
    src.Filename = filename;
    src.InputStream.reset(in);
    return OpenMainGameFileBase(in, src);
}

HGameFileError ReadDialogScript(PScript &dialog_script, Stream *in, GameDataVersion data_ver)
{
    if (data_ver > kGameVersion_310) // 3.1.1+ dialog script
    {
        dialog_script.reset(ccScript::CreateFromStream(in));
        if (dialog_script == nullptr)
            return new MainGameFileError(kMGFErr_CreateDialogScriptFailed, ccErrorString);
    }
    return HGameFileError::None();
}

HGameFileError ReadScriptModules(std::vector<PScript> &sc_mods, Stream *in, GameDataVersion data_ver)
{
    if (data_ver >= kGameVersion_270) // 2.7.0+ script modules
    {
        int count = in->ReadInt32();
        sc_mods.resize(count);
        for (int i = 0; i < count; ++i)
        {
            sc_mods[i].reset(ccScript::CreateFromStream(in));
            if (sc_mods[i] == nullptr)
                return new MainGameFileError(kMGFErr_CreateScriptModuleFailed, ccErrorString);
        }
    }
    else
    {
        sc_mods.resize(0);
    }
    return HGameFileError::None();
}

void ReadViewStruct272_Aligned(std::vector<ViewStruct272> &oldv, Stream *in, size_t count)
{
    AlignedStream align_s(in, Common::kAligned_Read);
    oldv.resize(count);
    for (size_t i = 0; i < count; ++i)
    {
        oldv[i].ReadFromFile(&align_s);
        align_s.Reset();
    }
}

void ReadViews(GameSetupStruct &game, ViewStruct *&views, Stream *in, GameDataVersion data_ver)
{
    int count = game.numviews;
    views = (ViewStruct*)calloc(sizeof(ViewStruct) * count, 1);
        for (int i = 0; i < game.numviews; ++i)
        {
            views[i].ReadFromFile(in);
        }
}

// CLNUP remove the old dialogs reading stuff when possible
void ReadDialogs(DialogTopic *&dialog, Stream *in, GameDataVersion data_ver, int dlg_count)
{
    // TODO: I suspect +5 was a hacky way to "supress" memory access mistakes;
    // double check and remove if proved unnecessary
    dialog = (DialogTopic*)malloc(sizeof(DialogTopic) * dlg_count + 5);
    for (int i = 0; i < dlg_count; ++i)
    {
        dialog[i].ReadFromFile(in);
    }

    if (data_ver > kGameVersion_310)
        return;

    for (int i = 0; i < dlg_count; ++i)
    {
        // NOTE: originally this was read into dialog[i].optionscripts
        in->Seek(dialog[i].codesize);// CLNUP old dialog

        // Encrypted text script
        int script_text_len = in->ReadInt32();
        in->Seek(script_text_len);// CLNUP was read and decrypted into old_dialog_src[i]
    }

    // Read the dialog lines
    //
    // TODO: investigate this: these strings were read much simplier in the editor, see code:
    /*
        char stringbuffer[1000];
        for (bb=0;bb<thisgame.numdlgmessage;bb++) {
            if ((filever >= 26) && (encrypted))
                read_string_decrypt(iii, stringbuffer);
            else
                fgetstring(stringbuffer, iii);
        }
    */
    int i = 0;
    char buffer[1000];

        // Encrypted text on > 2.60
        while (1)
        {
            size_t newlen = in->ReadInt32();
            if (static_cast<int32_t>(newlen) == 0xCAFEBEEF)  // GUI magic
            {
                in->Seek(-4);
                break;
            }

            newlen = Math::Min(newlen, sizeof(buffer) - 1);
        in->Seek(newlen);// CLNUP old_speech_lines
            decrypt_text(buffer, newlen);
            i++;
        }
}

HGameFileError ReadPlugins(std::vector<PluginInfo> &infos, Stream *in)
{
    int fmt_ver = in->ReadInt32();
    if (fmt_ver != 1)
        return new MainGameFileError(kMGFErr_PluginDataFmtNotSupported, String::FromFormat("Version: %d, supported: %d", fmt_ver, 1));

    int pl_count = in->ReadInt32();
    for (int i = 0; i < pl_count; ++i)
    {
        String name = String::FromStream(in);
        size_t datasize = in->ReadInt32();
        // just check for silly datasizes
        if (datasize > PLUGIN_SAVEBUFFERSIZE)
            return new MainGameFileError(kMGFErr_PluginDataSizeTooLarge, String::FromFormat("Required: %zu, max: %zu", datasize, (size_t)PLUGIN_SAVEBUFFERSIZE));

        PluginInfo info;
        info.Name = name;
        if (datasize > 0)
        {
            info.Data.reset(new char[datasize]);
            in->Read(info.Data.get(), datasize);
        }
        info.DataLen = datasize;
        infos.push_back(info);
    }
    return HGameFileError::None();
}

// Create the missing audioClips data structure for 3.1.x games.
// This is done by going through the data files and adding all music*.*
// and sound*.* files to it.
void BuildAudioClipArray(const std::vector<String> &assets, std::vector<ScriptAudioClip> &audioclips)
{
    char temp_name[30];
    int temp_number;
    char temp_extension[10];

    for (const String &asset : assets)
    {
        if (sscanf(asset.GetCStr(), "%5s%d.%3s", temp_name, &temp_number, temp_extension) != 3)
            continue;

        ScriptAudioClip clip;
        if (ags_stricmp(temp_extension, "mp3") == 0)
            clip.fileType = eAudioFileMP3;
        else if (ags_stricmp(temp_extension, "wav") == 0)
            clip.fileType = eAudioFileWAV;
        else if (ags_stricmp(temp_extension, "voc") == 0)
            clip.fileType = eAudioFileVOC;
        else if (ags_stricmp(temp_extension, "mid") == 0)
            clip.fileType = eAudioFileMIDI;
        else if ((ags_stricmp(temp_extension, "mod") == 0) || (ags_stricmp(temp_extension, "xm") == 0)
            || (ags_stricmp(temp_extension, "s3m") == 0) || (ags_stricmp(temp_extension, "it") == 0))
            clip.fileType = eAudioFileMOD;
        else if (ags_stricmp(temp_extension, "ogg") == 0)
            clip.fileType = eAudioFileOGG;
        else
            continue;

        if (ags_stricmp(temp_name, "music") == 0)
        {
            clip.scriptName.Format("aMusic%d", temp_number);
            clip.fileName.Format("music%d.%s", temp_number, temp_extension);
            clip.bundlingType = (ags_stricmp(temp_extension, "mid") == 0) ? AUCL_BUNDLE_EXE : AUCL_BUNDLE_VOX;
            clip.type = 2;
            clip.defaultRepeat = 1;
        }
        else if (ags_stricmp(temp_name, "sound") == 0)
        {
            clip.scriptName.Format("aSound%d", temp_number);
            clip.fileName.Format("sound%d.%s", temp_number, temp_extension);
            clip.bundlingType = AUCL_BUNDLE_EXE;
            clip.type = 3;
            clip.defaultRepeat = 0;
        }
        else
        {
            continue;
        }

        clip.defaultVolume = 100;
        clip.defaultPriority = 50;
        clip.id = audioclips.size();
        audioclips.push_back(clip);
    }
}

void ApplySpriteData(GameSetupStruct &game, const LoadedGameEntities &ents, GameDataVersion data_ver)
{
    if (ents.SpriteCount == 0)
        return;

    // Apply sprite flags read from original format (sequential array)
    spriteset.EnlargeTo(ents.SpriteCount - 1);
    for (size_t i = 0; i < ents.SpriteCount; ++i)
    {
        game.SpriteInfos[i].Flags = ents.SpriteFlags[i];
    }
}

void UpgradeFonts(GameSetupStruct &game, GameDataVersion data_ver)
{
    if (data_ver < kGameVersion_399)
    {
        for (size_t font = 0; font < game.numfonts; font++)
        {
            FontInfo &finfo = game.fonts[font];
            // Thickness that corresponds to 1 game pixel
            finfo.AutoOutlineThickness = 1;
            finfo.AutoOutlineStyle = FontInfo::kSquared;
        }
    }
}

// Convert audio data to the current version
void UpgradeAudio(GameSetupStruct &game, LoadedGameEntities &ents, GameDataVersion data_ver)
{
}

// Convert character data to the current version
void UpgradeCharacters(GameSetupStruct &game, GameDataVersion data_ver)
{
}

void UpgradeMouseCursors(GameSetupStruct &game, GameDataVersion data_ver)
{
}

// Adjusts score clip id, depending on game data version
void RemapLegacySoundNums(GameSetupStruct &game, ViewStruct *&views, GameDataVersion data_ver)
{
}

// Assigns default global message at given index
void SetDefaultGlmsg(GameSetupStruct &game, int msgnum, const char *val)
{
    // TODO: find out why the index should be lowered by 500
    // (or rather if we may pass correct index right away)
    msgnum -= 500;
    if (game.messages[msgnum] == nullptr)
        game.messages[msgnum] = ags_strdup(val);
}

// CLNUP not to remove yet, global messages may be deprecated but these are used by the engine
// Sets up default global messages (these are used mainly in older games)
void SetDefaultGlobalMessages(GameSetupStruct &game)
{
    SetDefaultGlmsg(game, 983, "Sorry, not now.");
    SetDefaultGlmsg(game, 984, "Restore");
    SetDefaultGlmsg(game, 985, "Cancel");
    SetDefaultGlmsg(game, 986, "Select a game to restore:");
    SetDefaultGlmsg(game, 987, "Save");
    SetDefaultGlmsg(game, 988, "Type a name to save as:");
    SetDefaultGlmsg(game, 989, "Replace");
    SetDefaultGlmsg(game, 990, "The save directory is full. You must replace an existing game:");
    SetDefaultGlmsg(game, 991, "Replace:");
    SetDefaultGlmsg(game, 992, "With:");
    SetDefaultGlmsg(game, 993, "Quit");
    SetDefaultGlmsg(game, 994, "Play");
    SetDefaultGlmsg(game, 995, "Are you sure you want to quit?");
    SetDefaultGlmsg(game, 996, "You are carrying nothing.");
}

void FixupSaveDirectory(GameSetupStruct &game)
{
    // If the save game folder was not specified by game author, create one of
    // the game name, game GUID, or uniqueid, as a last resort
    if (!game.saveGameFolderName[0])
    {
        if (game.gamename[0])
            snprintf(game.saveGameFolderName, MAX_SG_FOLDER_LEN, "%s", game.gamename);
        else if (game.guid[0])
            snprintf(game.saveGameFolderName, MAX_SG_FOLDER_LEN, "%s", game.guid);
        else
            snprintf(game.saveGameFolderName, MAX_SG_FOLDER_LEN, "AGS-Game-%d", game.uniqueid);
    }
    // Lastly, fixup folder name by removing any illegal characters
    String s = Path::FixupSharedFilename(game.saveGameFolderName);
    snprintf(game.saveGameFolderName, MAX_SG_FOLDER_LEN, "%s", s.GetCStr());
}

HGameFileError ReadSpriteFlags(LoadedGameEntities &ents, Stream *in, GameDataVersion data_ver)
{
    size_t sprcount;
    if (data_ver < kGameVersion_256)
        sprcount = LEGACY_MAX_SPRITES_V25;
    else
        sprcount = in->ReadInt32();
    if (sprcount > (size_t)SpriteCache::MAX_SPRITE_INDEX + 1)
        return new MainGameFileError(kMGFErr_TooManySprites, String::FromFormat("Count: %zu, max: %zu", sprcount, (size_t)SpriteCache::MAX_SPRITE_INDEX + 1));

    ents.SpriteCount = sprcount;
    ents.SpriteFlags.reset(new char[sprcount]);
    in->Read(ents.SpriteFlags.get(), sprcount);
    return HGameFileError::None();
}

static HGameFileError ReadExtBlock(LoadedGameEntities &ents, Stream *in, const String &ext_id, soff_t block_len, GameDataVersion data_ver)
{
    // Add extensions here checking ext_id, which is an up to 16-chars name, for example:
    // if (ext_id.CompareNoCase("GUI_NEWPROPS") == 0)
    // {
    //     // read new gui properties
    // }
    // Early development version of "ags4"
    if (ext_id.CompareNoCase("ext_ags399") == 0)
    {
        // adjustable font outlines
        for (size_t i = 0; i < (size_t)ents.Game.numfonts; ++i)
        {
            ents.Game.fonts[i].AutoOutlineThickness = in->ReadInt32();
            ents.Game.fonts[i].AutoOutlineStyle =
                static_cast<enum FontInfo::AutoOutlineStyle>(in->ReadInt32());
        }

        // new character properties
        for (size_t i = 0; i < (size_t)ents.Game.numcharacters; ++i)
        {
            ents.CharEx[i].BlendMode = (BlendMode)in->ReadInt32();
            // Reserved for colour options
            in->Seek(sizeof(int32_t) * 3); // flags + tint rgbs + light level
            // Reserved for transform options (see brief list in savegame format)
            in->Seek(sizeof(int32_t) * 11);
        }

        // new gui properties
        for (size_t i = 0; i < guis.size(); ++i)
        {
            guis[i].BlendMode = (BlendMode)in->ReadInt32();
            // Reserved for colour options
            in->Seek(sizeof(int32_t) * 3); // flags + tint rgbs + light level
            // Reserved for transform options (see list in savegame format)
            in->Seek(sizeof(int32_t) * 11);
        }

        return HGameFileError::None();
    }
    return new MainGameFileError(kMGFErr_ExtUnknown, String::FromFormat("Type: %s", ext_id.GetCStr()));
}

HGameFileError ReadGameData(LoadedGameEntities &ents, Stream *in, GameDataVersion data_ver)
{
    GameSetupStruct &game = ents.Game;

    //-------------------------------------------------------------------------
    // The classic data section.
    //-------------------------------------------------------------------------
    {
        AlignedStream align_s(in, Common::kAligned_Read);
        game.GameSetupStructBase::ReadFromFile(&align_s);
    }

    Debug::Printf(kDbgMsg_Info, "Game title: '%s'", game.gamename);

    if (game.GetGameRes().IsNull())
        return new MainGameFileError(kMGFErr_InvalidNativeResolution);

    game.read_savegame_info(in, data_ver);
    game.read_font_infos(in, data_ver);
    HGameFileError err = ReadSpriteFlags(ents, in, data_ver);
    if (!err)
        return err;
    game.ReadInvInfo_Aligned(in);
    err = game.read_cursors(in, data_ver);
    if (!err)
        return err;
    game.read_interaction_scripts(in, data_ver);
    game.read_words_dictionary(in);

    if (!game.load_compiled_script)
        return new MainGameFileError(kMGFErr_NoGlobalScript);
    ents.GlobalScript.reset(ccScript::CreateFromStream(in));
    if (!ents.GlobalScript)
        return new MainGameFileError(kMGFErr_CreateGlobalScriptFailed, ccErrorString);
    err = ReadDialogScript(ents.DialogScript, in, data_ver);
    if (!err)
        return err;
    err = ReadScriptModules(ents.ScriptModules, in, data_ver);
    if (!err)
        return err;

    ReadViews(game, ents.Views, in, data_ver);

    game.read_characters(in, data_ver);
    game.read_lipsync(in, data_ver);
    game.read_messages(in, data_ver);

    ReadDialogs(ents.Dialogs, in, data_ver, game.numdialog);
    HError err2 = GUI::ReadGUI(guis, in);
    if (!err2)
        return new MainGameFileError(kMGFErr_GameEntityFailed, err2);
    game.numgui = guis.size();

    if (data_ver >= kGameVersion_260)
    {
        err = ReadPlugins(ents.PluginInfos, in);
        if (!err)
            return err;
    }

    err = game.read_customprops(in, data_ver);
    if (!err)
        return err;
    err = game.read_audio(in, data_ver);
    if (!err)
        return err;
    game.read_room_names(in, data_ver);

    if (data_ver <= kGameVersion_350)
        return HGameFileError::None();

    ents.CharEx.resize(game.numcharacters);
    //-------------------------------------------------------------------------
    // All the extended data, for AGS > 3.5.0.
    //-------------------------------------------------------------------------
    // This reader will process all blocks inside ReadExtBlock() function,
    // and read compatible data into the given LoadedGameEntities object.
    auto reader = [&ents, data_ver](Stream *in, int /*block_id*/, const String &ext_id,
        soff_t block_len, bool &read_next)
    { return (HError)ReadExtBlock(ents, in, ext_id, block_len, data_ver); };

    HError ext_err = ReadExtData(reader, kDataExt_NumID8 | kDataExt_File64, in);
    return ext_err ? HGameFileError::None() : new MainGameFileError(kMGFErr_ExtListFailed, ext_err);
}

HGameFileError UpdateGameData(LoadedGameEntities &ents, GameDataVersion data_ver)
{
    GameSetupStruct &game = ents.Game;
    ApplySpriteData(game, ents, data_ver);
    UpgradeFonts(game, data_ver);
    UpgradeAudio(game, ents, data_ver);
    UpgradeCharacters(game, data_ver);
    UpgradeMouseCursors(game, data_ver);
    SetDefaultGlobalMessages(game);
    // Old dialog options API for pre-3.4.0.2 games
    if (data_ver < kGameVersion_340_2)
    {
        game.options[OPT_DIALOGOPTIONSAPI] = -1;
    }
    // Relative asset resolution in pre-3.5.0.8 (always enabled)
    if (data_ver < kGameVersion_350)
    {
        game.options[OPT_RELATIVEASSETRES] = 1;
    }
    FixupSaveDirectory(game);
    return HGameFileError::None();
}

void PreReadGameData(GameSetupStruct &game, Stream *in, GameDataVersion data_ver)
{
    {
        AlignedStream align_s(in, Common::kAligned_Read);
        game.ReadFromFile(&align_s);
    }
    // Discard game messages we do not need here
    delete[] game.load_messages;
    game.load_messages = nullptr;
    game.read_savegame_info(in, data_ver);
}

} // namespace Common
} // namespace AGS

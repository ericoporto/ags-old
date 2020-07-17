#include <core/assetmanager.h>
#include <ac/gamesetupstruct.h>
#include <ac/spritecache.h>
#include "game/room_file.h"
#include "game/roomstruct.h"
#include "gfx/bitmap.h"
#include "roomlib.h"
#include "cstrech.h"
using AGS::Common::Stream;

using AGS::Common::RoomStruct;

void save_room_file(const char *path);

const char *sprsetname = "acsprset.spr";
const char *sprindexname = "sprindex.dat";
const char *old_editor_data_file = "editor.dat";
const char *new_editor_data_file = "game.agf";
const char *old_editor_main_game_file = "ac2game.dta";
const char *ROOM_TEMPLATE_ID_FILE = "rtemplate.dat";
const int ROOM_TEMPLATE_ID_FILE_SIGNATURE = 0x74673812;

RoomStruct thisroom;
typedef AGS::Common::Bitmap AGSBitmap;
typedef AGS::Common::PBitmap PBitmap;
const int MAX_PATH = 260;
color*palette = NULL;
GameSetupStruct thisgame;
SpriteCache spriteset(thisgame.SpriteInfos);
bool enable_greyed_out_masks = true;
typedef int HDC;

inline void Cstretch_blit(Common::Bitmap *src, Common::Bitmap *dst, int sx, int sy, int sw, int sh, int dx, int dy, int dw, int dh)
{
    Cstretch_blit(src->GetAllegroBitmap(), dst->GetAllegroBitmap(), sx, sy, sw, sh, dx, dy, dw, dh);
}
inline void Cstretch_sprite(Common::Bitmap *dst, Common::Bitmap *src, int x, int y, int w, int h)
{
    Cstretch_sprite(dst->GetAllegroBitmap(), src->GetAllegroBitmap(), x, y, w, h);
}

void quit(const char * message)
{
   // ThrowManagedException((const char*)message);
}

struct NativeRoomTools
{
    bool roomModified = false;
    int loaded_room_number = -1;
    std::unique_ptr<AGSBitmap> drawBuffer;
    std::unique_ptr<AGSBitmap> undoBuffer;
    std::unique_ptr<AGSBitmap> roomBkgBuffer;
};
std::unique_ptr<NativeRoomTools> RoomTools;

int extract_room_template_files(const char *templateFileName, int newRoomNumber)
{
    if (Common::AssetManager::SetDataFile(templateFileName) != Common::kAssetNoError)
    {
        return 0;
    }
    if (Common::AssetManager::GetAssetOffset(ROOM_TEMPLATE_ID_FILE) < 1)
    {
        Common::AssetManager::SetDataFile(NULL);
        return 0;
    }

    int numFile = Common::AssetManager::GetAssetCount();

    for (int a = 0; a < numFile; a++) {
        const char *thisFile = Common::AssetManager::GetAssetFileByIndex(a);
        if (thisFile == NULL) {
            Common::AssetManager::SetDataFile(NULL);
            return 0;
        }

        // don't extract the template metadata file
        if (stricmp(thisFile, ROOM_TEMPLATE_ID_FILE) == 0)
            continue;

        Stream *readin = Common::AssetManager::OpenAsset ((char*)thisFile);
        char outputName[MAX_PATH];
        const char *extension = strchr(thisFile, '.');
        sprintf(outputName, "room%d%s", newRoomNumber, extension);
        Stream *wrout = Common::File::CreateFile(outputName);
        if ((readin == NULL) || (wrout == NULL))
        {
            delete wrout;
            delete readin;
            Common::AssetManager::SetDataFile(NULL);
            return 0;
        }
        long size = Common::AssetManager::GetAssetSize((char*)thisFile);
        char *membuff = (char*)malloc (size);
        readin->Read(membuff, size);
        wrout->Write (membuff, size );
        delete readin;
        delete wrout;
        free (membuff);
    }

    Common::AssetManager::SetDataFile(NULL);
    return 1;
}

void extract_icon_from_template(char *iconName, char **iconDataBuffer, long *bufferSize)
{
    // make sure we get the icon from the file
    Common::AssetManager::SetSearchPriority(Common::kAssetPriorityLib);
    long sizey = Common::AssetManager::GetAssetSize(iconName);
    Stream* inpu = Common::AssetManager::OpenAsset (iconName);
    if ((inpu != NULL) && (sizey > 0))
    {
        char *iconbuffer = (char*)malloc(sizey);
        inpu->Read (iconbuffer, sizey);
        delete inpu;
        *iconDataBuffer = iconbuffer;
        *bufferSize = sizey;
    }
    else
    {
        *iconDataBuffer = NULL;
        *bufferSize = 0;
    }
    // restore to normal setting after NewGameChooser changes it
    Common::AssetManager::SetSearchPriority(Common::kAssetPriorityDir);
}



int load_template_file(const char *fileName, char **iconDataBuffer, long *iconDataSize, bool isRoomTemplate)
{
    if (Common::AssetManager::SetDataFile(fileName) == Common::kAssetNoError)
    {
        if (isRoomTemplate)
        {
            if (Common::AssetManager::GetAssetOffset((char*)ROOM_TEMPLATE_ID_FILE) > 0)
            {
                Stream *inpu = Common::AssetManager::OpenAsset((char*)ROOM_TEMPLATE_ID_FILE);
                if (inpu->ReadInt32() != ROOM_TEMPLATE_ID_FILE_SIGNATURE)
                {
                    delete inpu;
                    Common::AssetManager::SetDataFile(NULL);
                    return 0;
                }
                int roomNumber = inpu->ReadInt32();
                delete inpu;
                char iconName[MAX_PATH];
                sprintf(iconName, "room%d.ico", roomNumber);
                if (Common::AssetManager::GetAssetOffset(iconName) > 0)
                {
                    extract_icon_from_template(iconName, iconDataBuffer, iconDataSize);
                }
                Common::AssetManager::SetDataFile(NULL);
                return 1;
            }
            Common::AssetManager::SetDataFile(NULL);
            return 0;
        }
        else if ((Common::AssetManager::GetAssetOffset((char*)old_editor_data_file) > 0) || (Common::AssetManager::GetAssetOffset((char*)new_editor_data_file) > 0))
        {
            Common::String oriname = Common::AssetManager::GetLibraryBaseFile();
            if ((strstr(oriname, ".exe") != NULL) ||
                (strstr(oriname, ".dat") != NULL) ||
                (strstr(oriname, ".ags") != NULL))
            {
                // wasn't originally meant as a template
                Common::AssetManager::SetDataFile(NULL);
                return 0;
            }

            Stream *inpu = Common::AssetManager::OpenAsset((char*)old_editor_main_game_file);
            if (inpu != NULL)
            {
                inpu->Seek(30);
                int gameVersion = inpu->ReadInt32();
                delete inpu;
                if (gameVersion != 32)
                {
                    // older than 2.72 template
                    Common::AssetManager::SetDataFile(NULL);
                    return 0;
                }
            }

            int useIcon = 0;
            const char *iconName = "template.ico";
            if (Common::AssetManager::GetAssetOffset (iconName) < 1)
                iconName = "user.ico";
            // the file is a CLIB file, so let's extract the icon to display
            if (Common::AssetManager::GetAssetOffset (iconName) > 0)
            {
                extract_icon_from_template((char*) iconName, iconDataBuffer, iconDataSize);
            }
            Common::AssetManager::SetDataFile(NULL);
            return 1;
        }
    }
    return 0;
}

enum RoomAreaMask
{
    None,
    Hotspots,
    WalkBehinds,
    WalkableAreas,
    Regions
};

// TODO: mask's bitmap, as well as coordinate factor, should be a property of the room or some room's mask class
// TODO: create weak_ptr here?
AGSBitmap *get_bitmap_for_mask(RoomStruct *roomptr, RoomAreaMask maskType)
{
    switch (maskType)
    {
        case RoomAreaMask::Hotspots: return roomptr->HotspotMask.get();
        case RoomAreaMask::Regions: return roomptr->RegionMask.get();
        case RoomAreaMask::WalkableAreas: return roomptr->WalkAreaMask.get();
        case RoomAreaMask::WalkBehinds: return roomptr->WalkBehindMask.get();
    }
    return NULL;
}

float get_scale_for_mask(RoomStruct *roomptr, RoomAreaMask maskType)
{
    switch (maskType)
    {
        case RoomAreaMask::Hotspots: return 1.f / roomptr->MaskResolution;
        case RoomAreaMask::Regions: return 1.f / roomptr->MaskResolution;
        case RoomAreaMask::WalkableAreas: return 1.f / roomptr->MaskResolution;
        case RoomAreaMask::WalkBehinds: return 1.f; // walk-behinds always 1:1 with room size
    }
    return 0.f;
}


void copy_walkable_to_regions (void *roomptr) {
    RoomStruct *theRoom = (RoomStruct*)roomptr;
    theRoom->RegionMask->Blit(theRoom->WalkAreaMask.get(), 0, 0, 0, 0, theRoom->RegionMask->GetWidth(), theRoom->RegionMask->GetHeight());
}

int get_mask_pixel(void *roomptr, int maskType, int x, int y)
{
    Common::Bitmap *mask = get_bitmap_for_mask((RoomStruct*)roomptr, (RoomAreaMask)maskType);
    float scale = get_scale_for_mask((RoomStruct*)roomptr, (RoomAreaMask)maskType);
    return mask->GetPixel(x * scale, y * scale);
}

void draw_line_onto_mask(void *roomptr, int maskType, int x1, int y1, int x2, int y2, int color)
{
    Common::Bitmap *mask = get_bitmap_for_mask((RoomStruct*)roomptr, (RoomAreaMask)maskType);
    float scale = get_scale_for_mask((RoomStruct*)roomptr, (RoomAreaMask)maskType);
    mask->DrawLine(Line(x1 * scale, y1 * scale, x2 * scale, y2 * scale), color);
}

void draw_filled_rect_onto_mask(void *roomptr, int maskType, int x1, int y1, int x2, int y2, int color)
{
    Common::Bitmap *mask = get_bitmap_for_mask((RoomStruct*)roomptr, (RoomAreaMask)maskType);
    float scale = get_scale_for_mask((RoomStruct*)roomptr, (RoomAreaMask)maskType);
    mask->FillRect(Rect(x1 * scale, y1 * scale, x2 * scale, y2 * scale), color);
}

void draw_fill_onto_mask(void *roomptr, int maskType, int x1, int y1, int color)
{
    Common::Bitmap *mask = get_bitmap_for_mask((RoomStruct*)roomptr, (RoomAreaMask)maskType);
    float scale = get_scale_for_mask((RoomStruct*)roomptr, (RoomAreaMask)maskType);
    mask->FloodFill(x1 * scale, y1 * scale, color);
}

void create_undo_buffer(void *roomptr, int maskType)
{
    Common::Bitmap *mask = get_bitmap_for_mask((RoomStruct*)roomptr, (RoomAreaMask)maskType);
    auto &undoBuffer = RoomTools->undoBuffer;
    if (undoBuffer != NULL)
    {
        if ((undoBuffer->GetWidth() != mask->GetWidth()) || (undoBuffer->GetHeight() != mask->GetHeight()))
        {
            undoBuffer.reset();
        }
    }
    if (undoBuffer == NULL)
    {
        undoBuffer.reset(Common::BitmapHelper::CreateBitmap(mask->GetWidth(), mask->GetHeight(), mask->GetColorDepth()));
    }
    undoBuffer->Blit(mask, 0, 0, 0, 0, mask->GetWidth(), mask->GetHeight());
}

bool does_undo_buffer_exist()
{
    return (RoomTools->undoBuffer != NULL);
}

void clear_undo_buffer()
{
    if (does_undo_buffer_exist())
    {
        RoomTools->undoBuffer.reset();
    }
}

void restore_from_undo_buffer(void *roomptr, int maskType)
{
    if (does_undo_buffer_exist())
    {
        Common::Bitmap *mask = get_bitmap_for_mask((RoomStruct*)roomptr, (RoomAreaMask)maskType);
        mask->Blit(RoomTools->undoBuffer.get(), 0, 0, 0, 0, mask->GetWidth(), mask->GetHeight());
    }
}

void setup_greyed_out_palette(int selCol)
{
    color thisColourOnlyPal[256];

    // The code below makes it so that all the hotspot colours
    // except the selected one are greyed out. It doesn't work
    // in 256-colour games.

    // Blank out the temporary palette, and set a shade of grey
    // for all the hotspot colours
    for (int aa = 0; aa < 256; aa++) {
        int lumin = 0;
        if ((aa < MAX_ROOM_HOTSPOTS) && (aa > 0))
            lumin = ((MAX_ROOM_HOTSPOTS - aa) % 30) * 2;
        thisColourOnlyPal[aa].r = lumin;
        thisColourOnlyPal[aa].g = lumin;
        thisColourOnlyPal[aa].b = lumin;
    }
    // Highlight the currently selected area colour
    if (selCol > 0) {
        // if a bright colour, use it
        if ((selCol < 15) && (selCol != 7) && (selCol != 8))
            thisColourOnlyPal[selCol] = palette[selCol];
        else {
            // else, draw in red
            thisColourOnlyPal[selCol].r = 60;
            thisColourOnlyPal[selCol].g = 0;
            thisColourOnlyPal[selCol].b = 0;
        }
    }
    set_palette(thisColourOnlyPal);
}

Common::Bitmap *recycle_bitmap(Common::Bitmap* check, int colDepth, int w, int h)
{
    if ((check != NULL) && (check->GetWidth() == w) && (check->GetHeight() == h) &&
        (check->GetColorDepth() == colDepth))
    {
        return check;
    }
    delete check;

    return Common::BitmapHelper::CreateBitmap(w, h, colDepth);
}

Common::Bitmap *stretchedSprite = NULL, *srcAtRightColDep = NULL;

void draw_area_mask(RoomStruct *roomptr, Common::Bitmap *ds, RoomAreaMask maskType, int selectedArea, int transparency)
{
    Common::Bitmap *source = get_bitmap_for_mask(roomptr, maskType);

    if (source == NULL) return;

    int dest_width = ds->GetWidth();
    int dest_height = ds->GetHeight();
    int dest_depth =  ds->GetColorDepth();

    if (source->GetColorDepth() != dest_depth)
    {
        Common::Bitmap *sourceSprite = source;
        if ((source->GetWidth() != dest_width) || (source->GetHeight() != dest_height))
        {
            stretchedSprite = recycle_bitmap(stretchedSprite, source->GetColorDepth(), dest_width, dest_height);
            stretchedSprite->StretchBlt(source, RectWH(0, 0, source->GetWidth(), source->GetHeight()),
                                        RectWH(0, 0, stretchedSprite->GetWidth(), stretchedSprite->GetHeight()));
            sourceSprite = stretchedSprite;
        }

        if (enable_greyed_out_masks)
        {
            setup_greyed_out_palette(selectedArea);
        }

        if (transparency > 0)
        {
            srcAtRightColDep = recycle_bitmap(srcAtRightColDep, dest_depth, dest_width, dest_height);

            int oldColorConv = get_color_conversion();
            set_color_conversion(oldColorConv | COLORCONV_KEEP_TRANS);

            srcAtRightColDep->Blit(sourceSprite, 0, 0, 0, 0, sourceSprite->GetWidth(), sourceSprite->GetHeight());
            set_trans_blender(0, 0, 0, (100 - transparency) + 155);
            ds->TransBlendBlt(srcAtRightColDep, 0, 0);
            set_color_conversion(oldColorConv);
        }
        else
        {
            ds->Blit(sourceSprite, 0, 0, Common::kBitmap_Transparency);
        }

        set_palette(palette);
    }
    else
    {
        Cstretch_sprite(ds, source, 0, 0, dest_width, dest_height);
    }
}

void drawBlockScaledAt (int hdc, Common::Bitmap *todraw ,int x, int y, float scaleFactor) {
    //if (todraw->GetColorDepth () == 8)
//        set_palette_to_hdc ((HDC)hdc, palette);

    // FIXME later
  //  stretch_blit_to_hdc (todraw->GetAllegroBitmap(), (HDC)hdc, 0,0,todraw->GetWidth(),todraw->GetHeight(),
    //                     x,y,todraw->GetWidth() * scaleFactor, todraw->GetHeight() * scaleFactor);
}

void draw_room_background(void *roomvoidptr, int hdc, int x, int y, int bgnum, float scaleFactor, int maskType, int selectedArea, int maskTransparency)
{
    RoomStruct *roomptr = (RoomStruct*)roomvoidptr;

    if (bgnum < 0 || (size_t)bgnum >= roomptr->BgFrameCount)
        return;

    PBitmap srcBlock = roomptr->BgFrames[bgnum].Graphic;
    if (srcBlock == NULL)
        return;

    auto &drawBuffer = RoomTools->drawBuffer;
    auto &roomBkgBuffer = RoomTools->roomBkgBuffer;
    if (drawBuffer != NULL)
    {
        if (!roomBkgBuffer || roomBkgBuffer->GetSize() != srcBlock->GetSize() || roomBkgBuffer->GetColorDepth() != srcBlock->GetColorDepth())
            roomBkgBuffer.reset(new AGSBitmap(srcBlock->GetWidth(), srcBlock->GetHeight(), drawBuffer->GetColorDepth()));
        if (srcBlock->GetColorDepth() == 8)
        {
            select_palette(roomptr->BgFrames[bgnum].Palette);
        }

        roomBkgBuffer->Blit(srcBlock.get(), 0, 0, 0, 0, srcBlock->GetWidth(), srcBlock->GetHeight());

        if (srcBlock->GetColorDepth() == 8)
        {
            unselect_palette();
        }

        draw_area_mask(roomptr, roomBkgBuffer.get(), (RoomAreaMask)maskType, selectedArea, maskTransparency);

        int srcX = 0, srcY = 0;
        int srcWidth = srcBlock->GetWidth();
        int srcHeight = srcBlock->GetHeight();

        if (x < 0)
        {
            srcX = (int)(-x / scaleFactor);
            x += (int)(srcX * scaleFactor);
            srcWidth = drawBuffer->GetWidth() / scaleFactor;
            if (srcX + srcWidth > roomBkgBuffer->GetWidth())
            {
                srcWidth = roomBkgBuffer->GetWidth() - srcX;
            }
        }
        if (y < 0)
        {
            srcY = (int)(-y / scaleFactor);
            y += (int)(srcY * scaleFactor);
            srcHeight = drawBuffer->GetHeight() / scaleFactor;
            if (srcY + srcHeight > roomBkgBuffer->GetHeight())
            {
                srcHeight = roomBkgBuffer->GetHeight() - srcY;
            }
        }

        Cstretch_blit(roomBkgBuffer.get(), drawBuffer.get(), srcX, srcY, srcWidth, srcHeight,
                      x, y, (int)(srcWidth * scaleFactor), (int)(srcHeight * scaleFactor));
    }
    else {
        drawBlockScaledAt(hdc, srcBlock.get(), x, y, scaleFactor);
    }

}

bool initialize_native()
{
    Common::AssetManager::CreateInstance();

    set_uformat(U_ASCII);  // required to stop ALFONT screwing up text
    install_allegro(SYSTEM_NONE, &errno, atexit);

    RoomTools.reset(new NativeRoomTools());
    return true;
}

void shutdown_native()
{
    RoomTools.reset();
    // We must dispose all native bitmaps before shutting down the library
    thisroom.Free();
    allegro_exit();
    Common::AssetManager::DestroyInstance();
}

void validate_mask(Common::Bitmap *toValidate, const char *name, int maxColour) {
    if ((toValidate == NULL) || (toValidate->GetColorDepth() != 8) ||
        (!toValidate->IsMemoryBitmap())) {
        quit("Invalid mask passed to validate_mask");
        return;
    }

    bool errFound = false;
    int xx, yy;
    for (yy = 0; yy < toValidate->GetHeight(); yy++) {
        for (xx = 0; xx < toValidate->GetWidth(); xx++) {
            if (toValidate->GetScanLine(yy)[xx] >= maxColour) {
                errFound = true;
                toValidate->GetScanLineForWriting(yy)[xx] = 0;
            }
        }
    }

    if (errFound) {
        char errorBuf[1000];
        sprintf(errorBuf, "Invalid colours were found in the %s mask. They have now been removed."
                          "\n\nWhen drawing a mask in an external paint package, you need to make "
                          "sure that the image is set as 256-colour (Indexed Palette), and that "
                          "you use the first 16 colours in the palette for drawing your areas. Palette "
                          "entry 0 corresponds to No Area, palette index 1 corresponds to area 1, and "
                          "so forth.", name);
        //MessageBox(NULL, errorBuf, "Mask Error", MB_OK);
        RoomTools->roomModified = true;
    }
}

const char* load_room_file(const char*rtlo) {
    load_room(rtlo, &thisroom, thisgame.IsLegacyHiRes(), thisgame.SpriteInfos);

    // Allocate enough memory to add extra variables
    thisroom.LocalVariables.resize(MAX_GLOBAL_VARIABLES);

    for (size_t i = 0; i < thisroom.ObjectCount; ++i) {
        // change invalid objects to blue cup
        // TODO: should this be done in the common native lib?
        if (spriteset[thisroom.Objects[i].Sprite] == NULL)
            thisroom.Objects[i].Sprite = 0;
    }

    set_palette_range(palette, 0, 255, 0);

    if ((thisroom.BgFrames[0].Graphic->GetColorDepth () > 8) &&
        (thisgame.color_depth == 1))
        printf("WARNING: This room is hi-color, but your game is currently 256-colour. You will not be able to use this room in this game. Also, the room background will not look right in the editor.");

    RoomTools->roomModified = false;

    validate_mask(thisroom.HotspotMask.get(), "hotspot", MAX_ROOM_HOTSPOTS);
    validate_mask(thisroom.WalkBehindMask.get(), "walk-behind", MAX_WALK_AREAS + 1);
    validate_mask(thisroom.WalkAreaMask.get(), "walkable area", MAX_WALK_AREAS + 1);
    validate_mask(thisroom.RegionMask.get(), "regions", MAX_ROOM_REGIONS);
    return NULL;
}


void calculate_walkable_areas () {
    int ww, thispix;

    for (ww = 0; ww <= MAX_WALK_AREAS; ww++) {
        thisroom.WalkAreas[ww].Top = thisroom.Height;
        thisroom.WalkAreas[ww].Bottom = 0;
    }
    for (ww = 0; ww < thisroom.WalkAreaMask->GetWidth(); ww++) {
        for (int qq = 0; qq < thisroom.WalkAreaMask->GetHeight(); qq++) {
            thispix = thisroom.WalkAreaMask->GetPixel (ww, qq);
            if (thispix > MAX_WALK_AREAS)
                continue;
            if (thisroom.WalkAreas[thispix].Top > qq)
                thisroom.WalkAreas[thispix].Top = qq;
            if (thisroom.WalkAreas[thispix].Bottom < qq)
                thisroom.WalkAreas[thispix].Bottom= qq;
        }
    }

}

/*
AGS::Types::Room^ load_crm_file(UnloadedRoom ^roomToLoad)
{
    AGSString roomFileName = ConvertFileNameToNativeString(roomToLoad->FileName);

    const char *errorMsg = load_room_file(roomFileName);
    if (errorMsg != NULL)
    {
        throw gcnew AGSEditorException(gcnew String(errorMsg));
    }

    RoomTools->loaded_room_number = roomToLoad->Number;

    Room ^room = gcnew Room(roomToLoad->Number);
    room->Description = roomToLoad->Description;
    room->Script = roomToLoad->Script;
    room->BottomEdgeY = thisroom.Edges.Bottom;
    room->LeftEdgeX = thisroom.Edges.Left;
    room->MusicVolumeAdjustment = (AGS::Types::RoomVolumeAdjustment)thisroom.Options.StartupMusic;
    room->PlayerCharacterView = thisroom.Options.PlayerView;
    room->PlayMusicOnRoomLoad = thisroom.Options.StartupMusic;
    room->RightEdgeX = thisroom.Edges.Right;
    room->SaveLoadEnabled = (thisroom.Options.SaveLoadDisabled == 0);
    room->ShowPlayerCharacter = (thisroom.Options.PlayerCharOff == 0);
    room->TopEdgeY = thisroom.Edges.Top;
    room->Width = thisroom.Width;
    room->Height = thisroom.Height;
    room->ColorDepth = thisroom.BgFrames[0].Graphic->GetColorDepth();
    room->BackgroundAnimationDelay = thisroom.BgAnimSpeed;
    room->BackgroundCount = thisroom.BgFrameCount;
    room->Resolution = (AGS::Types::RoomResolution)thisroom.GetResolutionType();
    room->MaskResolution = thisroom.MaskResolution;

    for (size_t i = 0; i < thisroom.LocalVariables.size(); ++i)
    {
        OldInteractionVariable ^intVar;
        intVar = gcnew OldInteractionVariable(gcnew String(thisroom.LocalVariables[i].Name), thisroom.LocalVariables[i].Value);
        room->OldInteractionVariables->Add(intVar);
    }

    for (size_t i = 0; i < thisroom.MessageCount; ++i)
    {
        RoomMessage ^newMessage = gcnew RoomMessage(i);
        newMessage->Text = gcnew String(thisroom.Messages[i]);
        newMessage->ShowAsSpeech = (thisroom.MessageInfos[i].DisplayAs > 0);
        newMessage->CharacterID = (thisroom.MessageInfos[i].DisplayAs - 1);
        newMessage->DisplayNextMessageAfter = ((thisroom.MessageInfos[i].Flags & MSG_DISPLAYNEXT) != 0);
        newMessage->AutoRemoveAfterTime = ((thisroom.MessageInfos[i].Flags & MSG_TIMELIMIT) != 0);
        room->Messages->Add(newMessage);
    }

    for (size_t i = 0; i < thisroom.ObjectCount; ++i)
    {
        RoomObject ^obj = gcnew RoomObject(room);
        obj->ID = i;
        obj->Image = thisroom.Objects[i].Sprite;
        obj->StartX = thisroom.Objects[i].X;
        obj->StartY = thisroom.Objects[i].Y;
        obj->Visible = (thisroom.Objects[i].IsOn != 0);
        obj->Clickable = ((thisroom.Objects[i].Flags & OBJF_NOINTERACT) == 0);
        obj->Baseline = thisroom.Objects[i].Baseline;
        obj->Name = gcnew String(thisroom.Objects[i].ScriptName);
        obj->Description = gcnew String(thisroom.Objects[i].Name);
        obj->UseRoomAreaScaling = ((thisroom.Objects[i].Flags & OBJF_USEROOMSCALING) != 0);
        obj->UseRoomAreaLighting = ((thisroom.Objects[i].Flags & OBJF_USEREGIONTINTS) != 0);
        ConvertCustomProperties(obj->Properties, &thisroom.Objects[i].Properties);

        if (thisroom.DataVersion < kRoomVersion_300a)
        {
            char scriptFuncPrefix[100];
            sprintf(scriptFuncPrefix, "object%d_", i);
            ConvertInteractions(obj->Interactions, thisroom.Objects[i].Interaction.get(), gcnew String(scriptFuncPrefix), nullptr, 2);
        }
        else
        {
            CopyInteractions(obj->Interactions, thisroom.Objects[i].EventHandlers.get());
        }

        room->Objects->Add(obj);
    }

    for (size_t i = 0; i < thisroom.HotspotCount; ++i)
    {
        RoomHotspot ^hotspot = room->Hotspots[i];
        hotspot->ID = i;
        hotspot->Description = gcnew String(thisroom.Hotspots[i].Name);
        hotspot->Name = (gcnew String(thisroom.Hotspots[i].ScriptName))->Trim();
        hotspot->WalkToPoint = System::Drawing::Point(thisroom.Hotspots[i].WalkTo.X, thisroom.Hotspots[i].WalkTo.Y);
        ConvertCustomProperties(hotspot->Properties, &thisroom.Hotspots[i].Properties);

        if (thisroom.DataVersion < kRoomVersion_300a)
        {
            char scriptFuncPrefix[100];
            sprintf(scriptFuncPrefix, "hotspot%d_", i);
            ConvertInteractions(hotspot->Interactions, thisroom.Hotspots[i].Interaction.get(), gcnew String(scriptFuncPrefix), nullptr, 1);
        }
        else
        {
            CopyInteractions(hotspot->Interactions, thisroom.Hotspots[i].EventHandlers.get());
        }
    }

    for (size_t i = 0; i <= MAX_WALK_AREAS; ++i)
    {
        RoomWalkableArea ^area = room->WalkableAreas[i];
        area->ID = i;
        area->AreaSpecificView = thisroom.WalkAreas[i].Light;
        area->UseContinuousScaling = !(thisroom.WalkAreas[i].ScalingNear == NOT_VECTOR_SCALED);
        area->ScalingLevel = thisroom.WalkAreas[i].ScalingFar + 100;
        area->MinScalingLevel = thisroom.WalkAreas[i].ScalingFar + 100;
        if (area->UseContinuousScaling)
        {
            area->MaxScalingLevel = thisroom.WalkAreas[i].ScalingNear + 100;
        }
        else
        {
            area->MaxScalingLevel = area->MinScalingLevel;
        }
    }

    for (size_t i = 0; i < MAX_WALK_BEHINDS; ++i)
    {
        RoomWalkBehind ^area = room->WalkBehinds[i];
        area->ID = i;
        area->Baseline = thisroom.WalkBehinds[i].Baseline;
    }

    for (size_t i = 0; i < MAX_ROOM_REGIONS; ++i)
    {
        RoomRegion ^area = room->Regions[i];
        area->ID = i;
        // NOTE: Region's light level value exposed in editor is always 100 units higher,
        // for compatibility with older versions of the editor.
        // TODO: probably we could remove this behavior? Need to consider possible compat mode
        area->LightLevel = thisroom.GetRegionLightLevel(i) + 100;
        area->UseColourTint = thisroom.HasRegionTint(i);
        area->BlueTint = (thisroom.Regions[i].Tint >> 16) & 0x00ff;
        area->GreenTint = (thisroom.Regions[i].Tint >> 8) & 0x00ff;
        area->RedTint = thisroom.Regions[i].Tint & 0x00ff;
        // Set saturation's and luminance's default values in the editor if it is disabled in room data
        int saturation = (thisroom.Regions[i].Tint >> 24) & 0xFF;
        area->TintSaturation = (saturation > 0 && area->UseColourTint) ? saturation :
                               Utilities::GetDefaultValue(area->GetType(), "TintSaturation", 0);
        int luminance = thisroom.GetRegionTintLuminance(i);
        area->TintLuminance = area->UseColourTint ? luminance :
                              Utilities::GetDefaultValue(area->GetType(), "TintLuminance", 0);

        if (thisroom.DataVersion < kRoomVersion_300a)
        {
            char scriptFuncPrefix[100];
            sprintf(scriptFuncPrefix, "region%d_", i);
            ConvertInteractions(area->Interactions, thisroom.Regions[i].Interaction.get(), gcnew String(scriptFuncPrefix), nullptr, 0);
        }
        else
        {
            CopyInteractions(area->Interactions, thisroom.Regions[i].EventHandlers.get());
        }
    }

    room->_roomStructPtr = (IntPtr)&thisroom;

    ConvertCustomProperties(room->Properties, &thisroom.Properties);

    if (thisroom.DataVersion < kRoomVersion_300a)
    {
        ConvertInteractions(room->Interactions, thisroom.Interaction.get(), "room_", nullptr, 0);
    }
    else
    {
        CopyInteractions(room->Interactions, thisroom.EventHandlers.get());
    }

    room->GameID = thisroom.GameID;
    clear_undo_buffer();

    return room;
}

void save_crm_file(Room ^room)
{
    thisroom.FreeMessages();
    thisroom.FreeScripts();

    //
    // Convert managed Room object into the native roomstruct that is going
    // to be saved using native procedure.
    //
    thisroom.SetResolution((AGS::Common::RoomResolutionType)room->Resolution);
    thisroom.MaskResolution = room->MaskResolution;

    thisroom.GameID = room->GameID;
    thisroom.Edges.Bottom = room->BottomEdgeY;
    thisroom.Edges.Left = room->LeftEdgeX;
    thisroom.Options.StartupMusic = (int)room->MusicVolumeAdjustment;
    thisroom.Options.PlayerView = room->PlayerCharacterView;
    thisroom.Options.StartupMusic = room->PlayMusicOnRoomLoad;
    thisroom.Edges.Right = room->RightEdgeX;
    thisroom.Options.SaveLoadDisabled = room->SaveLoadEnabled ? 0 : 1;
    thisroom.Options.PlayerCharOff = room->ShowPlayerCharacter ? 0 : 1;
    thisroom.Edges.Top = room->TopEdgeY;
    thisroom.Width = room->Width;
    thisroom.Height = room->Height;
    thisroom.BgAnimSpeed = room->BackgroundAnimationDelay;
    thisroom.BgFrameCount = room->BackgroundCount;

    thisroom.MessageCount = room->Messages->Count;
    for (size_t i = 0; i < thisroom.MessageCount; ++i)
    {
        RoomMessage ^newMessage = room->Messages[i];
        thisroom.Messages[i] = ConvertStringToNativeString(newMessage->Text);
        if (newMessage->ShowAsSpeech)
        {
            thisroom.MessageInfos[i].DisplayAs = newMessage->CharacterID + 1;
        }
        else
        {
            thisroom.MessageInfos[i].DisplayAs = 0;
        }
        thisroom.MessageInfos[i].Flags = 0;
        if (newMessage->DisplayNextMessageAfter) thisroom.MessageInfos[i].Flags |= MSG_DISPLAYNEXT;
        if (newMessage->AutoRemoveAfterTime) thisroom.MessageInfos[i].Flags |= MSG_TIMELIMIT;
    }

    thisroom.ObjectCount = room->Objects->Count;
    for (size_t i = 0; i < thisroom.ObjectCount; ++i)
    {
        RoomObject ^obj = room->Objects[i];
        thisroom.Objects[i].ScriptName = ConvertStringToNativeString(obj->Name);

        thisroom.Objects[i].Sprite = obj->Image;
        thisroom.Objects[i].X = obj->StartX;
        thisroom.Objects[i].Y = obj->StartY;
        thisroom.Objects[i].IsOn = obj->Visible;
        thisroom.Objects[i].Baseline = obj->Baseline;
        thisroom.Objects[i].Name = ConvertStringToNativeString(obj->Description);
        thisroom.Objects[i].Flags = 0;
        if (obj->UseRoomAreaScaling) thisroom.Objects[i].Flags |= OBJF_USEROOMSCALING;
        if (obj->UseRoomAreaLighting) thisroom.Objects[i].Flags |= OBJF_USEREGIONTINTS;
        if (!obj->Clickable) thisroom.Objects[i].Flags |= OBJF_NOINTERACT;
        CompileCustomProperties(obj->Properties, &thisroom.Objects[i].Properties);
    }

    thisroom.HotspotCount = room->Hotspots->Count;
    for (size_t i = 0; i < thisroom.HotspotCount; ++i)
    {
        RoomHotspot ^hotspot = room->Hotspots[i];
        thisroom.Hotspots[i].Name = ConvertStringToNativeString(hotspot->Description);
        thisroom.Hotspots[i].ScriptName = ConvertStringToNativeString(hotspot->Name);
        thisroom.Hotspots[i].WalkTo.X = hotspot->WalkToPoint.X;
        thisroom.Hotspots[i].WalkTo.Y = hotspot->WalkToPoint.Y;
        CompileCustomProperties(hotspot->Properties, &thisroom.Hotspots[i].Properties);
    }

    for (size_t i = 0; i <= MAX_WALK_AREAS; ++i)
    {
        RoomWalkableArea ^area = room->WalkableAreas[i];
        thisroom.WalkAreas[i].Light = area->AreaSpecificView;

        if (area->UseContinuousScaling)
        {
            thisroom.WalkAreas[i].ScalingFar = area->MinScalingLevel - 100;
            thisroom.WalkAreas[i].ScalingNear = area->MaxScalingLevel - 100;
        }
        else
        {
            thisroom.WalkAreas[i].ScalingFar = area->ScalingLevel - 100;
            thisroom.WalkAreas[i].ScalingNear = NOT_VECTOR_SCALED;
        }
    }

    for (size_t i = 0; i < MAX_WALK_BEHINDS; ++i)
    {
        RoomWalkBehind ^area = room->WalkBehinds[i];
        thisroom.WalkBehinds[i].Baseline = area->Baseline;
    }

    for (size_t i = 0; i < MAX_ROOM_REGIONS; ++i)
    {
        RoomRegion ^area = room->Regions[i];
        thisroom.Regions[i].Tint = 0;
        if (area->UseColourTint)
        {
            thisroom.Regions[i].Tint  = area->RedTint | (area->GreenTint << 8) | (area->BlueTint << 16) | (area->TintSaturation << 24);
            thisroom.Regions[i].Light = (area->TintLuminance * 25) / 10;
        }
        else
        {
            thisroom.Regions[i].Tint = 0;
            // NOTE: Region's light level value exposed in editor is always 100 units higher,
            // for compatibility with older versions of the editor.
            thisroom.Regions[i].Light = area->LightLevel - 100;
        }
    }

    CompileCustomProperties(room->Properties, &thisroom.Properties);

    thisroom.CompiledScript = ((AGS::Native::CompiledScript^)room->Script->CompiledData)->Data;

    AGSString roomFileName = ConvertFileNameToNativeString(room->FileName);

    TempDataStorage::RoomBeingSaved = room;

    save_room_file(roomFileName);

    TempDataStorage::RoomBeingSaved = nullptr;

    for (size_t i = 0; i < thisroom.HotspotCount; ++i)
    {
        thisroom.Hotspots[i].Name.Free(); // TODO: not sure if makes sense here
    }
}

PInteractionScripts convert_interaction_scripts(Interactions ^interactions)
{
    AGS::Common::InteractionScripts *native_scripts = new AGS::Common::InteractionScripts();
    for each (String^ funcName in interactions->ScriptFunctionNames)
    {
        native_scripts->ScriptFuncNames.push_back(ConvertStringToNativeString(funcName));
    }
    return PInteractionScripts(native_scripts);
}

void convert_room_interactions_to_native()
{
    Room ^roomBeingSaved = TempDataStorage::RoomBeingSaved;
    thisroom.EventHandlers = convert_interaction_scripts(roomBeingSaved->Interactions);
    for (int i = 0; i < roomBeingSaved->Hotspots->Count; ++i)
    {
        thisroom.Hotspots[i].EventHandlers = convert_interaction_scripts(roomBeingSaved->Hotspots[i]->Interactions);
    }
    for (int i = 0; i < roomBeingSaved->Objects->Count; ++i)
    {
        thisroom.Objects[i].EventHandlers = convert_interaction_scripts(roomBeingSaved->Objects[i]->Interactions);
    }
    for (int i = 0; i < roomBeingSaved->Regions->Count; ++i)
    {
        thisroom.Regions[i].EventHandlers = convert_interaction_scripts(roomBeingSaved->Regions[i]->Interactions);
    }
}



#pragma unmanaged


void save_room_file(const char *path)
{
    thisroom.DataVersion = kRoomVersion_Current;
    copy_room_palette_to_global_palette();

    calculate_walkable_areas();

    thisroom.BackgroundBPP = thisroom.BgFrames[0].Graphic->GetBPP();
    // Fix hi-color screens
    // TODO: find out why this is needed; may be related to the Allegro internal 16-bit bitmaps format
    for (size_t i = 0; i < (size_t)thisroom.BgFrameCount; ++i)
        fix_block(thisroom.BgFrames[i].Graphic.get());

    thisroom.WalkBehindCount = MAX_WALK_BEHINDS; // TODO: why we need to do this here?

    // Prepare script links
    convert_room_interactions_to_native();

    Stream *out = AGS::Common::File::CreateFile(path);
    if (out == NULL)
        quit("save_room: unable to open room file for writing.");

    AGS::Common::HRoomFileError err = AGS::Common::WriteRoomData(&thisroom, out, kRoomVersion_Current);
    delete out;
    if (!err)
        quit(AGSString::FromFormat("save_room: unable to write room data, error was:\r\n%s", err->FullMessage()));

    // Fix hi-color screens back again
    // TODO: find out why this is needed; may be related to the Allegro internal 16-bit bitmaps format
    for (size_t i = 0; i < thisroom.BgFrameCount; ++i)
        fix_block(thisroom.BgFrames[i].Graphic.get());
}


*/

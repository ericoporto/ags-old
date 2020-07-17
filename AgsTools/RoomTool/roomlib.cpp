#include <core/assetmanager.h>
#include <ac/gamesetupstruct.h>
#include <ac/spritecache.h>
#include <util/filestream.h>
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

RoomStruct this_room;
typedef AGS::Common::Bitmap AGSBitmap;
typedef AGS::Common::PBitmap PBitmap;
typedef AGS::Common::String AGSString;
const int MAX_PATH = 260;
color* room_palette = NULL;
GameSetupStruct thisgame;
SpriteCache this_spriteset(thisgame.SpriteInfos);
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

void quit_with_error(const char * message)
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
            thisColourOnlyPal[selCol] = room_palette[selCol];
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

        set_palette(room_palette);
    }
    else
    {
        Cstretch_sprite(ds, source, 0, 0, dest_width, dest_height);
    }
}

void drawBlockScaledAt (int hdc, Common::Bitmap *todraw ,int x, int y, float scaleFactor) {
    //if (todraw->GetColorDepth () == 8)
//        set_palette_to_hdc ((HDC)hdc, room_palette);

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
    this_room.Free();
    allegro_exit();
    Common::AssetManager::DestroyInstance();
}

void validate_mask(Common::Bitmap *toValidate, const char *name, int maxColour) {
    if ((toValidate == NULL) || (toValidate->GetColorDepth() != 8) ||
        (!toValidate->IsMemoryBitmap())) {
        quit_with_error("Invalid mask passed to validate_mask");
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
    load_room(rtlo, &this_room, thisgame.IsLegacyHiRes(), thisgame.SpriteInfos);

    // Allocate enough memory to add extra variables
    this_room.LocalVariables.resize(MAX_GLOBAL_VARIABLES);

    for (size_t i = 0; i < this_room.ObjectCount; ++i) {
        // change invalid objects to blue cup
        // TODO: should this be done in the common native lib?
        if (this_spriteset[this_room.Objects[i].Sprite] == NULL)
            this_room.Objects[i].Sprite = 0;
    }

    set_palette_range(room_palette, 0, 255, 0);

    if ((this_room.BgFrames[0].Graphic->GetColorDepth () > 8) &&
        (thisgame.color_depth == 1))
        printf("WARNING: This room is hi-color, but your game is currently 256-colour. You will not be able to use this room in this game. Also, the room background will not look right in the editor.");

    RoomTools->roomModified = false;

    validate_mask(this_room.HotspotMask.get(), "hotspot", MAX_ROOM_HOTSPOTS);
    validate_mask(this_room.WalkBehindMask.get(), "walk-behind", MAX_WALK_AREAS + 1);
    validate_mask(this_room.WalkAreaMask.get(), "walkable area", MAX_WALK_AREAS + 1);
    validate_mask(this_room.RegionMask.get(), "regions", MAX_ROOM_REGIONS);
    return nullptr;
}


void calculate_walkable_areas () {
    int ww, thispix;

    for (ww = 0; ww <= MAX_WALK_AREAS; ww++) {
        this_room.WalkAreas[ww].Top = this_room.Height;
        this_room.WalkAreas[ww].Bottom = 0;
    }
    for (ww = 0; ww < this_room.WalkAreaMask->GetWidth(); ww++) {
        for (int qq = 0; qq < this_room.WalkAreaMask->GetHeight(); qq++) {
            thispix = this_room.WalkAreaMask->GetPixel (ww, qq);
            if (thispix > MAX_WALK_AREAS)
                continue;
            if (this_room.WalkAreas[thispix].Top > qq)
                this_room.WalkAreas[thispix].Top = qq;
            if (this_room.WalkAreas[thispix].Bottom < qq)
                this_room.WalkAreas[thispix].Bottom= qq;
        }
    }

}

void save_room_file(const char *path)
{
    this_room.DataVersion = kRoomVersion_Current;

    calculate_walkable_areas();

    this_room.BackgroundBPP = this_room.BgFrames[0].Graphic->GetBPP();

    this_room.WalkBehindCount = MAX_WALK_BEHINDS; // TODO: why we need to do this here?

    // Prepare script links
    //convert_room_interactions_to_native();

    Stream *out = AGS::Common::File::CreateFile(path);
    if (out == NULL)
        quit_with_error("save_room: unable to open room file for writing.");

    AGS::Common::HRoomFileError err = AGS::Common::WriteRoomData(&this_room, out, kRoomVersion_Current);
    delete out;
    if (!err)
        quit_with_error(AGSString::FromFormat("save_room: unable to write room data, error was:\r\n%s", err->FullMessage().GetCStr()));
}

void ModifyRoomScript(const char * room_path, const char * script_path) {
    AGSString roomFileName = AGSString(room_path);

    const char *errorMsg = load_room_file(roomFileName);
    if (errorMsg != nullptr)
    {
        throw std::runtime_error(errorMsg);
    }

    PScript scriptObject;
    AGS::Common::FileStream* script_file_stream = (AGS::Common::FileStream*) AGS::Common::File::OpenFileRead(script_path);
    scriptObject->Read(script_file_stream);
    script_file_stream->Close();

    this_room.CompiledScript = scriptObject;

    save_room_file(room_path);
}


#include <stdio.h>
void ThrowManagedException(const char *message);
#pragma unmanaged
#pragma warning (disable: 4996 4312)  // disable deprecation warnings
extern bool Scintilla_RegisterClasses(void *hInstance);
extern int Scintilla_LinkLexers();

#define NOMINMAX
#include "agsnative.h"
#include <allegro.h>
#include <winalleg.h>
#undef CreateFile
#undef DeleteFile
#include "util/misc.h"
#include "util/wgt2allg.h"
#include "ac/spritecache.h"
#include "ac/gamesetupstruct.h"
#include "font/fonts.h"
#include "game/main_game_file.h"
#include "game/plugininfo.h"
#include "game/room_file.h"
#include "game/roomstruct.h"
#include "gui/guimain.h"
#include "gui/guiinv.h"
#include "gui/guibutton.h"
#include "gui/guilabel.h"
#include "gui/guitextbox.h"
#include "gui/guilistbox.h"
#include "gui/guislider.h"
#include "util/compress.h"
#include "util/string_types.h"
#include "util/string_utils.h"    // fputstring, etc
#include "util/alignedstream.h"
#include "util/directory.h"
#include "util/filestream.h"
#include "util/path.h"
#include "gfx/bitmap.h"
#include "core/assetmanager.h"
#include "NativeUtils.h"

using AGS::Types::AGSEditorException;

using AGS::Common::AlignedStream;
using AGS::Common::Stream;
using AGS::Common::AssetLibInfo;
using AGS::Common::AssetManager;
using AGS::Common::AssetMgr;
namespace AGSProps = AGS::Common::Properties;
namespace BitmapHelper = AGS::Common::BitmapHelper;
using AGS::Common::GUIMain;
using AGS::Common::RoomStruct;
using AGS::Common::PInteractionScripts;
typedef AGS::Common::String AGSString;
namespace AGSDirectory = AGS::Common::Directory;
namespace AGSFile = AGS::Common::File;
namespace AGSPath = AGS::Common::Path;

typedef System::Drawing::Bitmap SysBitmap;
typedef AGS::Common::Bitmap AGSBitmap;
typedef AGS::Common::PBitmap PBitmap;

typedef AGS::Common::Error AGSError;
typedef AGS::Common::HError HAGSError;

// TODO: do something with this later
// (those are from 'cstretch' unit)
extern void Cstretch_blit(BITMAP *src, BITMAP *dst, int sx, int sy, int sw, int sh, int dx, int dy, int dw, int dh);
extern void Cstretch_sprite(BITMAP *dst, BITMAP *src, int x, int y, int w, int h);

inline void Cstretch_blit(Common::Bitmap *src, Common::Bitmap *dst, int sx, int sy, int sw, int sh, int dx, int dy, int dw, int dh)
{
	Cstretch_blit(src->GetAllegroBitmap(), dst->GetAllegroBitmap(), sx, sy, sw, sh, dx, dy, dw, dh);
}
inline void Cstretch_sprite(Common::Bitmap *dst, Common::Bitmap *src, int x, int y, int w, int h)
{
	Cstretch_sprite(dst->GetAllegroBitmap(), src->GetAllegroBitmap(), x, y, w, h);
}

void save_room_file(const AGSString &path);

int mousex = 0, mousey = 0;
int antiAliasFonts = 0;
bool outlineGuiObjects = false;
RGB*palette = NULL;
GameSetupStruct thisgame;
SpriteCache spriteset(thisgame.SpriteInfos);
GUIMain tempgui;
const char *sprsetname = "acsprset.spr";
const char *sprindexname = "sprindex.dat";
const char *old_editor_data_file = "editor.dat";
const char *new_editor_data_file = "game.agf";
const char *old_editor_main_game_file = "ac2game.dta";
const char *TEMPLATE_LOCK_FILE = "template.dta";
const char *ROOM_TEMPLATE_ID_FILE = "rtemplate.dat";
const int ROOM_TEMPLATE_ID_FILE_SIGNATURE = 0x74673812;
bool spritesModified = false;
RoomStruct thisroom;

GameDataVersion loaded_game_file_version = kGameVersion_Current;

// stuff for importing old games
DialogTopic *dialog = NULL;
std::vector<GUIMain> guis;
ViewStruct *newViews = NULL;
int numNewViews = 0;

// A reference color depth, for correct color selection;
// originally was defined by 'abuf' bitmap.
int BaseColorDepth = 0;


struct NativeRoomTools
{
    int loaded_room_number = -1;
};
std::unique_ptr<NativeRoomTools> RoomTools;


bool reload_font(int curFont);
void drawBlockScaledAt(int hdc, Common::Bitmap *todraw ,int x, int y, float scaleFactor);
// this is to shut up the linker, it's used by CSRUN.CPP
void write_log(const char *) { }
SysBitmap^ ConvertBlockToBitmap(Common::Bitmap *todraw, bool useAlphaChannel);

// jibbles the sprite around to fix hi-color problems, by swapping
// the red and blue elements
// TODO: find out if this is actually needed. I guess this may have something to do
// with Allegro keeping 16-bit bitmaps in an unusual format internally.
#define fix_sprite(num) fix_block(spriteset[num])
void fix_block (Common::Bitmap *todraw) {
  int a,b,pixval;
  if (todraw == NULL)
    return;
  // TODO: redo this using direct bitmap data access for the sake of speed
  if (todraw->GetColorDepth() == 16) {
    for (a = 0; a < todraw->GetWidth(); a++) {
      for (b = 0; b < todraw->GetHeight(); b++) {
        pixval = todraw->GetPixel (a, b);
        todraw->PutPixel (a, b, makecol16 (getb16(pixval),getg16(pixval),getr16(pixval)));
      }
    }
  }
  else if (todraw->GetColorDepth() == 32) {
    for (a = 0; a < todraw->GetWidth(); a++) {
      for (b = 0; b < todraw->GetHeight(); b++) {
        pixval = todraw->GetPixel (a, b);
        todraw->PutPixel (a, b, makeacol32 (getb32(pixval),getg32(pixval),getr32(pixval), geta32(pixval)));
      }
    }
  }
}

void initialize_sprite(int spnum) {
  fix_sprite(spnum);
}

void pre_save_sprite(int spnum) {
  fix_sprite(spnum);
}

Common::Bitmap *get_sprite (int spnr) {
  if (spnr < 0)
    return NULL;
  if (spriteset[spnr] == NULL) {
    spnr = 0;
  }
  return spriteset[spnr];
}

void SetNewSprite(int slot, Common::Bitmap *sprit) {
  delete spriteset[slot];

  spriteset.SetSprite(slot, sprit);
  spritesModified = true;
}

void deleteSprite (int sprslot) {
  spriteset.RemoveSprite(sprslot, true);
  
  spritesModified = true;
}

void SetNewSpriteFromHBitmap(int slot, int hBmp) {
  // FIXME later
  Common::Bitmap *tempsprite = Common::BitmapHelper::CreateRawBitmapOwner(convert_hbitmap_to_bitmap((HBITMAP)hBmp));
  SetNewSprite(slot, tempsprite);
}

int GetSpriteAsHBitmap(int slot) {
  // FIXME later
  return (int)convert_bitmap_to_hbitmap(get_sprite(slot)->GetAllegroBitmap());
}

bool DoesSpriteExist(int slot) {
	return (spriteset[slot] != NULL);
}

int GetMaxSprites() {
	return MAX_STATIC_SPRITES;
}

int GetSpriteWidth(int slot) {
	return get_sprite(slot)->GetWidth();
}

int GetSpriteHeight(int slot) {
	return get_sprite(slot)->GetHeight();
}

void GetSpriteInfo(int slot, ::SpriteInfo &info) {
    // TODO: find out if we may get width/height from SpriteInfos
    // or it is necessary to go through get_sprite and check bitmaps in cache?
    info.Width = GetSpriteWidth(slot);
    info.Height = GetSpriteHeight(slot);
    info.Flags = thisgame.SpriteInfos[slot].Flags;
}

unsigned char* GetRawSpriteData(int spriteSlot) {
  return &get_sprite(spriteSlot)->GetScanLineForWriting(0)[0];
}

int GetSpriteColorDepth(int slot) {
  return get_sprite(slot)->GetColorDepth();
}

int GetPaletteAsHPalette() {
  return (int)convert_palette_to_hpalette(palette);
}

void transform_string(char *text) {
	encrypt_text(text);
}

int find_free_sprite_slot() {
  return spriteset.GetFreeIndex();
}

// CLNUP probably to remove
void update_sprite_resolution(int spriteNum)
{
	thisgame.SpriteInfos[spriteNum].Flags &= ~(SPF_HIRES | SPF_VAR_RESOLUTION);
}

void change_sprite_number(int oldNumber, int newNumber) {

  spriteset.SetSprite(newNumber, spriteset[oldNumber]);
  spriteset.RemoveSprite(oldNumber, false);

  thisgame.SpriteInfos[newNumber].Flags = thisgame.SpriteInfos[oldNumber].Flags;
  thisgame.SpriteInfos[oldNumber].Flags = 0;

  spritesModified = true;
}

int crop_sprite_edges(int numSprites, int *sprites, bool symmetric) {
  // this function has passed in a list of sprites, all the
  // same size, to crop to the size of the smallest
  int aa, xx, yy;
  int width = spriteset[sprites[0]]->GetWidth();
  int height = spriteset[sprites[0]]->GetHeight();
  int left = width, right = 0;
  int top = height, bottom = 0;

  for (aa = 0; aa < numSprites; aa++) {
    Common::Bitmap *sprit = get_sprite(sprites[aa]);
    int maskcol = sprit->GetMaskColor();

    // find the left hand side
    for (xx = 0; xx < width; xx++) {
      for (yy = 0; yy < height; yy++) {
        if (sprit->GetPixel(xx, yy) != maskcol) {
          if (xx < left)
            left = xx;
          xx = width + 10;
          break;
        }
      }
    }
    // find the right hand side
    for (xx = width - 1; xx >= 0; xx--) {
      for (yy = 0; yy < height; yy++) {
        if (sprit->GetPixel(xx, yy) != maskcol) {
          if (xx > right)
            right = xx;
          xx = -10;
          break;
        }
      }
    }
    // find the top side
    for (yy = 0; yy < height; yy++) {
      for (xx = 0; xx < width; xx++) {
        if (sprit->GetPixel(xx, yy) != maskcol) {
          if (yy < top)
            top = yy;
          yy = height + 10;
          break;
        }
      }
    }
    // find the bottom side
    for (yy = height - 1; yy >= 0; yy--) {
      for (xx = 0; xx < width; xx++) {
        if (sprit->GetPixel(xx, yy) != maskcol) {
          if (yy > bottom)
            bottom = yy;
          yy = -10;
          break;
        }
      }
    }
  }

  // Now, we have been through all the sprites and found the outer
  // edges that we need to keep. So, now crop everything down
  // to the smaller sizes
  if (symmetric) {
    // symmetric -- make sure that the left and right edge chopping
    // are equal
    int rightDist = (width - right) - 1;
    if (rightDist < left)
      left = rightDist;
    if (left < rightDist)
      right = (width - left) - 1;
  }
  int newWidth = (right - left) + 1;
  int newHeight = (bottom - top) + 1;

  if ((newWidth == width) && (newHeight == height)) {
    // no change in size
    return 0;
  }

  if ((newWidth < 1) || (newHeight < 1))
  {
	  // completely transparent sprite, don't attempt to crop
	  return 0;
  }

  for (aa = 0; aa < numSprites; aa++) {
    Common::Bitmap *sprit = get_sprite(sprites[aa]);
    // create a new, smaller sprite and copy across
	Common::Bitmap *newsprit = Common::BitmapHelper::CreateBitmap(newWidth, newHeight, sprit->GetColorDepth());
    newsprit->Blit(sprit, left, top, 0, 0, newWidth, newHeight);
    delete sprit;

    spriteset.SetSprite(sprites[aa], newsprit);
  }

  spritesModified = true;

  return 1;
}

HAGSError extract_room_template_files(const AGSString &templateFileName, int newRoomNumber)
{
  const AssetLibInfo *lib = nullptr;
  std::unique_ptr<AssetManager> templateMgr(new AssetManager());
  auto err = templateMgr->AddLibrary(templateFileName, &lib);
  if (err != Common::kAssetNoError) 
  {
    return new AGSError("Failed to read the template file.", Common::GetAssetErrorText(err));
  }
  if (!templateMgr->DoesAssetExist(ROOM_TEMPLATE_ID_FILE))
  {
    return new AGSError("Template file does not contain necessary metadata.");
  }

  size_t numFile = lib->AssetInfos.size();
  for (size_t a = 0; a < numFile; a++) {
    AGSString thisFile = lib->AssetInfos[a].FileName;
    if (thisFile.IsEmpty())
      continue; // should not normally happen

    // don't extract the template metadata file
    if (thisFile.CompareNoCase(ROOM_TEMPLATE_ID_FILE) == 0)
      continue;

    soff_t size = 0;
    Stream *readin = templateMgr->OpenAsset(thisFile, &size);
    if (!readin)
    {
      return new AGSError(AGSString::FromFormat("Failed to open template asset '%s' for reading.", thisFile.GetCStr()));
    }
    char outputName[MAX_PATH];
    AGSString extension = AGSPath::GetFileExtension(thisFile);
    sprintf(outputName, "room%d%s", newRoomNumber, extension.GetCStr());
    Stream *wrout = AGSFile::CreateFile(outputName);
    if (!wrout) 
    {
      delete readin;
      return new AGSError(AGSString::FromFormat("Failed to open file '%s' for writing.", outputName));
    }
    
    char *membuff = new char[size];
    readin->Read(membuff, size);
    wrout->Write(membuff, size);
    delete readin;
    delete wrout;
    delete[] membuff;
  }

  return HAGSError::None();
}

HAGSError extract_template_files(const AGSString &templateFileName)
{
  const AssetLibInfo *lib = nullptr;
  std::unique_ptr<AssetManager> templateMgr(new AssetManager());
  auto err = templateMgr->AddLibrary(templateFileName, &lib);
  if (err != Common::kAssetNoError) 
  {
    return new AGSError("Failed to read the template file", Common::GetAssetErrorText(err));
  }
  if ((!templateMgr->DoesAssetExist(old_editor_data_file)) && (!templateMgr->DoesAssetExist(new_editor_data_file)))
  {
    return new AGSError("Template file does not contain main project data.");
  }

  size_t numFile = lib->AssetInfos.size();
  for (size_t a = 0; a < numFile; a++) {
    AGSString thisFile = lib->AssetInfos[a].FileName;
    if (thisFile.IsEmpty())
      continue; // should not normally happen

    // don't extract the dummy template lock file
    if (thisFile.CompareNoCase(TEMPLATE_LOCK_FILE) == 0)
      continue;

    soff_t size = 0;
    Stream *readin = templateMgr->OpenAsset(thisFile, &size);
    if (!readin)
    {
      return new AGSError(AGSString::FromFormat("Failed to open template asset '%s' for reading.", thisFile.GetCStr()));
    }
    // Make sure to create necessary subfolders,
    // e.g. if it's an old template with Music & Sound folders
    AGSDirectory::CreateAllDirectories(".", AGSPath::GetDirectoryPath(thisFile));
    Stream *wrout = AGSFile::CreateFile(thisFile);
    if (!wrout)
    {
      delete readin;
      return new AGSError(AGSString::FromFormat("Failed to open file '%s' for writing.", thisFile.GetCStr()));
    }
    char *membuff = new char[size];
    readin->Read(membuff, size);
    wrout->Write(membuff, size);
    delete readin;
    delete wrout;
    delete[] membuff;
  }

  return HAGSError::None();
}

void extract_icon_from_template(AssetManager *templateMgr, char *iconName, char **iconDataBuffer, long *bufferSize)
{
  // make sure we get the icon from the file
  templateMgr->SetSearchPriority(Common::kAssetPriorityLib);
  soff_t sizey = 0;
  Stream* inpu = templateMgr->OpenAsset (iconName, &sizey);
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
}

int load_template_file(const AGSString &fileName, char **iconDataBuffer, long *iconDataSize, bool isRoomTemplate)
{
  const AssetLibInfo *lib = nullptr;
  std::unique_ptr<AssetManager> templateMgr(new AssetManager());
  if (templateMgr->AddLibrary(fileName, &lib) == Common::kAssetNoError)
  {
    if (isRoomTemplate)
    {
      if (templateMgr->DoesAssetExist(ROOM_TEMPLATE_ID_FILE))
      {
        Stream *inpu = templateMgr->OpenAsset(ROOM_TEMPLATE_ID_FILE);
        if (inpu->ReadInt32() != ROOM_TEMPLATE_ID_FILE_SIGNATURE)
        {
          delete inpu;
          return 0;
        }
        int roomNumber = inpu->ReadInt32();
        delete inpu;
        char iconName[MAX_PATH];
        sprintf(iconName, "room%d.ico", roomNumber);
        if (templateMgr->DoesAssetExist(iconName))
        {
          extract_icon_from_template(templateMgr.get(), iconName, iconDataBuffer, iconDataSize);
        }
        return 1;
      }
      return 0;
    }
	  else if ((templateMgr->DoesAssetExist(old_editor_data_file)) || (templateMgr->DoesAssetExist(new_editor_data_file)))
	  {
      Common::String oriname = lib->BaseFileName;
      if ((oriname.FindString(".exe") != -1) ||
          (oriname.FindString(".dat") != -1) ||
          (oriname.FindString(".ags") != -1))
      {
        // wasn't originally meant as a template
	      return 0;
      }

	    Stream *inpu = templateMgr->OpenAsset(old_editor_main_game_file);
	    if (inpu != NULL) 
	    {
		    inpu->Seek(30);
		    int gameVersion = inpu->ReadInt32();
		    delete inpu;
            // TODO: check this out, in theory we still support pre-2.72 game import
		    if (gameVersion != 32)
		    {
			    // older than 2.72 template
			    return 0;
		    }
	    }

      int useIcon = 0;
      char *iconName = "template.ico";
      if (!templateMgr->DoesAssetExist(iconName))
        iconName = "user.ico";
      // the file is a CLIB file, so let's extract the icon to display
      if (templateMgr->DoesAssetExist(iconName))
      {
        extract_icon_from_template(templateMgr.get(), iconName, iconDataBuffer, iconDataSize);
      }
      return 1;
    }
  }
  return 0;
}

void drawBlockDoubleAt (int hdc, Common::Bitmap *todraw ,int x, int y) {
  drawBlockScaledAt (hdc, todraw, x, y, 2);
}

void wputblock_stretch(Common::Bitmap *g, int xpt,int ypt,Common::Bitmap *tblock,int nsx,int nsy) {
  if (tblock->GetBPP() != thisgame.color_depth) {
    Common::Bitmap *tempst=Common::BitmapHelper::CreateBitmapCopy(tblock, thisgame.color_depth*8);
    int ww,vv;
    for (ww=0;ww<tblock->GetWidth();ww++) {
      for (vv=0;vv<tblock->GetHeight();vv++) {
        if (tblock->GetPixel(ww,vv)==tblock->GetMaskColor())
          tempst->PutPixel(ww,vv,tempst->GetMaskColor());
      }
    }
    g->StretchBlt(tempst,RectWH(xpt,ypt,nsx,nsy), Common::kBitmap_Transparency);
    delete tempst;
  }
  else g->StretchBlt(tblock,RectWH(xpt,ypt,nsx,nsy), Common::kBitmap_Transparency);
}

void draw_gui_sprite(Common::Bitmap *g, int sprnum, int atxp, int atyp, bool use_alpha, Common::BlendMode blend_mode) {
  Common::Bitmap *blptr = get_sprite(sprnum);
  Common::Bitmap *towrite=blptr;
  int needtofree=0, main_color_depth = thisgame.color_depth * 8;

  if ((blptr->GetBPP() > 1) & (main_color_depth==8)) {

    towrite=Common::BitmapHelper::CreateBitmap(blptr->GetWidth(),blptr->GetHeight(), 8);
    needtofree=1;
    towrite->Clear(towrite->GetMaskColor());
    int xxp,yyp,tmv;
    for (xxp=0;xxp<blptr->GetWidth();xxp++) {
      for (yyp=0;yyp<blptr->GetHeight();yyp++) {
        tmv=blptr->GetPixel(xxp,yyp);
        if (tmv != blptr->GetMaskColor())
          towrite->PutPixel(xxp,yyp,makecol8(getr16(tmv),getg16(tmv),getb16(tmv)));
        }
      }

    }

  int nwid=towrite->GetWidth(),nhit=towrite->GetHeight();
  wputblock_stretch(g, atxp,atyp,towrite,nwid,nhit);
  if (needtofree) delete towrite;
}

void drawBlock (HDC hdc, Common::Bitmap *todraw, int x, int y) {
  set_palette_to_hdc (hdc, palette);
  // FIXME later
  blit_to_hdc (todraw->GetAllegroBitmap(), hdc, 0,0,x,y,todraw->GetWidth(),todraw->GetHeight());
}

// CLNUP scaling stuff, need to check
  // TODO maybe turn OPT_NOSCALEFNT into the multiplier value, see also drawFontAt
AGSString import_sci_font(const AGSString &filename, int fslot) {
  char wgtfontname[100];
  sprintf(wgtfontname,"agsfnt%d.wfn",fslot);
  Stream*iii=AGSFile::OpenFileRead(filename);
  if (iii==NULL) {
    return "File not found";
  }
  if (iii->ReadByte()!=0x87) {
    delete iii;
    return "Not a valid SCI font file";
  }
  iii->Seek(3);
  if (iii->ReadInt16()!=0x80) {
    delete iii; 
	  return "Invalid SCI font"; 
  }
  int lineHeight = iii->ReadInt16();
  short theiroffs[0x80];
  iii->ReadArrayOfInt16(theiroffs,0x80);
  Stream*ooo=AGSFile::CreateFile(wgtfontname);
  ooo->Write("WGT Font File  ",15);
  ooo->WriteInt16(0);  // will be table address
  short coffsets[0x80];
  char buffer[1000];
  int aa;
  for (aa=0;aa<0x80;aa++) 
  {
    if (theiroffs[aa] < 100)
    {
      delete iii;
      delete ooo;
      unlink(wgtfontname);
      return "Invalid character found in file";
    }
    iii->Seek(theiroffs[aa]+2, Common::kSeekBegin);
    int wwi=iii->ReadByte()-1;
    int hhi=iii->ReadByte();
    coffsets[aa]=ooo->GetPosition();
    ooo->WriteInt16(wwi+1);
    ooo->WriteInt16(hhi);
    if ((wwi<1) | (hhi<1)) continue;
    memset(buffer,0,1000);
    int bytesPerRow = (wwi/8)+1;
    iii->ReadArray(buffer, bytesPerRow, hhi);
    for (int bb=0;bb<hhi;bb++) { 
      int thisoffs = bb * bytesPerRow;
      ooo->Write(&buffer[thisoffs], bytesPerRow);
    }
  }
  long tableat=ooo->GetPosition();
  ooo->WriteArrayOfInt16(&coffsets[0],0x80);
  delete ooo;
  ooo=AGSFile::OpenFile(wgtfontname,Common::kFile_Open,Common::kFile_ReadWrite);
  ooo->Seek(15, Common::kSeekBegin);
  ooo->WriteInt16(tableat); 
  delete ooo;
  delete iii;
  FontInfo fi;
  if (!wloadfont_size(fslot, fi))
  {
    return "Unable to load converted WFN file";
  }
  return NULL;
}

// CLNUP temporarily forced doubleSize to 2, it will get scaled up in the font preview, but not in the GUIs
int drawFontAt (int hdc, int fontnum, int x, int y, int width) {
  
  if (fontnum >= thisgame.numfonts)
  {
    return 0;
  }

  if (!is_font_loaded(fontnum))
    reload_font(fontnum);

  // TODO: rewrite this, use actual font size (maybe related to window size) and not game's resolution type
  int doubleSize = 2;
  int blockSize = 1;
  antiAliasFonts = thisgame.options[OPT_ANTIALIASFONTS];

  int char_height = thisgame.fonts[fontnum].SizePt * thisgame.fonts[fontnum].SizeMultiplier;
  int grid_size   = std::max(10, char_height);
  int grid_margin = std::max(4, grid_size / 4);
  grid_size += grid_margin * 2;
  grid_size *= blockSize;
  int first_char = 0;
  int num_chars  = 256;
  int padding = 5;

  if (doubleSize > 1)
      width /= 2;
  int chars_per_row = std::max(1, (width - (padding * 2)) / grid_size);
  int height = (num_chars / chars_per_row + 1) * grid_size + padding * 2;

  if (!hdc)
    return height * doubleSize;

  // we can't antialias font because changing col dep to 16 here causes
  // it to crash ... why?
  Common::Bitmap *tempblock = Common::BitmapHelper::CreateBitmap(width, height, 8);
  tempblock->Fill(0);
  color_t text_color = tempblock->GetCompatibleColor(15); // fixed white color
  for (int c = first_char; c < num_chars; ++c)
  {
    wgtprintf(tempblock,
                padding + (c % chars_per_row) * grid_size + grid_margin,
                padding + (c / chars_per_row) * grid_size + grid_margin,
                fontnum, text_color, "%c", c);
  }

  if (doubleSize > 1) 
    drawBlockDoubleAt(hdc, tempblock, x, y);
  else
    drawBlock((HDC)hdc, tempblock, x, y);
   
  delete tempblock;
  return height * doubleSize;
}

void proportionalDraw (int newwid, int sprnum, int*newx, int*newy) {
  int newhit = newwid;

  int newsizx=newwid,newsizy=newhit;
  int twid=get_sprite(sprnum)->GetWidth(),thit = get_sprite(sprnum)->GetHeight();
  if ((twid < newwid/2) && (thit < newhit/2)) {
    newsizx = twid * 2;
    newsizy = thit * 2;
  }
  else {
    if (twid >= thit) newsizy=(int)((float)thit/((float)twid/(float)newwid));
    else if (twid < thit) newsizx=(int)((float)twid/((float)thit/(float)newhit));
  }
  newx[0] = newsizx;
  newy[0] = newsizy;
}

static void doDrawViewLoop (int hdc, int numFrames, ViewFrame *frames, int x, int y, int size, int cursel) {
  int wtoDraw = size * numFrames;
  
  if ((numFrames > 0) && (frames[numFrames-1].pic == -1))
    wtoDraw -= size;

  Common::Bitmap *todraw = Common::BitmapHelper::CreateBitmap (wtoDraw, size, thisgame.color_depth*8);
  todraw->Clear (todraw->GetMaskColor ());
  int neww, newh;
  for (int i = 0; i < numFrames; i++) {
    // don't draw the Go-To-Next-Frame jibble
    if (frames[i].pic == -1)
      break;
    // work out the dimensions to stretch to
    proportionalDraw (size, frames[i].pic, &neww, &newh);
    Common::Bitmap *toblt = get_sprite(frames[i].pic);
    bool freeBlock = false;
    if (toblt->GetColorDepth () != todraw->GetColorDepth ()) {
      // 256-col sprite in hi-col game, we need to copy first
      Common::Bitmap *oldBlt = toblt;
      toblt = Common::BitmapHelper::CreateBitmap (toblt->GetWidth(), toblt->GetHeight(), todraw->GetColorDepth ());
      toblt->Blit (oldBlt, 0, 0, 0, 0, oldBlt->GetWidth(), oldBlt->GetHeight());
      freeBlock = true;
    }
    Common::Bitmap *flipped = NULL;
    if (frames[i].flags & VFLG_FLIPSPRITE) {
      // mirror the sprite
      flipped = Common::BitmapHelper::CreateBitmap (toblt->GetWidth(), toblt->GetHeight(), todraw->GetColorDepth ());
      flipped->Clear (flipped->GetMaskColor ());
      flipped->FlipBlt(toblt, 0, 0, Common::kBitmap_HFlip);
      if (freeBlock)
        delete toblt;
      toblt = flipped;
      freeBlock = true;
    }
    //->StretchBlt(toblt, todraw, 0, 0, toblt->GetWidth(), toblt->GetHeight(), size*i, 0, neww, newh);
	Cstretch_sprite(todraw, toblt, size*i, 0, neww, newh);
    if (freeBlock)
      delete toblt;
    if (i < numFrames-1) {
      int linecol = makecol_depth(thisgame.color_depth * 8, 0, 64, 200);
      if (thisgame.color_depth == 1)
        linecol = 12;

      // Draw dividing line
	  todraw->DrawLine (Line(size*(i+1) - 1, 0, size*(i+1) - 1, size-1), linecol);
    }
    if (i == cursel) {
      // Selected item
      int linecol = makecol_depth(thisgame.color_depth * 8, 255, 255,255);
      if (thisgame.color_depth == 1)
        linecol = 14;
      
      todraw->DrawRect(Rect (size * i, 0, size * (i+1) - 1, size-1), linecol);
    }
  }
  drawBlock ((HDC)hdc, todraw, x, y);
  delete todraw;
}

// CLNUP probably to remove, need to check how the flag is involved
int get_adjusted_spritewidth(int spr) {
  Common::Bitmap *tsp = get_sprite(spr);
  if (tsp == NULL)
      return 0;
  int retval = tsp->GetWidth();
  return retval;
}

// CLNUP probably to remove
int get_adjusted_spriteheight(int spr) {
  Common::Bitmap *tsp = get_sprite(spr);
  if (tsp == NULL)
      return 0;
  int retval = tsp->GetHeight();
  return retval;
}

void drawBlockOfColour(int hdc, int x,int y, int width, int height, int colNum)
{
	__my_setcolor(&colNum, colNum, BaseColorDepth);
  /*if (thisgame.color_depth > 2) {
    // convert to 24-bit colour
    int red = ((colNum >> 11) & 0x1f) * 8;
    int grn = ((colNum >> 5) & 0x3f) * 4;
    int blu = (colNum & 0x1f) * 8;
    colNum = (red << _rgb_r_shift_32) | (grn << _rgb_g_shift_32) | (blu << _rgb_b_shift_32);
  }*/

  Common::Bitmap *palbmp = Common::BitmapHelper::CreateBitmap(width, height, thisgame.color_depth * 8);
  palbmp->Clear (colNum);
  drawBlockScaledAt(hdc, palbmp, x, y, 1);
  delete palbmp;
}

/* [IKM] 2012-07-08: use the Common implementation
void NewInteractionCommand::remove () 
{
}
*/

void new_font () {
  FontInfo fi;
  wloadfont_size(thisgame.numfonts, fi);
  thisgame.fonts.push_back(FontInfo());
  thisgame.numfonts++;
}

bool initialize_native()
{
    AssetMgr.reset(new AssetManager());
    AssetMgr->AddLibrary("."); // TODO: this is for search in editor program folder, but maybe don't use implicit cwd?

	set_uformat(U_ASCII);  // required to stop ALFONT screwing up text
	install_allegro(SYSTEM_NONE, &errno, atexit);
	//set_gdi_color_format();
	palette = &thisgame.defpal[0];
	thisgame.color_depth = 2;
	//abuf = Common::BitmapHelper::CreateBitmap(10, 10, 32);
    BaseColorDepth = 32;
	thisgame.numfonts = 0;
	new_font();

	spriteset.Reset();
	HAGSError err = spriteset.InitFile(sprsetname, sprindexname);
	if (!err)
	  return false;
	spriteset.SetMaxCacheSize(100 * 1024 * 1024);  // 100 mb cache // TODO: set this up in preferences?

	if (!Scintilla_RegisterClasses (GetModuleHandle(NULL)))
      return false;

	init_font_renderer();

	RoomTools.reset(new NativeRoomTools());
	return true;
}

void shutdown_native()
{
    RoomTools.reset();
    // We must dispose all native bitmaps before shutting down the library
    thisroom.Free();
    shutdown_font_renderer();
    spriteset.Reset();
    allegro_exit();
    AssetMgr.reset();
}

void drawBlockScaledAt (int hdc, Common::Bitmap *todraw ,int x, int y, float scaleFactor) {
  if (todraw->GetColorDepth () == 8)
    set_palette_to_hdc ((HDC)hdc, palette);

  // FIXME later
  stretch_blit_to_hdc (todraw->GetAllegroBitmap(), (HDC)hdc, 0,0,todraw->GetWidth(),todraw->GetHeight(),
    x,y,todraw->GetWidth() * scaleFactor, todraw->GetHeight() * scaleFactor);
}

void drawSprite(int hdc, int x, int y, int spriteNum, bool flipImage) {

	Common::Bitmap *theSprite = get_sprite(spriteNum);

  if (theSprite == NULL)
    return;

	if (flipImage) {
		Common::Bitmap *flipped = Common::BitmapHelper::CreateBitmap (theSprite->GetWidth(), theSprite->GetHeight(), theSprite->GetColorDepth());
		flipped->FillTransparent();
		flipped->FlipBlt(theSprite, 0, 0, Common::kBitmap_HFlip);
		drawBlockScaledAt(hdc, flipped, x, y, 1);
		delete flipped;
	}
	else 
	{
		drawBlockScaledAt(hdc, theSprite, x, y, 1);
	}
}

void drawSpriteStretch(int hdc, int x, int y, int width, int height, int spriteNum, bool flipImage) {
  Common::Bitmap *todraw = get_sprite(spriteNum);
  Common::Bitmap *tempBlock = NULL;
	
  if (todraw->GetColorDepth () == 8)
    set_palette_to_hdc ((HDC)hdc, palette);

  int hdcBpp = GetDeviceCaps((HDC)hdc, BITSPIXEL);
  if (hdcBpp != todraw->GetColorDepth())
  {
	  tempBlock = Common::BitmapHelper::CreateBitmapCopy(todraw, hdcBpp);
	  todraw = tempBlock;
  }

  if (flipImage) {
    Common::Bitmap* flipped = Common::BitmapHelper::CreateBitmap(todraw->GetWidth(), todraw->GetHeight(), todraw->GetColorDepth());
    flipped->FillTransparent();
    flipped->FlipBlt(todraw, 0, 0, Common::kBitmap_HFlip);
    stretch_blit_to_hdc(flipped->GetAllegroBitmap(), (HDC)hdc, 0, 0, flipped->GetWidth(), flipped->GetHeight(), x, y, width, height);
    delete flipped;
  } else {
    // FIXME later
    stretch_blit_to_hdc(todraw->GetAllegroBitmap(), (HDC)hdc, 0, 0, todraw->GetWidth(), todraw->GetHeight(), x, y, width, height);
  }

  delete tempBlock;
}

void drawGUIAt (int hdc, int x,int y,int x1,int y1,int x2,int y2, int resolutionFactor, float scale) {

  if ((tempgui.Width < 1) || (tempgui.Height < 1))
    return;
  
  Common::Bitmap *tempblock = Common::BitmapHelper::CreateBitmap(tempgui.Width, tempgui.Height, thisgame.color_depth*8);
  tempblock->Clear(tempblock->GetMaskColor ());

  tempgui.DrawAt (tempblock, 0, 0);

  if (x1 >= 0) {
    tempblock->DrawRect(Rect (x1, y1, x2, y2), 14);
  }

  drawBlockScaledAt(hdc, tempblock, x, y, scale);
  delete tempblock;
}

#define SIMP_INDEX0  0
#define SIMP_TOPLEFT 1
#define SIMP_BOTLEFT 2
#define SIMP_TOPRIGHT 3
#define SIMP_BOTRIGHT 4
#define SIMP_LEAVEALONE 5
#define SIMP_NONE     6

// Adjusts sprite's transparency using the chosen method
void sort_out_transparency(Common::Bitmap *toimp, int sprite_import_method, RGB*itspal, int importedColourDepth,
    int &transcol)
{
  if (sprite_import_method == SIMP_LEAVEALONE)
  {
    transcol = 0;
    return;
  }

  set_palette_range(palette, 0, 255, 0);
  transcol=toimp->GetMaskColor();
  // NOTE: This takes the pixel from the corner of the overall import
  // graphic, NOT just the image to be imported
  if (sprite_import_method == SIMP_TOPLEFT)
    transcol=toimp->GetPixel(0,0);
  else if (sprite_import_method==SIMP_BOTLEFT)
    transcol=toimp->GetPixel(0,(toimp->GetHeight())-1);
  else if (sprite_import_method == SIMP_TOPRIGHT)
    transcol = toimp->GetPixel((toimp->GetWidth())-1, 0);
  else if (sprite_import_method == SIMP_BOTRIGHT)
    transcol = toimp->GetPixel((toimp->GetWidth())-1, (toimp->GetHeight())-1);

  if (sprite_import_method == SIMP_NONE)
  {
    // remove all transparency pixels (change them to
    // a close non-trnasparent colour)
    int changeTransparencyTo;
    if (transcol == 0)
      changeTransparencyTo = 16;
    else
      changeTransparencyTo = transcol - 1;

    for (int tt=0;tt<toimp->GetWidth();tt++) {
      for (int uu=0;uu<toimp->GetHeight();uu++) {
        if (toimp->GetPixel(tt,uu) == transcol)
          toimp->PutPixel(tt,uu, changeTransparencyTo);
      }
    }
  }
  else
  {
	  int bitmapMaskColor = toimp->GetMaskColor();
    int replaceWithCol = 16;
	  if (toimp->GetColorDepth() > 8)
	  {
      if (importedColourDepth == 8)
        replaceWithCol = makecol_depth(toimp->GetColorDepth(), itspal[0].r * 4, itspal[0].g * 4, itspal[0].b * 4);
      else
		    replaceWithCol = 0;
	  }
    // swap all transparent pixels with index 0 pixels
    for (int tt=0;tt<toimp->GetWidth();tt++) {
      for (int uu=0;uu<toimp->GetHeight();uu++) {
        if (toimp->GetPixel(tt,uu)==transcol)
          toimp->PutPixel(tt,uu, bitmapMaskColor);
        else if (toimp->GetPixel(tt,uu) == bitmapMaskColor)
          toimp->PutPixel(tt,uu, replaceWithCol);
      }
    }
  }
}

// Adjusts 8-bit sprite's palette
void sort_out_palette(Common::Bitmap *toimp, RGB*itspal, bool useBgSlots, int transcol)
{
  set_palette_range(palette, 0, 255, 0);
  if ((thisgame.color_depth == 1) && (itspal != NULL)) { 
    // 256-colour mode only
    if (transcol!=0)
      itspal[transcol] = itspal[0];
    wsetrgb(0,0,0,0,itspal); // set index 0 to black
    __wremap_keep_transparent = 1;
    RGB oldpale[256];
    for (int uu=0;uu<255;uu++) {
      if (useBgSlots)  //  use background scene palette
        oldpale[uu]=palette[uu];
      else if (thisgame.paluses[uu]==PAL_BACKGROUND)
        wsetrgb(uu,0,0,0,oldpale);
      else 
        oldpale[uu]=palette[uu];
    }
    wremap(itspal,toimp,oldpale); 
    set_palette_range(palette, 0, 255, 0);
  }
  else if (toimp->GetColorDepth() == 8) {  // hi-colour game
    set_palette(itspal);
  }
}

void update_abuf_coldepth() {
//  delete abuf;
//  abuf = Common::BitmapHelper::CreateBitmap(10, 10, thisgame.color_depth * 8);
    BaseColorDepth = thisgame.color_depth * 8;
}

bool reload_font(int curFont)
{
  return wloadfont_size(curFont, thisgame.fonts[curFont]);
}

HAGSError reset_sprite_file() {
  spriteset.Reset();
  HAGSError err = spriteset.InitFile(sprsetname, sprindexname);
  if (!err)
    return err;
  spriteset.SetMaxCacheSize(100 * 1024 * 1024);  // 100 mb cache // TODO: set in preferences?
  return HAGSError::None();
}

Common::PluginInfo thisgamePlugins[MAX_PLUGINS];
int numThisgamePlugins = 0;

HAGSError init_game_after_import(const AGS::Common::LoadedGameEntities &ents, GameDataVersion data_ver)
{
    numNewViews = thisgame.numviews;

    numThisgamePlugins = (int)ents.PluginInfos.size();
    for (int i = 0; i < numThisgamePlugins; ++i)
    {
        thisgamePlugins[i] = ents.PluginInfos[i];
        // we don't care if it's an editor-only plugin or not
        if (thisgamePlugins[i].Name.GetLast() == '!')
            thisgamePlugins[i].Name.ClipRight(1);
    }

    for (int i = 0; i < thisgame.numgui; ++i)
    {
        HAGSError err = guis[i].RebuildArray();
        if (!err)
            return err;
    }

    // reset colour 0, it's possible for it to get corrupted
    palette[0].r = 0;
    palette[0].g = 0;
    palette[0].b = 0;
    set_palette_range(palette, 0, 255, 0);

    HAGSError err = reset_sprite_file();
    if (!err)
        return err;

    free_all_fonts();
    for (int i = 0; i < thisgame.numfonts; ++i)
        reload_font(i);

    update_abuf_coldepth();
    spritesModified = false;
    thisgame.filever = data_ver;
    return HAGSError::None();
}

HAGSError load_dta_file_into_thisgame(const AGSString &filename)
{
    AGS::Common::MainGameSource src;
    AGS::Common::LoadedGameEntities ents(thisgame, dialog, newViews);
    HGameFileError load_err = AGS::Common::OpenMainGameFile(filename, src);
    if (load_err)
    {
        load_err = AGS::Common::ReadGameData(ents, src.InputStream.get(), src.DataVersion);
        if (load_err)
            load_err = AGS::Common::UpdateGameData(ents, src.DataVersion);
    }
    if (!load_err)
        return HAGSError(load_err);
    return init_game_after_import(ents, src.DataVersion);
}

void free_old_game_data()
{
  int bb;
  for (bb = 0; bb < thisgame.numdialog; bb++) 
  {
	  if (dialog[bb].optionscripts != NULL)
		  free(dialog[bb].optionscripts);
  }
  for (bb = 0; bb < numNewViews; bb++)
  {
    for (int cc = 0; cc < newViews[bb].numLoops; cc++)
    {
      newViews[bb].loops[cc].Dispose();
    }
    newViews[bb].Dispose();
  }
  free(newViews);
  guis.clear();
  free(dialog);

  // free game struct last because it contains object counts
  thisgame.Free();
}

// remap the scene, from its current palette oldpale to palette
void remap_background (Common::Bitmap *scene, RGB *oldpale, RGB*palette, int exactPal) {
  int a;  

  if (exactPal) {
    // exact palette import (for doing palette effects, don't change)
    for (a=0;a<256;a++) 
    {
      if (thisgame.paluses[a] == PAL_BACKGROUND)
      {
        palette[a] = oldpale[a];
      }
    }
    return;
  }

  // find how many slots there are reserved for backgrounds
  int numbgslots=0;
  for (a=0;a<256;a++) { oldpale[a].filler=0;
    if (thisgame.paluses[a]!=PAL_GAMEWIDE) numbgslots++;
  }
  // find which colours from the image palette are actually used
  int imgpalcnt[256],numimgclr=0;
  memset(&imgpalcnt[0],0,sizeof(int)*256);

  for (a=0;a<(scene->GetWidth()) * (scene->GetHeight());a++) {
    imgpalcnt[scene->GetScanLine(0)[a]]++;
  }
  for (a=0;a<256;a++) {
    if (imgpalcnt[a]>0) numimgclr++;
  }
  // count up the number of unique colours in the image
  int numclr=0,bb;
  RGB tpal[256];
  for (a=0;a<256;a++) {
    if (thisgame.paluses[a]==PAL_BACKGROUND)
      wsetrgb(a,0,0,0,palette);  // black out the bg slots before starting
    if ((oldpale[a].r==0) & (oldpale[a].g==0) & (oldpale[a].b==0)) {
      imgpalcnt[a]=0;
      continue;
    }
    for (bb=0;bb<numclr;bb++) {
      if ((oldpale[a].r==tpal[bb].r) &
        (oldpale[a].g==tpal[bb].g) &
        (oldpale[a].b==tpal[bb].b)) bb=1000;
    }
    if (bb>300) { 
      imgpalcnt[a]=0;
      continue;
    }
    if (imgpalcnt[a]==0)
      continue;
    tpal[numclr]=oldpale[a];
    numclr++;
  }
  if (numclr>numbgslots) {
    MessageBox(NULL, "WARNING: This image uses more colours than are allocated to backgrounds. Some colours will be lost.", "Warning", MB_OK);
  }

  // fill the background slots in the palette with the colours
  int palslt=255;  // start from end of palette and work backwards
  for (a=0;a<numclr;a++) {
    while (thisgame.paluses[palslt]!=PAL_BACKGROUND) {
      palslt--;
      if (palslt<0) break;
    }
    if (palslt<0) break;
    palette[palslt]=tpal[a];
    palslt--;
    if (palslt<0) break;
  }
  // blank out the sprite colours, then remap the picture
  for (a=0;a<256;a++) {
    if (thisgame.paluses[a]==PAL_GAMEWIDE) {
      tpal[a].r=0;
      tpal[a].g=0; tpal[a].b=0; 
    }
    else tpal[a]=palette[a];
  }
  wremapall(oldpale,scene,tpal); //palette);
}

void validate_mask(Common::Bitmap *toValidate, const char *name, int maxColour) {
  if ((toValidate == NULL) || (toValidate->GetColorDepth() != 8)) {
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
	MessageBox(NULL, errorBuf, "Mask Error", MB_OK);
  }
}

void copy_room_palette_to_global_palette()
{
  for (int ww = 0; ww < 256; ww++) 
  {
    if (thisgame.paluses[ww] == PAL_BACKGROUND)
    {
      thisroom.Palette[ww] = thisroom.BgFrames[0].Palette[ww];
      palette[ww] = thisroom.BgFrames[0].Palette[ww];
    }
  }
}

void copy_global_palette_to_room_palette()
{
  for (int ww = 0; ww < 256; ww++) 
  {
    if (thisgame.paluses[ww] != PAL_BACKGROUND)
      thisroom.BgFrames[0].Palette[ww] = palette[ww];
  }
}

AGSString load_room_file(const AGSString &filename) {
  load_room(filename, &thisroom, thisgame.SpriteInfos);

  // Update room palette with gamewide colours
  copy_global_palette_to_room_palette();
  // Update current global palette with room background colours
  copy_room_palette_to_global_palette();
  for (size_t i = 0; i < thisroom.ObjectCount; ++i) {
    // change invalid objects to blue cup
    // TODO: should this be done in the common native lib?
    if (spriteset[thisroom.Objects[i].Sprite] == NULL)
      thisroom.Objects[i].Sprite = 0;
  }
  // Fix hi-color screens
  // TODO: find out why this is necessary. Probably has something to do with
  // Allegro switching color components at certain version update long time ago.
  // do we need to do this in the engine too?
  for (size_t i = 0; i < thisroom.BgFrameCount; ++i)
    fix_block (thisroom.BgFrames[i].Graphic.get());

  set_palette_range(palette, 0, 255, 0);
  
  if ((thisroom.BgFrames[0].Graphic->GetColorDepth () > 8) &&
      (thisgame.color_depth == 1))
    MessageBox(NULL,"WARNING: This room is hi-color, but your game is currently 256-colour. You will not be able to use this room in this game. Also, the room background will not look right in the editor.", "Colour depth warning", MB_OK);

  validate_mask(thisroom.HotspotMask.get(), "hotspot", MAX_ROOM_HOTSPOTS);
  validate_mask(thisroom.WalkBehindMask.get(), "walk-behind", MAX_WALK_AREAS + 1);
  validate_mask(thisroom.WalkAreaMask.get(), "walkable area", MAX_WALK_AREAS + 1);
  validate_mask(thisroom.RegionMask.get(), "regions", MAX_ROOM_REGIONS);
  return AGSString();
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



// ****** CLIB MAKER **** //
#include "util/multifilelib.h"

namespace MFLUtil = AGS::Common::MFLUtil;


int copy_file_across(Stream*inlibb,Stream*coppy, soff_t leftforthis) {
  int success = 1;
  char*diskbuffer=(char*)malloc(CHUNKSIZE+10);
  while (leftforthis>0) {
    if (leftforthis>CHUNKSIZE) {
      inlibb->Read(diskbuffer,CHUNKSIZE);
      success = coppy->Write(diskbuffer,CHUNKSIZE);
      leftforthis-=CHUNKSIZE;
    }
    else {
      inlibb->Read(diskbuffer,leftforthis);
      success = coppy->Write(diskbuffer,leftforthis);
      leftforthis=0;
    }
    if (success < 1)
      break;
  }
  free(diskbuffer);
  return success;
}

// NOTE: this is used for audio.vox and speech.vox
void make_single_lib_data_file(const AGSString &dataFileName, const std::vector<AGSString> &filenames)
{
    size_t numfile = filenames.size();
    AssetLibInfo lib;

    lib.LibFileNames.resize(1);
    lib.LibFileNames[0] = AGSPath::GetFilename(dataFileName);

    lib.AssetInfos.resize(numfile);
    for (size_t i = 0; i < numfile; ++i)
    {
        AGS::Common::AssetInfo &asset = lib.AssetInfos[i];
        soff_t filesize = AGSFile::GetFileSize(filenames[i]);
        if (filesize < 0)
            ThrowManagedException((AGSString::FromFormat("Unable to retrieve file size: '%s'", filenames[i].GetCStr())).GetCStr());
        asset.FileName = AGSPath::GetFilename(filenames[i]);
        asset.Size = filesize;
    }

    // Write the header
    Stream *wout = AGSFile::CreateFile(dataFileName);
    if (!wout)
        ThrowManagedException(AGSString::FromFormat("Failed to open data file for writing: '%s'", dataFileName.GetCStr()).GetCStr());

    MFLUtil::WriteHeader(lib, MFLUtil::kMFLVersion_MultiV30, 0, wout);

    // Recalculate offsets (knowing that asset data will be positioned in sequence) and rewrite header again
    // TODO: this is not a very pleasant way to do things, need to refactor writing process to fix this
    lib.AssetInfos[0].Offset = wout->GetPosition();
    // set offsets (assets are positioned in sequence)
    for (size_t i = 1; i < numfile; ++i)
    {
        lib.AssetInfos[i].Offset = lib.AssetInfos[i - 1].Offset + lib.AssetInfos[i - 1].Size;
    }
    wout->Seek(0, AGS::Common::kSeekBegin);
    MFLUtil::WriteHeader(lib, MFLUtil::kMFLVersion_MultiV30, 0, wout);

    // now copy the data
    AGSString err_msg;
    for (size_t i = 0; i < numfile; ++i)
    {
        Stream *in = AGSFile::OpenFileRead(filenames[i]);
        if (!in)
        {
            err_msg.Format("Unable to open asset file for reading: '%s'", filenames[i].GetCStr());
            break;
        }
        int res = copy_file_across(in, wout, lib.AssetInfos[i].Size);
        delete in;
        if (res < 1)
        {
            err_msg.Format("Error writing asset into package, possibly disk full: '%s'", filenames[i].GetCStr());
            break;
        }
    }
    delete wout;

    if (!err_msg.IsEmpty())
    {
        AGSFile::DeleteFile(dataFileName);
        ThrowManagedException(err_msg.GetCStr());
    }
}

Stream* find_file_in_path(AssetManager *assetMgr, char *buffer, const char *fileName)
{
	char tomake[MAX_PATH];
	strcpy(tomake, fileName);
	Stream* iii = assetMgr->OpenAsset(tomake);
	if (iii == NULL) {
	  // try in the Audio folder if not found
	  sprintf(tomake, "AudioCache\\%s", fileName);
	  iii = assetMgr->OpenAsset(tomake);
	}
	if (iii == NULL) {
	  // no? maybe Speech then, templates include this
	  sprintf(tomake, "Speech\\%s", fileName);
	  iii = assetMgr->OpenAsset(tomake);
	}

	if (buffer != NULL)
	  strcpy(buffer, tomake);

	return iii;
}

// TODO: find out why this function is still used in some cases, instead of the method in C# class DataFileWriter
const char* make_data_file(int numFiles, char * const*fileNames, long splitSize, const char *baseFileName, bool makeFileNameAssumptionsForEXE)
{
  Stream*wout;
  char tomake[MAX_PATH];
  AssetLibInfo lib;
  lib.AssetInfos.resize(numFiles);
  AssetMgr->SetSearchPriority(Common::kAssetPriorityDir);

  int currentDataFile = 0;
  long sizeSoFar = 0;
  bool doSplitting = false;

  for (int a = 0; a < numFiles; a++)
  {
	  if (splitSize > 0)
	  {
		  if (stricmp(fileNames[a], sprsetname) == 0) 
		  {
			  // the sprite file's appearance signifies it's time to start splitting
			  doSplitting = true;
			  currentDataFile++;
			  sizeSoFar = 0;
		  }
		  else if ((sizeSoFar > splitSize) && (doSplitting) && 
			  (currentDataFile < MFLUtil::MaxMultiLibFiles - 1))
		  {
			  currentDataFile++;
			  sizeSoFar = 0;
		  }
	  }
	  soff_t thisFileSize = 0;
	  Stream *tf = AGSFile::OpenFileRead(fileNames[a]);
	  thisFileSize = tf->GetLength();
	  delete tf;
	  
	  sizeSoFar += thisFileSize;

      const char *fileNameSrc = fileNames[a];
  	  if (strrchr(fileNames[a], '\\') != NULL)
	    fileNameSrc = strrchr(fileNames[a], '\\') + 1;
	  else if (strrchr(fileNames[a], '/') != NULL)
		  fileNameSrc = strrchr(fileNames[a], '/') + 1;
      if (strlen(fileNameSrc) >= MAX_PATH)
      {
          ThrowManagedException(AGSString::FromFormat("Filename too long: %s", fileNames[a]).GetCStr());
      }
      lib.AssetInfos[a].FileName = fileNameSrc;
      lib.AssetInfos[a].LibUid = currentDataFile;
      lib.AssetInfos[a].Size = thisFileSize;
  }

  lib.LibFileNames.resize(currentDataFile + 1);

  soff_t startOffset = 0;
  char outputFileName[MAX_PATH];
  char firstDataFileFullPath[MAX_PATH];

  if (makeFileNameAssumptionsForEXE)
  {
	  _mkdir("Compiled");
  }

  // First, set up the ourlib.data_filenames array with all the filenames
  // so that write_clib_header will write the correct amount of data
  for (size_t a = 0; a < lib.LibFileNames.size(); a++)
  {
	  if (makeFileNameAssumptionsForEXE) 
	  {
          lib.LibFileNames[a].Format("%s.%03d", baseFileName, a);
		  if (a == 0)
		  {
              // TODO: this is a very bad assumption that may cause failure in certain conditions
              // we do not need to save first file extension!
              // (but fixing this will require mirror fix in the engine.
			  lib.LibFileNames[a].ReplaceMid(lib.LibFileNames[a].GetLength() - 3, 3, "exe");
		  }
	  }
	  else 
	  {
          lib.LibFileNames[a] = AGSPath::GetFilename(baseFileName);
	  }
  }

  // adjust the file paths if necessary, so that write_clib_header will
  // write the correct amount of data
  for (size_t b = 0; b < lib.AssetInfos.size(); b++) 
  {
	Stream *iii = find_file_in_path(AssetMgr.get(), tomake, lib.AssetInfos[b].FileName.GetCStr());
	if (iii != NULL)
	{
		delete iii;

		if (!makeFileNameAssumptionsForEXE)
            lib.AssetInfos[b].FileName = tomake;
	}
  }

  // now, create the actual files
  for (size_t a = 0; a < lib.LibFileNames.size(); a++)
  {
	  if (makeFileNameAssumptionsForEXE) 
	  {
		  sprintf(outputFileName, "Compiled\\%s", lib.LibFileNames[a].GetCStr());
	  }
	  else 
	  {
		  strcpy(outputFileName, baseFileName);
      }
      if (a == 0) strcpy(firstDataFileFullPath, outputFileName);

	  wout = AGSFile::OpenFile(outputFileName,
          (a == 0) ? Common::kFile_Create : Common::kFile_CreateAlways, Common::kFile_Write);
	  if (wout == NULL) 
	  {
		  return "ERROR: unable to open file for writing";
	  }

	  startOffset = wout->GetLength();

      MFLUtil::WriteHeader(lib, MFLUtil::kMFLVersion_MultiV30, a, wout);

    

    for (size_t b=0;b<lib.AssetInfos.size();b++) {
      AGS::Common::AssetInfo &asset = lib.AssetInfos[b];
      if (asset.LibUid == a) {
          asset.Offset = wout->GetPosition() - startOffset;

		Stream *iii = find_file_in_path(AssetMgr.get(), NULL, asset.FileName.GetCStr());
        if (iii == NULL) {
          delete wout;
          unlink(outputFileName);

		  char buffer[500];
		  sprintf(buffer, "Unable to find file '%s' for compilation. Do not remove files during the compilation process.", asset.FileName.GetCStr());
		  ThrowManagedException(buffer);
        }
    
        if (copy_file_across(iii,wout,asset.Size) < 1) {
          delete iii;
          return "Error writing file: possibly disk full";
        }
        delete iii;
      }
    }
	if (startOffset > 0)
	{
        MFLUtil::WriteEnder(startOffset, MFLUtil::kMFLVersion_MultiV30, wout);
	}
    delete wout;
  }

  wout = AGSFile::OpenFile(firstDataFileFullPath, Common::kFile_Open, Common::kFile_ReadWrite);
  wout->Seek(startOffset, Common::kSeekBegin);
  MFLUtil::WriteHeader(lib, MFLUtil::kMFLVersion_MultiV30, 0, wout);
  delete wout;
  return NULL;
}



// **** MANAGED CODE ****

#pragma managed
using namespace AGS::Types;
using namespace System;
using namespace System::Collections::Generic;
using namespace System::Drawing;
using namespace System::Drawing::Imaging;
using namespace System::Runtime::InteropServices;
#include "scripting.h"

public ref class TempDataStorage
{
public:
	static Room ^RoomBeingSaved;
};

void ThrowManagedException(const char *message) 
{
	throw gcnew AGS::Types::AGSEditorException(gcnew String((const char*)message));
}

void UpdateNativeSprites(SpriteFolder ^folder, std::vector<int> &missing)
{
	for each (Sprite ^sprite in folder->Sprites)
	{
        if (!spriteset.DoesSpriteExist(sprite->Number))
        {
            missing.push_back(sprite->Number);
            spriteset.SetEmptySprite(sprite->Number, true); // mark as an asset to prevent disposal on reload
        }

        int flags = 0;
		if (sprite->AlphaChannel)
            flags |= SPF_ALPHACHANNEL;
        thisgame.SpriteInfos[sprite->Number].Flags = flags;
	}

	for each (SpriteFolder^ subFolder in folder->SubFolders) 
	{
        UpdateNativeSprites(subFolder, missing);
	}
}

void UpdateNativeSpritesToGame(Game ^game, List<String^> ^errors)
{
    std::vector<int> missing;
    UpdateNativeSprites(game->RootSpriteFolder, missing);
    if (missing.size() > 0)
    {
        const size_t max_nums = 40;
        auto sprnum = gcnew System::Text::StringBuilder();
        sprnum->Append(missing[0]);
        for (size_t i = 1; i < missing.size() && i < max_nums; i++)
        {
            sprnum->Append(", ");
            sprnum->Append(missing[i]);
        }
        if (missing.size() > max_nums)
            sprnum->AppendFormat(", and {0} more.", missing.size() - max_nums);

        spritesModified = true;
        errors->Add(String::Format(
            "Sprite file (acsprset.spr) contained less sprites than the game project referenced ({0} sprites were missing). "
            "This could happen if it was not saved properly last time. Some sprites could be missing actual images. "
            "You may try restoring them by reimporting from the source files.{1}{2}Affected sprites:{3}{4}",
            missing.size(), Environment::NewLine, Environment::NewLine, Environment::NewLine, sprnum->ToString()));
    }
}

// Attempts to save the current spriteset contents into the temporary file
// provided by the system API. On success assigns saved_filename.
void SaveTempSpritefile(bool compressSprites, AGSString &saved_spritefile, AGSString &saved_indexfile)
{
    // First save new sprite set into the temporary file
    String ^temp_spritefile = nullptr;
    String ^temp_indexfile = nullptr;
    try
    {
        temp_spritefile = IO::Path::GetTempFileName();
        temp_indexfile = IO::Path::GetTempFileName();
    }
    catch (Exception ^e)
    {
        throw gcnew AGSEditorException("Unable to create a temporary file to save sprites to.", e);
    }

    AGSString n_temp_spritefile = ConvertStringToNativeString(temp_spritefile);
    AGSString n_temp_indexfile = ConvertStringToNativeString(temp_indexfile);
    SpriteFileIndex index;
    if (spriteset.SaveToFile(n_temp_spritefile, compressSprites, index) != 0)
        throw gcnew AGSEditorException(String::Format("Unable to save the sprites. An error occurred whilst writing the sprite file.{0}Temp path: {1}",
            Environment::NewLine, temp_spritefile));
    saved_spritefile = n_temp_spritefile;
    if (spriteset.SaveSpriteIndex(n_temp_indexfile, index) == 0)
        saved_indexfile = n_temp_indexfile;
}

// Updates the backup and spritefile, moving it from the temp location.
void PutNewSpritefileIntoProject(const AGSString &temp_spritefile, const AGSString &temp_indexfile)
{
    spriteset.DetachFile();
    // Now when sprites are safe, move last sprite file to backup file
    String ^sprfilename = gcnew String(sprsetname);
    String ^backupname = String::Format("backup_{0}", sprfilename);
    try
    {
        if (IO::File::Exists(backupname))
            IO::File::Delete(backupname);
        if (IO::File::Exists(sprfilename))
            IO::File::Move(sprfilename, backupname);
    }
    catch (Exception ^e)
    {// TODO: ignore for now, but proper warning output system in needed here
    }

    // And then temp file to its final location
    String ^sprindexfilename = gcnew String(sprindexname);
    try
    {
        if (IO::File::Exists(sprfilename))
            IO::File::Delete(sprfilename);
        IO::File::Move(ToStr(temp_spritefile), sprfilename);
    }
    catch (Exception ^e)
    {
        throw gcnew AGSEditorException("Unable to replace the previous sprite file in your project folder.", e);
    }

    // Sprite index is wanted but optional, so react to exceptions separately
    try
    {
        if (IO::File::Exists(sprindexfilename))
            IO::File::Delete(sprindexfilename);
        if (!temp_indexfile.IsEmpty())
            IO::File::Move(ToStr(temp_indexfile), sprindexfilename);
    }
    catch (Exception ^e)
    {// TODO: ignore for now, but proper warning output system in needed here
    }
}

void SaveNativeSprites(bool compressSprites)
{
    if (!spritesModified && (compressSprites == spriteset.IsFileCompressed()))
        return;

    AGSString saved_spritefile;
    AGSString saved_indexfile;
    SaveTempSpritefile(compressSprites, saved_spritefile, saved_indexfile);

    Exception ^main_exception;
    try
    {
        PutNewSpritefileIntoProject(saved_spritefile, saved_indexfile);
        saved_spritefile = sprsetname;
        saved_indexfile = sprindexname;
    }
    catch (Exception ^e)
    {
        main_exception = e;
    }
    finally
    {
        // Reset the sprite cache to whichever file was successfully saved
        spriteset.Reset();
        HAGSError err = spriteset.InitFile(saved_spritefile, saved_indexfile);
        if (!err)
        {
            throw gcnew AGSEditorException(
                String::Format("Unable to re-initialize sprite file after save.{0}{1}",
                    Environment::NewLine, gcnew String(err->FullMessage().GetCStr())), main_exception);
        }
        else if (err && main_exception != nullptr)
        {
            throw gcnew AGSEditorException(
                String::Format("Unable to save sprites in your project folder. The sprites were saved to a temporary location:{0}{1}",
                    Environment::NewLine, ToStr(saved_spritefile)), main_exception);
        }
    }
    spritesModified = false;
}

void SaveGame(bool compressSprites)
{
    SaveNativeSprites(compressSprites);
}

void SetGameResolution(Game ^game)
{
    thisgame.options[OPT_LETTERBOX] = 0; // always disabled now
    thisgame.SetGameResolution(::Size(game->Settings->CustomResolution.Width, game->Settings->CustomResolution.Height));
}

void GameDirChanged(String ^workingDir)
{
    AssetMgr->RemoveAllLibraries();
    AssetMgr->AddLibrary(ConvertStringToNativeString(workingDir));
}

void GameFontUpdated(Game ^game, int fontNumber, bool forceUpdate);

void GameUpdated(Game ^game, bool forceUpdate) {
  // TODO: this function may get called when only one item is added/removed or edited;
  // probably it would be best to split it up into several callbacks at some point.
  thisgame.color_depth = (int)game->Settings->ColorDepth;
  SetGameResolution(game);

  thisgame.options[OPT_RELATIVEASSETRES] = game->Settings->AllowRelativeAssetResolutions;
  thisgame.options[OPT_ANTIALIASFONTS] = game->Settings->AntiAliasFonts;
  antiAliasFonts = thisgame.options[OPT_ANTIALIASFONTS];

  //delete abuf;
  //abuf = Common::BitmapHelper::CreateBitmap(32, 32, thisgame.color_depth * 8);
  BaseColorDepth = thisgame.color_depth * 8;

  // ensure that the sprite import knows about pal slots 
  for (int i = 0; i < 256; i++) {
	if (game->Palette[i]->ColourType == PaletteColourType::Background)
	{
	  thisgame.paluses[i] = PAL_BACKGROUND;
	}
	else 
	{
  	  thisgame.paluses[i] = PAL_GAMEWIDE;
    }
  }

  // Reload native fonts and update font information in the managed component
  thisgame.numfonts = game->Fonts->Count;
  thisgame.fonts.resize(thisgame.numfonts);
  for (int i = 0; i < thisgame.numfonts; i++) 
  {
      GameFontUpdated(game, i, forceUpdate);
  }
}

void GameFontUpdated(Game ^game, int fontNumber, bool forceUpdate)
{
    FontInfo &font_info = thisgame.fonts[fontNumber];
    AGS::Types::Font ^font = game->Fonts[fontNumber];

    int old_sizept = font_info.SizePt;
    int old_scaling = font_info.SizeMultiplier;

    font_info.SizePt = font->PointSize;
    font_info.SizeMultiplier = font->SizeMultiplier;
    font_info.YOffset = font->VerticalOffset;
    font_info.LineSpacing = font->LineSpacing;
    set_fontinfo(fontNumber, font_info);

    if (forceUpdate ||
        font_info.SizePt != old_sizept ||
        font_info.SizeMultiplier != old_scaling)
    {
        reload_font(fontNumber);
    }

    font->Height = getfontheight(fontNumber);
}

void drawViewLoop (int hdc, ViewLoop^ loopToDraw, int x, int y, int size, int cursel)
{
  ::ViewFrame * frames = (::ViewFrame*)malloc(sizeof(::ViewFrame) * loopToDraw->Frames->Count);
	for (int i = 0; i < loopToDraw->Frames->Count; i++) 
	{
		frames[i].pic = loopToDraw->Frames[i]->Image;
		frames[i].flags = (loopToDraw->Frames[i]->Flipped) ? VFLG_FLIPSPRITE : 0;
	}
  // stretch_sprite is dodgy, retry a few times if it crashes
  int retries = 0;
  while (retries < 3)
  {
    try
    {
	    doDrawViewLoop(hdc, loopToDraw->Frames->Count, frames, x, y, size, cursel);
      break;
    }
    catch (AccessViolationException ^)
    {
      retries++;
    }
  }
  free(frames);
}

Common::Bitmap *CreateBlockFromBitmap(System::Drawing::Bitmap ^bmp, RGB *imgpal, bool fixColourDepth, bool keepTransparency, int *originalColDepth)
{
	int colDepth;
	if (bmp->PixelFormat == PixelFormat::Format8bppIndexed)
	{
		colDepth = 8;
	}
	else if (bmp->PixelFormat == PixelFormat::Format16bppRgb555)
	{
		colDepth = 15;
	}
	else if (bmp->PixelFormat == PixelFormat::Format16bppRgb565)
	{
		colDepth = 16;
	}
	else if (bmp->PixelFormat == PixelFormat::Format24bppRgb)
	{
		colDepth = 24;
	}
	else if (bmp->PixelFormat == PixelFormat::Format32bppRgb)
	{
		colDepth = 32;
	}
	else if (bmp->PixelFormat == PixelFormat::Format32bppArgb)
	{
		colDepth = 32;
	}
  else if ((bmp->PixelFormat == PixelFormat::Format48bppRgb) ||
           (bmp->PixelFormat == PixelFormat::Format64bppArgb) ||
           (bmp->PixelFormat == PixelFormat::Format64bppPArgb))
  {
    throw gcnew AGSEditorException("The source image is 48-bit or 64-bit colour. AGS does not support images with a colour depth higher than 32-bit. Make sure that your paint program is set to produce 32-bit images (8-bit per channel), not 48-bit images (16-bit per channel).");
  }
	else
	{
		throw gcnew AGSEditorException(gcnew System::String("Unknown pixel format"));
	}

  if ((thisgame.color_depth == 1) && (colDepth > 8))
  {
    throw gcnew AGSEditorException("You cannot import a hi-colour or true-colour image into a 256-colour game.");
  }

  if (originalColDepth != NULL)
    *originalColDepth = colDepth;

  bool needToFixColourDepth = false;
  if ((colDepth != thisgame.color_depth * 8) && (fixColourDepth))
  {
    needToFixColourDepth = true;
  }

	Common::Bitmap *tempsprite = Common::BitmapHelper::CreateBitmap(bmp->Width, bmp->Height, colDepth);
    if (!tempsprite)
        return nullptr; // out of mem?

	System::Drawing::Rectangle rect(0, 0, bmp->Width, bmp->Height);
	BitmapData ^bmpData = bmp->LockBits(rect, ImageLockMode::ReadWrite, bmp->PixelFormat);
	unsigned char *address = (unsigned char*)bmpData->Scan0.ToPointer();
	for (int y = 0; y < tempsprite->GetHeight(); y++) {
	  memcpy(&tempsprite->GetScanLineForWriting(y)[0], address, bmp->Width * ((colDepth + 1) / 8));
	  address += bmpData->Stride;
	}
	bmp->UnlockBits(bmpData);

	if (colDepth == 8)
	{
		cli::array<System::Drawing::Color> ^bmpPalette = bmp->Palette->Entries;
		for (int i = 0; i < 256; i++) {
      if (i >= bmpPalette->Length)
      {
        // BMP files can have an arbitrary palette size, fill any
        // missing colours with black
			  imgpal[i].r = 1;
			  imgpal[i].g = 1;
			  imgpal[i].b = 1;
      }
      else
      {
			  imgpal[i].r = bmpPalette[i].R / 4;
			  imgpal[i].g = bmpPalette[i].G / 4;
			  imgpal[i].b = bmpPalette[i].B / 4;

        if ((needToFixColourDepth) && (i > 0) && 
            (imgpal[i].r == imgpal[0].r) &&
            (imgpal[i].g == imgpal[0].g) && 
            (imgpal[i].b == imgpal[0].b))
        {
          // convert any (0,0,0) colours to (1,1,1) since the image
          // is about to be converted to hi-colour; this will preserve
          // any transparency
          imgpal[i].r = (imgpal[0].r < 32) ? (imgpal[0].r + 1) : (imgpal[0].r - 1);
			    imgpal[i].g = (imgpal[0].g < 32) ? (imgpal[0].g + 1) : (imgpal[0].g - 1);
			    imgpal[i].b = (imgpal[0].b < 32) ? (imgpal[0].b + 1) : (imgpal[0].b - 1);
        }
      }
		}
	}

	if (needToFixColourDepth)
	{
		Common::Bitmap *spriteAtRightDepth = Common::BitmapHelper::CreateBitmap(tempsprite->GetWidth(), tempsprite->GetHeight(), thisgame.color_depth * 8);
        if (!spriteAtRightDepth)
        {
            delete tempsprite;
            return nullptr; // out of mem?
        }

		if (colDepth == 8)
		{
			select_palette(imgpal);
		}

		int oldColorConv = get_color_conversion();
		if (keepTransparency)
		{
			set_color_conversion(oldColorConv | COLORCONV_KEEP_TRANS);
		}
		else
		{
			set_color_conversion(oldColorConv & ~COLORCONV_KEEP_TRANS);
		}

		spriteAtRightDepth->Blit(tempsprite, 0, 0, 0, 0, tempsprite->GetWidth(), tempsprite->GetHeight());

		set_color_conversion(oldColorConv);

		if (colDepth == 8) 
		{
			unselect_palette();
		}
		delete tempsprite;
		tempsprite = spriteAtRightDepth;
	}

	if (colDepth > 8) 
	{
		fix_block(tempsprite);
	}

	return tempsprite;
}

void DeleteBackground(Room ^room, int backgroundNumber) 
{
	RoomStruct *theRoom = (RoomStruct*)(void*)room->_roomStructPtr;

 if (theRoom->BgFrames[backgroundNumber].Graphic)
 {
     theRoom->BgFrames[backgroundNumber].Graphic.reset();
     theRoom->BgFrameCount--;

     for (size_t i = backgroundNumber; i < theRoom->BgFrameCount; i++)
     {
         theRoom->BgFrames[i] = theRoom->BgFrames[i + 1];
         theRoom->BgFrames[i].IsPaletteShared = theRoom->BgFrames[i + 1].IsPaletteShared;
     }
 }
}

void ImportBackground(Room ^room, int backgroundNumber, System::Drawing::Bitmap ^bmp, bool useExactPalette, bool sharePalette) 
{
    RGB oldpale[256];
	Common::Bitmap *newbg = CreateBlockFromBitmap(bmp, oldpale, true, false, NULL);
	RoomStruct *theRoom = (RoomStruct*)(void*)room->_roomStructPtr;
	theRoom->Width = room->Width;
	theRoom->Height = room->Height;
    theRoom->MaskResolution = room->MaskResolution;

	if (newbg->GetColorDepth() == 8) 
	{
		for (int aa = 0; aa < 256; aa++) {
		  // make sure it maps to locked cols properly
		  if (thisgame.paluses[aa] == PAL_LOCKED)
			  theRoom->BgFrames[backgroundNumber].Palette[aa] = palette[aa];
		}

		// sharing palette with main background - so copy it across
		if (sharePalette) {
		  memcpy (&theRoom->BgFrames[backgroundNumber].Palette[0], &palette[0], sizeof(RGB) * 256);
		  theRoom->BgFrames[backgroundNumber].IsPaletteShared = 1;
		  if ((size_t)backgroundNumber >= theRoom->BgFrameCount - 1)
		  	theRoom->BgFrames[0].IsPaletteShared = 1;

		  if (!useExactPalette)
			wremapall(oldpale, newbg, palette);
		}
		else {
		  theRoom->BgFrames[backgroundNumber].IsPaletteShared = 0;
		  remap_background (newbg, oldpale, theRoom->BgFrames[backgroundNumber].Palette, useExactPalette);
		}

    copy_room_palette_to_global_palette();
	}

	if ((size_t)backgroundNumber >= theRoom->BgFrameCount)
	{
		theRoom->BgFrameCount++;
	}
	theRoom->BgFrames[backgroundNumber].Graphic.reset(newbg);

  // if size or resolution has changed, reset masks
	if ((newbg->GetWidth() != theRoom->WalkBehindMask->GetWidth()) || (newbg->GetHeight() != theRoom->WalkBehindMask->GetHeight()) ||
      (theRoom->Width != theRoom->WalkBehindMask->GetWidth()) )
	{
        theRoom->WalkAreaMask.reset(new AGSBitmap(theRoom->Width / theRoom->MaskResolution, theRoom->Height / theRoom->MaskResolution, 8));
        theRoom->HotspotMask.reset(new AGSBitmap(theRoom->Width / theRoom->MaskResolution, theRoom->Height / theRoom->MaskResolution, 8));
        theRoom->WalkBehindMask.reset(new AGSBitmap(theRoom->Width, theRoom->Height, 8));
        theRoom->RegionMask.reset(new AGSBitmap(theRoom->Width / theRoom->MaskResolution, theRoom->Height / theRoom->MaskResolution, 8));
        theRoom->WalkAreaMask->Clear();
        theRoom->HotspotMask->Clear();
        theRoom->WalkBehindMask->Clear();
        theRoom->RegionMask->Clear();
	}
}

void set_area_mask(void* roomptr, int maskType, SysBitmap^ bmp)
{
    RGB oldpale[256];
    RoomStruct* room = (RoomStruct*)roomptr;
    AGSBitmap* oldMask = room->GetMask((RoomAreaMask)maskType);
    AGSBitmap* newMask = CreateBlockFromBitmap(bmp, oldpale, false, false, NULL);
    if (maskType != kRoomAreaNone)
        validate_mask(newMask, "imported", (maskType == kRoomAreaHotspot) ? MAX_ROOM_HOTSPOTS : (MAX_WALK_AREAS + 1));

    switch (maskType)
    {
    case kRoomAreaHotspot:
        room->HotspotMask = PBitmap(newMask);
        break;
    case kRoomAreaRegion:
        room->RegionMask = PBitmap(newMask);
        break;
    case kRoomAreaWalkable:
        room->WalkAreaMask = PBitmap(newMask);
        break;
    case kRoomAreaWalkBehind:
        room->WalkBehindMask = PBitmap(newMask);
        break;
    default:
        return;
    }
}

SysBitmap ^export_area_mask(void *roomptr, int maskType)
{
    AGSBitmap *mask = ((RoomStruct*)roomptr)->GetMask((RoomAreaMask)maskType);
    return ConvertBlockToBitmap(mask, false);
}

void set_rgb_mask_from_alpha_channel(Common::Bitmap *image)
{
	for (int y = 0; y < image->GetHeight(); y++)
	{
		unsigned long* thisLine = (unsigned long*)image->GetScanLine(y);
		for (int x = 0; x < image->GetWidth(); x++)
		{
			if ((thisLine[x] & 0xff000000) == 0)
			{
				thisLine[x] = MASK_COLOR_32;
			}
		}
	}
}

void set_opaque_alpha_channel(Common::Bitmap *image)
{
	for (int y = 0; y < image->GetHeight(); y++)
	{
		unsigned long* thisLine = (unsigned long*)image->GetScanLine(y);
		for (int x = 0; x < image->GetWidth(); x++)
		{
			if (thisLine[x] != MASK_COLOR_32)
			  thisLine[x] |= 0xff000000;
		}
	}
}

AGS::Types::SpriteImportResolution SetNewSpriteFromBitmap(int slot, System::Drawing::Bitmap^ bmp, int spriteImportMethod, bool remapColours, bool useRoomBackgroundColours, bool alphaChannel)
{
    RGB imgPalBuf[256];
  int importedColourDepth;
	Common::Bitmap *tempsprite = CreateBlockFromBitmap(bmp, imgPalBuf, true, (spriteImportMethod != SIMP_NONE), &importedColourDepth);

	if (thisgame.color_depth > 1) 
	{
		sort_out_transparency(tempsprite, spriteImportMethod, imgPalBuf, useRoomBackgroundColours, importedColourDepth);
	}
    else
    {
        int transcol;
        sort_out_transparency(tempsprite, spriteImportMethod, imgPalBuf, importedColourDepth, transcol);
        if (remapColours)
            sort_out_palette(tempsprite, imgPalBuf, useRoomBackgroundColours, transcol);
    }

    int flags = 0;
	if (alphaChannel)
	{
        flags |= SPF_ALPHACHANNEL;
		if (tempsprite->GetColorDepth() == 32)
		{
			set_rgb_mask_from_alpha_channel(tempsprite);
		}
	}
	else if (tempsprite->GetColorDepth() == 32)
	{
		set_opaque_alpha_channel(tempsprite);
	}
    thisgame.SpriteInfos[slot].Flags = flags;

	SetNewSprite(slot, tempsprite);

	return AGS::Types::SpriteImportResolution::Real;
}

void SetBitmapPaletteFromGlobalPalette(System::Drawing::Bitmap ^bmp)
{
	ColorPalette ^colorPal = bmp->Palette;
	cli::array<System::Drawing::Color> ^bmpPalette = colorPal->Entries;
	for (int i = 0; i < 256; i++) 
	{
		bmpPalette[i] = Color::FromArgb((i == 0) ? i : 255, palette[i].r * 4, palette[i].g * 4, palette[i].b * 4);
	}

	// Need to set this back to make it pick up the changes
	bmp->Palette = colorPal;
	//bmp->MakeTransparent(bmpPalette[0]);
}

System::Drawing::Bitmap^ ConvertBlockToBitmap(Common::Bitmap *todraw, bool useAlphaChannel) 
{
  fix_block(todraw);

  PixelFormat pixFormat = PixelFormat::Format32bppRgb;
  if ((todraw->GetColorDepth() == 32) && (useAlphaChannel))
	  pixFormat = PixelFormat::Format32bppArgb;
  else if (todraw->GetColorDepth() == 24)
    pixFormat = PixelFormat::Format24bppRgb;
  else if (todraw->GetColorDepth() == 16)
    pixFormat = PixelFormat::Format16bppRgb565;
  else if (todraw->GetColorDepth() == 15)
    pixFormat = PixelFormat::Format16bppRgb555;
  else if (todraw->GetColorDepth() == 8)
    pixFormat = PixelFormat::Format8bppIndexed;
  int bytesPerPixel = (todraw->GetColorDepth() + 1) / 8;

  System::Drawing::Bitmap ^bmp = gcnew System::Drawing::Bitmap(todraw->GetWidth(), todraw->GetHeight(), pixFormat);
  System::Drawing::Rectangle rect(0, 0, bmp->Width, bmp->Height);
  BitmapData ^bmpData = bmp->LockBits(rect, ImageLockMode::WriteOnly, bmp->PixelFormat);
  unsigned char *address = (unsigned char*)bmpData->Scan0.ToPointer();
  for (int y = 0; y < todraw->GetHeight(); y++) {
    memcpy(address, &todraw->GetScanLine(y)[0], bmp->Width * bytesPerPixel);
    address += bmpData->Stride;
  }
  bmp->UnlockBits(bmpData);

  if (pixFormat == PixelFormat::Format8bppIndexed)
    SetBitmapPaletteFromGlobalPalette(bmp);

  fix_block(todraw);
  return bmp;
}

System::Drawing::Bitmap^ ConvertBlockToBitmap32(Common::Bitmap *todraw, int width, int height, bool useAlphaChannel) 
{
  Common::Bitmap *tempBlock = Common::BitmapHelper::CreateBitmap(todraw->GetWidth(), todraw->GetHeight(), 32);
  if (!tempBlock)
    return nullptr; // out of mem?

  if (todraw->GetColorDepth() == 8)
    select_palette(palette);

  tempBlock->Blit(todraw, 0, 0, 0, 0, todraw->GetWidth(), todraw->GetHeight());

  if (todraw->GetColorDepth() == 8)
	unselect_palette();

  if ((width != todraw->GetWidth()) || (height != todraw->GetHeight())) 
  {
	  Common::Bitmap *newBlock = Common::BitmapHelper::CreateBitmap(width, height, 32);
      if (!newBlock)
      {
          delete tempBlock;
          return nullptr; // out of mem?
      }
	  Cstretch_blit(tempBlock, newBlock, 0, 0, todraw->GetWidth(), todraw->GetHeight(), 0, 0, width, height);
	  delete tempBlock;
	  tempBlock = newBlock;
  }

  fix_block(tempBlock);

  PixelFormat pixFormat = PixelFormat::Format32bppRgb;
  if ((todraw->GetColorDepth() == 32) && (useAlphaChannel))
	  pixFormat = PixelFormat::Format32bppArgb;

  System::Drawing::Bitmap ^bmp = gcnew System::Drawing::Bitmap(width, height, pixFormat);
  if (!bmp)
  {
      delete tempBlock;
      return nullptr; // out of mem?
  }
  System::Drawing::Rectangle rect(0, 0, bmp->Width, bmp->Height);
  BitmapData ^bmpData = bmp->LockBits(rect, ImageLockMode::WriteOnly, bmp->PixelFormat);
  unsigned char *address = (unsigned char*)bmpData->Scan0.ToPointer();
  for (int y = 0; y < tempBlock->GetHeight(); y++) {
    memcpy(address, &tempBlock->GetScanLine(y)[0], bmp->Width * 4);
    address += bmpData->Stride;
  }
  bmp->UnlockBits(bmpData);
  delete tempBlock;
  return bmp;
}

System::Drawing::Bitmap^ ConvertAreaMaskToBitmap(Common::Bitmap *mask) 
{
	System::Drawing::Bitmap ^bmp = gcnew System::Drawing::Bitmap(mask->GetWidth(), mask->GetHeight(), PixelFormat::Format8bppIndexed);
	System::Drawing::Rectangle rect(0, 0, bmp->Width, bmp->Height);
	BitmapData ^bmpData = bmp->LockBits(rect, ImageLockMode::WriteOnly, bmp->PixelFormat);
	unsigned char *address = (unsigned char*)bmpData->Scan0.ToPointer();
	for (int y = 0; y < mask->GetHeight(); y++) 
	{
		memcpy(address, &mask->GetScanLine(y)[0], bmp->Width);
		address += bmpData->Stride;
	}
	bmp->UnlockBits(bmpData);

  SetBitmapPaletteFromGlobalPalette(bmp);

	return bmp;
}

System::Drawing::Bitmap^ getSpriteAsBitmap(int spriteNum) {
  Common::Bitmap *todraw = get_sprite(spriteNum);
  return ConvertBlockToBitmap(todraw, (thisgame.SpriteInfos[spriteNum].Flags & SPF_ALPHACHANNEL) != 0);
}

System::Drawing::Bitmap^ getSpriteAsBitmap32bit(int spriteNum, int width, int height) {
  Common::Bitmap *todraw = get_sprite(spriteNum);
  if (todraw == NULL)
  {
	  throw gcnew AGSEditorException(String::Format("getSpriteAsBitmap32bit: Unable to find sprite {0}", spriteNum));
  }
  return ConvertBlockToBitmap32(todraw, width, height, (thisgame.SpriteInfos[spriteNum].Flags & SPF_ALPHACHANNEL) != 0);
}

System::Drawing::Bitmap^ getBackgroundAsBitmap(Room ^room, int backgroundNumber) {

  RoomStruct *roomptr = (RoomStruct*)(void*)room->_roomStructPtr;
  return ConvertBlockToBitmap32(roomptr->BgFrames[backgroundNumber].Graphic.get(), room->Width, room->Height, false);
}

void PaletteUpdated(cli::array<PaletteEntry^>^ newPalette) 
{  
	for each (PaletteEntry ^colour in newPalette) 
	{
		palette[colour->Index].r = colour->Colour.R / 4;
		palette[colour->Index].g = colour->Colour.G / 4;
		palette[colour->Index].b = colour->Colour.B / 4;
	}
	set_palette(palette);
  copy_global_palette_to_room_palette();
}

void ConvertGUIToBinaryFormat(GUI ^guiObj, GUIMain *gui) 
{
  NormalGUI^ normalGui = dynamic_cast<NormalGUI^>(guiObj);
  if (normalGui)
  {
	gui->OnClickHandler = ConvertStringToNativeString(normalGui->OnClick);
	gui->X = normalGui->Left;
	gui->Y = normalGui->Top;
	gui->Width = normalGui->Width;
	gui->Height = normalGui->Height;
    gui->SetClickable(normalGui->Clickable);
    gui->SetVisible(normalGui->Visible);
    gui->PopupAtMouseY = normalGui->PopupYPos;
    gui->PopupStyle = (Common::GUIPopupStyle)normalGui->PopupStyle;
    gui->ZOrder = normalGui->ZOrder;
    gui->FgColor = normalGui->BorderColor;
    gui->SetTransparencyAsPercentage(normalGui->Transparency);
  }
  else
  {
    TextWindowGUI^ twGui = dynamic_cast<TextWindowGUI^>(guiObj);
	gui->Width = twGui->EditorWidth;
	gui->Height = twGui->EditorHeight;
    gui->SetTextWindow(true);
    gui->PopupStyle = Common::kGUIPopupModal;
	gui->Padding = twGui->Padding;
    gui->FgColor = twGui->TextColor;
  }
  gui->BgColor = guiObj->BackgroundColor;
  gui->BgImage = guiObj->BackgroundImage;
  
  gui->Name = ConvertStringToNativeString(guiObj->Name);

  gui->RemoveAllControls();

  for each (GUIControl^ control in guiObj->Controls)
  {
	  AGS::Types::GUIButton^ button = dynamic_cast<AGS::Types::GUIButton^>(control);
	  AGS::Types::GUILabel^ label = dynamic_cast<AGS::Types::GUILabel^>(control);
	  AGS::Types::GUITextBox^ textbox = dynamic_cast<AGS::Types::GUITextBox^>(control);
	  AGS::Types::GUIListBox^ listbox = dynamic_cast<AGS::Types::GUIListBox^>(control);
	  AGS::Types::GUISlider^ slider = dynamic_cast<AGS::Types::GUISlider^>(control);
	  AGS::Types::GUIInventory^ invwindow = dynamic_cast<AGS::Types::GUIInventory^>(control);
	  AGS::Types::GUITextWindowEdge^ textwindowedge = dynamic_cast<AGS::Types::GUITextWindowEdge^>(control);
	  if (button)
	  {
          guibuts.push_back(Common::GUIButton());
		  guibuts[numguibuts].TextColor = button->TextColor;
		  guibuts[numguibuts].Font = button->Font;
		  guibuts[numguibuts].Image = button->Image;
		  guibuts[numguibuts].CurrentImage = guibuts[numguibuts].Image;
		  guibuts[numguibuts].MouseOverImage = button->MouseoverImage;
		  guibuts[numguibuts].PushedImage = button->PushedImage;
		  guibuts[numguibuts].TextAlignment = (::FrameAlignment)button->TextAlignment;
          guibuts[numguibuts].ClickAction[Common::kMouseLeft] = (Common::GUIClickAction)button->ClickAction;
		  guibuts[numguibuts].ClickData[Common::kMouseLeft] = button->NewModeNumber;
          guibuts[numguibuts].SetClipImage(button->ClipImage);
          guibuts[numguibuts].SetText(ConvertStringToNativeString(button->Text));
          guibuts[numguibuts].EventHandlers[0] = ConvertStringToNativeString(button->OnClick);
		  
          gui->AddControl(Common::kGUIButton, numguibuts, &guibuts[numguibuts]);
		  numguibuts++;
	  }
	  else if (label)
	  {
          guilabels.push_back(Common::GUILabel());
		  guilabels[numguilabels].TextColor = label->TextColor;
		  guilabels[numguilabels].Font = label->Font;
		  guilabels[numguilabels].TextAlignment = (::HorAlignment)label->TextAlignment;
          Common::String text = ConvertStringToNativeString(label->Text);
		  guilabels[numguilabels].SetText(text);

          gui->AddControl(Common::kGUILabel, numguilabels, &guilabels[numguilabels]);
		  numguilabels++;
	  }
	  else if (textbox)
	  {
          guitext.push_back(Common::GUITextBox());
		  guitext[numguitext].TextColor = textbox->TextColor;
		  guitext[numguitext].Font = textbox->Font;
          guitext[numguitext].SetShowBorder(textbox->ShowBorder);
          guitext[numguitext].EventHandlers[0] = ConvertStringToNativeString(textbox->OnActivate);

          gui->AddControl(Common::kGUITextBox, numguitext, &guitext[numguitext]);
		  numguitext++;
	  }
	  else if (listbox)
	  {
          guilist.push_back(Common::GUIListBox());
		  guilist[numguilist].TextColor = listbox->TextColor;
		  guilist[numguilist].Font = listbox->Font;
		  guilist[numguilist].SelectedTextColor = listbox->SelectedTextColor;
		  guilist[numguilist].SelectedBgColor = listbox->SelectedBackgroundColor;
		  guilist[numguilist].TextAlignment = (::HorAlignment)listbox->TextAlignment;
          guilist[numguilist].SetTranslated(listbox->Translated);
          guilist[numguilist].SetShowBorder(listbox->ShowBorder);
		  guilist[numguilist].SetShowArrows(listbox->ShowScrollArrows);
          guilist[numguilist].EventHandlers[0] = ConvertStringToNativeString(listbox->OnSelectionChanged);

          gui->AddControl(Common::kGUIListBox, numguilist, &guilist[numguilist]);
		  numguilist++;
	  }
	  else if (slider)
	  {
          guislider.push_back(Common::GUISlider());
		  guislider[numguislider].MinValue = slider->MinValue;
		  guislider[numguislider].MaxValue = slider->MaxValue;
		  guislider[numguislider].Value = slider->Value;
		  guislider[numguislider].HandleImage = slider->HandleImage;
		  guislider[numguislider].HandleOffset = slider->HandleOffset;
		  guislider[numguislider].BgImage = slider->BackgroundImage;
          guislider[numguislider].EventHandlers[0] = ConvertStringToNativeString(slider->OnChange);

          gui->AddControl(Common::kGUISlider, numguislider, &guislider[numguislider]);
		  numguislider++;
	  }
	  else if (invwindow)
	  {
          guiinv.push_back(Common::GUIInvWindow());
		  guiinv[numguiinv].CharId = invwindow->CharacterID;
		  guiinv[numguiinv].ItemWidth = invwindow->ItemWidth;
		  guiinv[numguiinv].ItemHeight = invwindow->ItemHeight;

          gui->AddControl(Common::kGUIInvWindow, numguiinv, &guiinv[numguiinv]);
		  numguiinv++;
	  }
	  else if (textwindowedge)
	  {
          guibuts.push_back(Common::GUIButton());
		  guibuts[numguibuts].Image = textwindowedge->Image;
		  guibuts[numguibuts].CurrentImage = guibuts[numguibuts].Image;
		  
          gui->AddControl(Common::kGUIButton, numguibuts, &guibuts[numguibuts]);
		  numguibuts++;
	  }

      Common::GUIObject *newObj = gui->GetControl(gui->GetControlCount() - 1);
	  newObj->X = control->Left;
	  newObj->Y = control->Top;
	  newObj->Width = control->Width;
	  newObj->Height = control->Height;
	  newObj->Id = control->ID;
	  newObj->ZOrder = control->ZOrder;
      newObj->Name = ConvertStringToNativeString(control->Name);
  }

  gui->RebuildArray();
  gui->ResortZOrder();
}

void drawGUI(int hdc, int x,int y, GUI^ guiObj, int resolutionFactor, float scale, int selectedControl) {
  numguibuts = 0;
  numguilabels = 0;
  numguitext = 0;
  numguilist = 0;
  numguislider = 0;
  numguiinv = 0;
  guibuts.resize(0);
  guilabels.resize(0);
  guitext.resize(0);
  guilist.resize(0);
  guislider.resize(0);
  guiinv.resize(0);

  ConvertGUIToBinaryFormat(guiObj, &tempgui);

  tempgui.HighlightCtrl = selectedControl;

  drawGUIAt(hdc, x, y, -1, -1, -1, -1, resolutionFactor, scale);
}

Dictionary<int, Sprite^>^ load_sprite_dimensions()
{
	Dictionary<int, Sprite^>^ sprites = gcnew Dictionary<int, Sprite^>();

	for (int i = 0; i < spriteset.GetSpriteSlotCount(); i++)
	{
		Common::Bitmap *spr = spriteset[i];
		if (spr != NULL)
		{
			sprites->Add(i, gcnew Sprite(i, spr->GetWidth(), spr->GetHeight(), spr->GetColorDepth(),
                (thisgame.SpriteInfos[i].Flags & SPF_ALPHACHANNEL) ? true : false));
		}
	}

	return sprites;
}

void ConvertCustomProperties(AGS::Types::CustomProperties ^insertInto, AGS::Common::StringIMap *propToConvert)
{
    for (AGS::Common::StringIMap::const_iterator it = propToConvert->begin();
         it != propToConvert->end(); ++it)
	{
		CustomProperty ^newProp = gcnew CustomProperty();
		newProp->Name = ToStr(it->first);
		newProp->Value = ToStr(it->second);
		insertInto->PropertyValues->Add(newProp->Name, newProp);
	}
}

void CompileCustomProperties(AGS::Types::CustomProperties ^convertFrom, AGS::Common::StringIMap *compileInto)
{
	compileInto->clear();
	for each (String ^key in convertFrom->PropertyValues->Keys)
	{
        AGS::Common::String name, value;
		name = ConvertStringToNativeString(convertFrom->PropertyValues[key]->Name);
		value = ConvertStringToNativeString(convertFrom->PropertyValues[key]->Value);
		(*compileInto)[name] = value;
	}
}

char charScriptNameBuf[100];
const char *GetCharacterScriptName(int charid, AGS::Types::Game ^game) 
{
	if ((charid >= 0) && (game != nullptr) &&
    (charid < game->Characters->Count) &&
		(game->Characters[charid]->ScriptName->Length > 0))
	{
		ConvertStringToCharArray(game->Characters[charid]->ScriptName,  charScriptNameBuf, 100); 
	}
	else 
	{
		sprintf(charScriptNameBuf, "character[%d]", charid);
	}
	return charScriptNameBuf;
}

void CopyInteractions(AGS::Types::Interactions ^destination, AGS::Common::InteractionScripts *source)
{
    if (source->ScriptFuncNames.size() > (size_t)destination->ScriptFunctionNames->Length) 
	{
		throw gcnew AGS::Types::AGSEditorException("Invalid interaction funcs: too many interaction events");
	}

	for (size_t i = 0; i < source->ScriptFuncNames.size(); i++) 
	{
		destination->ScriptFunctionNames[i] = ToStr(source->ScriptFuncNames[i]);
	}
}

// Load compiled game's main data file and use it to create AGS::Types::Game.
// TODO: originally this function was meant import strictly 2.72 games;
// although technically it can now load game files of any version, more work is
// required to properly fill in editor's Game object's fields depending on
// which version is being loaded.
Game^ import_compiled_game_dta(const AGSString &filename)
{
	HAGSError err = load_dta_file_into_thisgame(filename);
    loaded_game_file_version = kGameVersion_Current;
	if (!err)
	{
		throw gcnew AGS::Types::AGSEditorException(ToStr(err->FullMessage()));
	}

	Game^ game = gcnew Game();
    game->Settings->AlwaysDisplayTextAsSpeech = (thisgame.options[OPT_ALWAYSSPCH] != 0);
	game->Settings->AntiAliasFonts = (thisgame.options[OPT_ANTIALIASFONTS] != 0);
	game->Settings->AntiGlideMode = (thisgame.options[OPT_ANTIGLIDE] != 0);
	game->Settings->AutoMoveInWalkMode = !thisgame.options[OPT_NOWALKMODE];
	game->Settings->BackwardsText = (thisgame.options[OPT_RIGHTLEFTWRITE] != 0);
	game->Settings->ColorDepth = (GameColorDepth)thisgame.color_depth;
	game->Settings->CompressSprites = (thisgame.options[OPT_COMPRESSSPRITES] != 0);
	game->Settings->CrossfadeMusic = (CrossfadeSpeed)thisgame.options[OPT_CROSSFADEMUSIC];
	game->Settings->DebugMode = (thisgame.options[OPT_DEBUGMODE] != 0);
	game->Settings->DialogOptionsBackwards = (thisgame.options[OPT_DIALOGUPWARDS] != 0);
	game->Settings->DialogOptionsGap = thisgame.options[OPT_DIALOGGAP];
	game->Settings->DialogOptionsGUI = thisgame.options[OPT_DIALOGIFACE];
	game->Settings->DialogOptionsBullet = thisgame.dialog_bullet;
	game->Settings->DisplayMultipleInventory = (thisgame.options[OPT_DUPLICATEINV] != 0);
    game->Settings->EnforceNewAudio = false;
	game->Settings->GameName = gcnew String(thisgame.gamename);
	game->Settings->UseGlobalSpeechAnimationDelay = true; // this was always on in pre-3.0 games
	game->Settings->HandleInvClicksInScript = (thisgame.options[OPT_HANDLEINVCLICKS] != 0);
	game->Settings->InventoryCursors = !thisgame.options[OPT_FIXEDINVCURSOR];
	game->Settings->MaximumScore = thisgame.totalscore;
    game->Settings->NumberDialogOptions = (thisgame.options[OPT_DIALOGNUMBERED] != 0) ? DialogOptionsNumbering::Normal : DialogOptionsNumbering::KeyShortcutsOnly;
	game->Settings->PixelPerfect = (thisgame.options[OPT_PIXPERFECT] != 0);
	game->Settings->PlaySoundOnScore = thisgame.options[OPT_SCORESOUND];
	game->Settings->Resolution = (GameResolutions)thisgame.GetResolutionType();
	game->Settings->RoomTransition = (RoomTransitionStyle)thisgame.options[OPT_FADETYPE];
	game->Settings->SaveScreenshots = (thisgame.options[OPT_SAVESCREENSHOT] != 0);
	game->Settings->SkipSpeech = (SkipSpeechStyle)thisgame.options[OPT_NOSKIPTEXT];
	game->Settings->SpeechPortraitSide = (SpeechPortraitSide)thisgame.options[OPT_PORTRAITSIDE];
	game->Settings->SpeechStyle = (SpeechStyle)thisgame.options[OPT_SPEECHTYPE];
	game->Settings->SplitResources = thisgame.options[OPT_SPLITRESOURCES];
	game->Settings->TextWindowGUI = thisgame.options[OPT_TWCUSTOM];
	game->Settings->ThoughtGUI = thisgame.options[OPT_THOUGHTGUI];
	game->Settings->TurnBeforeFacing = (thisgame.options[OPT_TURNTOFACELOC] != 0);
	game->Settings->TurnBeforeWalking = (thisgame.options[OPT_ROTATECHARS] != 0);
	game->Settings->WalkInLookMode = (thisgame.options[OPT_WALKONLOOK] != 0);
	game->Settings->WhenInterfaceDisabled = (InterfaceDisabledAction)thisgame.options[OPT_DISABLEOFF];
	game->Settings->UniqueID = thisgame.uniqueid;
    game->Settings->SaveGameFolderName = gcnew String(thisgame.gamename);
    game->Settings->RenderAtScreenResolution = (RenderAtScreenResolution)thisgame.options[OPT_RENDERATSCREENRES];

	game->Settings->InventoryHotspotMarker->DotColor = thisgame.hotdot;
	game->Settings->InventoryHotspotMarker->CrosshairColor = thisgame.hotdotouter;
	game->Settings->InventoryHotspotMarker->Image = thisgame.invhotdotsprite;
	if (thisgame.invhotdotsprite) 
	{
		game->Settings->InventoryHotspotMarker->Style = InventoryHotspotMarkerStyle::Sprite;
	}
	else if (thisgame.hotdot) 
	{
		game->Settings->InventoryHotspotMarker->Style = InventoryHotspotMarkerStyle::Crosshair;
	}
	else 
	{
		game->Settings->InventoryHotspotMarker->Style = InventoryHotspotMarkerStyle::None;
	}

	int i;
	for (i = 0; i < 256; i++) 
	{
		if (thisgame.paluses[i] == PAL_BACKGROUND) 
		{
			game->Palette[i]->ColourType = PaletteColourType::Background;
		}
		else 
		{
			game->Palette[i]->ColourType = PaletteColourType::Gamewide; 
		}
		game->Palette[i]->Colour = Color::FromArgb(palette[i].r * 4, palette[i].g * 4, palette[i].b * 4);
	}

	for (i = 0; i < numThisgamePlugins; i++) 
	{
		cli::array<System::Byte> ^pluginData = gcnew cli::array<System::Byte>(thisgamePlugins[i].DataLen);
		const char *data_ptr = thisgamePlugins[i].Data.get();
		for (size_t j = 0; j < thisgamePlugins[i].DataLen; j++) 
		{
			pluginData[j] = data_ptr[j];
		}
		
		AGS::Types::Plugin ^plugin = gcnew AGS::Types::Plugin(ToStr(thisgamePlugins[i].Name), pluginData);
		game->Plugins->Add(plugin);
	}

    /* CLNUP -- remove OldInteractionVariable?
	for (i = 0; i < numGlobalVars; i++)
	{
		OldInteractionVariable ^intVar;
		intVar = gcnew OldInteractionVariable(ToStr(globalvars[i].Name), globalvars[i].Value);
		game->OldInteractionVariables->Add(intVar);
	}
    */
	
    AGS::Types::IViewFolder ^viewFolder = AGS::Types::FolderHelper::CreateDefaultViewFolder();
	for (i = 0; i < thisgame.numviews; i++) 
	{
		AGS::Types::View ^view = gcnew AGS::Types::View();
		view->Name = ToStr(thisgame.viewNames[i]);
		view->ID = i + 1;

		for (int j = 0; j < newViews[i].numLoops; j++) 
		{
			ViewLoop^ newLoop = gcnew ViewLoop();
			newLoop->ID = j;
			newLoop->RunNextLoop = newViews[i].loops[j].flags & LOOPFLAG_RUNNEXTLOOP;

			for (int k = 0; k < newViews[i].loops[j].numFrames; k++) 
			{
				AGS::Types::ViewFrame^ newFrame = gcnew AGS::Types::ViewFrame();
				newFrame->ID = k;
				newFrame->Flipped = (newViews[i].loops[j].frames[k].flags & VFLG_FLIPSPRITE);
				newFrame->Image = newViews[i].loops[j].frames[k].pic;
				newFrame->Sound = newViews[i].loops[j].frames[k].sound;
				newFrame->Delay = newViews[i].loops[j].frames[k].speed;
				newLoop->Frames->Add(newFrame);
			}
			
			view->Loops->Add(newLoop);
		}

		viewFolder->Views->Add(view);
	}
    AGS::Types::FolderHelper::SetRootViewFolder(game, viewFolder);

	for (i = 0; i < thisgame.numcharacters; i++) 
	{
		AGS::Types::Character ^character = gcnew AGS::Types::Character();
		character->AdjustSpeedWithScaling = ((thisgame.chars[i].flags & CHF_SCALEMOVESPEED) != 0);
		character->AdjustVolumeWithScaling = ((thisgame.chars[i].flags & CHF_SCALEVOLUME) != 0);
		character->AnimationDelay = thisgame.chars[i].animspeed;
		character->BlinkingView = (thisgame.chars[i].blinkview < 1) ? 0 : (thisgame.chars[i].blinkview + 1);
		character->Clickable = !(thisgame.chars[i].flags & CHF_NOINTERACT);
		character->DiagonalLoops = !(thisgame.chars[i].flags & CHF_NODIAGONAL);
		character->ID = i;
		character->IdleView = (thisgame.chars[i].idleview < 1) ? 0 : (thisgame.chars[i].idleview + 1);
		character->MovementSpeed = thisgame.chars[i].walkspeed;
		character->MovementSpeedX = thisgame.chars[i].walkspeed;
		character->MovementSpeedY = thisgame.chars[i].walkspeed_y;
		character->NormalView = thisgame.chars[i].defview + 1;
		character->RealName = gcnew String(thisgame.chars[i].name);
		character->ScriptName = gcnew String(thisgame.chars[i].scrname);
		character->Solid = !(thisgame.chars[i].flags & CHF_NOBLOCKING);
		character->SpeechColor = thisgame.chars[i].talkcolor;
		character->SpeechView = (thisgame.chars[i].talkview < 1) ? 0 : (thisgame.chars[i].talkview + 1);
		character->StartingRoom = thisgame.chars[i].room;
		character->StartX = thisgame.chars[i].x;
		character->StartY = thisgame.chars[i].y;
    character->ThinkingView = (thisgame.chars[i].thinkview < 1) ? 0 : (thisgame.chars[i].thinkview + 1);
		character->TurnBeforeWalking = !(thisgame.chars[i].flags & CHF_NOTURNING);
		character->UniformMovementSpeed = (thisgame.chars[i].walkspeed_y == UNIFORM_WALK_SPEED);
		character->UseRoomAreaLighting = !(thisgame.chars[i].flags & CHF_NOLIGHTING);
		character->UseRoomAreaScaling = !(thisgame.chars[i].flags & CHF_MANUALSCALING);

		game->Characters->Add(character);

		ConvertCustomProperties(character->Properties, &thisgame.charProps[i]);
	}
	game->PlayerCharacter = game->Characters[thisgame.playercharacter];

	game->TextParser->Words->Clear();
	for (i = 0; i < thisgame.dict->num_words; i++) 
	{
		AGS::Types::TextParserWord ^newWord = gcnew AGS::Types::TextParserWord();
		newWord->WordGroup = thisgame.dict->wordnum[i];
		newWord->Word = gcnew String(thisgame.dict->word[i]);
		newWord->SetWordTypeFromGroup();

		game->TextParser->Words->Add(newWord);
	}

	for (i = 0; i < MAXGLOBALMES; i++) 
	{
		if (thisgame.messages[i] != NULL) 
		{
			game->GlobalMessages[i] = gcnew String(thisgame.messages[i]);
		}
		else
		{
			game->GlobalMessages[i] = String::Empty;
		}
	}

	game->LipSync->Type = (thisgame.options[OPT_LIPSYNCTEXT] != 0) ? LipSyncType::Text : LipSyncType::None;
	game->LipSync->DefaultFrame = thisgame.default_lipsync_frame;
	for (i = 0; i < MAXLIPSYNCFRAMES; i++) 
	{
		game->LipSync->CharactersPerFrame[i] = gcnew String(thisgame.lipSyncFrameLetters[i]);
	}

	for (i = 0; i < thisgame.numdialog; i++) 
	{
		AGS::Types::Dialog ^newDialog = gcnew AGS::Types::Dialog();
		newDialog->ID = i;
		for (int j = 0; j < dialog[i].numoptions; j++) 
		{
			AGS::Types::DialogOption ^newOption = gcnew AGS::Types::DialogOption();
			newOption->ID = j + 1;
			newOption->Text = gcnew String(dialog[i].optionnames[j]);
			newOption->Say = !(dialog[i].optionflags[j] & DFLG_NOREPEAT);
			newOption->Show = (dialog[i].optionflags[j] & DFLG_ON);

			newDialog->Options->Add(newOption);
		}

		newDialog->Name = ToStr(thisgame.dialogScriptNames[i]);
		newDialog->ShowTextParser = (dialog[i].topicFlags & DTFLG_SHOWPARSER);

		game->Dialogs->Add(newDialog);
	}

	for (i = 0; i < thisgame.numcursors; i++)
	{
		AGS::Types::MouseCursor ^cursor = gcnew AGS::Types::MouseCursor();
		cursor->Animate = (thisgame.mcurs[i].view >= 0);
		cursor->AnimateOnlyOnHotspots = ((thisgame.mcurs[i].flags & MCF_HOTSPOT) != 0);
		cursor->AnimateOnlyWhenMoving = ((thisgame.mcurs[i].flags & MCF_ANIMMOVE) != 0);
		cursor->Image = thisgame.mcurs[i].pic;
		cursor->HotspotX = thisgame.mcurs[i].hotx;
		cursor->HotspotY = thisgame.mcurs[i].hoty;
		cursor->ID = i;
		cursor->Name = gcnew String(thisgame.mcurs[i].name);
		cursor->StandardMode = ((thisgame.mcurs[i].flags & MCF_STANDARD) != 0);
		cursor->View = thisgame.mcurs[i].view + 1;
		if (cursor->View < 1) cursor->View = 0;

		game->Cursors->Add(cursor);
	}

	for (i = 0; i < thisgame.numfonts; i++) 
	{
		AGS::Types::Font ^font = gcnew AGS::Types::Font();
		font->ID = i;
		font->OutlineFont = (thisgame.fonts[i].Outline >= 0) ? thisgame.fonts[i].Outline : 0;
		if (thisgame.fonts[i].Outline == -1)
		{
			font->OutlineStyle = FontOutlineStyle::None;
		}
		else if (thisgame.fonts[i].Outline == FONT_OUTLINE_AUTO)
		{
			font->OutlineStyle = FontOutlineStyle::Automatic;
		}
		else 
		{
			font->OutlineStyle = FontOutlineStyle::UseOutlineFont;
		}
		font->PointSize = thisgame.fonts[i].SizePt;
		font->Name = gcnew String(String::Format("Font {0}", i));

		game->Fonts->Add(font);
	}

	for (i = 1; i < thisgame.numinvitems; i++)
	{
		InventoryItem^ invItem = gcnew InventoryItem();
    invItem->CursorImage = thisgame.invinfo[i].pic;
		invItem->Description = gcnew String(thisgame.invinfo[i].name);
		invItem->Image = thisgame.invinfo[i].pic;
		invItem->HotspotX = thisgame.invinfo[i].hotx;
		invItem->HotspotY = thisgame.invinfo[i].hoty;
		invItem->ID = i;
		invItem->Name = ToStr(thisgame.invScriptNames[i]);
		invItem->PlayerStartsWithItem = (thisgame.invinfo[i].flags & IFLG_STARTWITH);

		ConvertCustomProperties(invItem->Properties, &thisgame.invProps[i]);

		game->InventoryItems->Add(invItem);
	}

    for (AGS::Common::PropertySchema::const_iterator it = thisgame.propSchema.begin();
         it != thisgame.propSchema.end(); ++it)
	{
		CustomPropertySchemaItem ^schemaItem = gcnew CustomPropertySchemaItem();
		schemaItem->Name = ToStr(it->second.Name);
		schemaItem->Description = ToStr(it->second.Description);
		schemaItem->DefaultValue = ToStr(it->second.DefaultValue);
		schemaItem->Type = (AGS::Types::CustomPropertyType)it->second.Type;

		game->PropertySchema->PropertyDefinitions->Add(schemaItem);
	}

	for (i = 0; i < thisgame.numgui; i++)
	{
		guis[i].RebuildArray();
	    guis[i].ResortZOrder();

		GUI^ newGui;
		if (guis[i].IsTextWindow()) 
		{
			newGui = gcnew TextWindowGUI();
			newGui->Controls->Clear();  // we'll add our own edges
      ((TextWindowGUI^)newGui)->TextColor = guis[i].FgColor;
		}
		else 
		{
			newGui = gcnew NormalGUI(1, 1);
            ((NormalGUI^)newGui)->Clickable = guis[i].IsClickable();
            ((NormalGUI^)newGui)->Visible = guis[i].IsVisible();
			((NormalGUI^)newGui)->Top = guis[i].Y;
			((NormalGUI^)newGui)->Left = guis[i].X;
			((NormalGUI^)newGui)->Width = (guis[i].Width > 0) ? guis[i].Width : 1;
			((NormalGUI^)newGui)->Height = (guis[i].Height > 0) ? guis[i].Height : 1;
			((NormalGUI^)newGui)->PopupYPos = guis[i].PopupAtMouseY;
			((NormalGUI^)newGui)->PopupStyle = (GUIPopupStyle)guis[i].PopupStyle;
			((NormalGUI^)newGui)->ZOrder = guis[i].ZOrder;
			((NormalGUI^)newGui)->OnClick = ToStr(guis[i].OnClickHandler);
      ((NormalGUI^)newGui)->BorderColor = guis[i].FgColor;
		}
		newGui->BackgroundColor = guis[i].BgColor;
		newGui->BackgroundImage = guis[i].BgImage;
		newGui->ID = i;
		newGui->Name = ToStr(guis[i].Name);

		for (int j = 0; j < guis[i].GetControlCount(); j++)
		{
            Common::GUIObject* curObj = guis[i].GetControl(j);
			GUIControl ^newControl = nullptr;
            Common::GUIControlType ctrl_type = guis[i].GetControlType(j);
			switch (ctrl_type)
			{
			case Common::kGUIButton:
				{
				if (guis[i].IsTextWindow())
				{
					AGS::Types::GUITextWindowEdge^ edge = gcnew AGS::Types::GUITextWindowEdge();
					Common::GUIButton *copyFrom = (Common::GUIButton*)curObj;
					newControl = edge;
					edge->Image = copyFrom->Image;
				}
				else
				{
					AGS::Types::GUIButton^ newButton = gcnew AGS::Types::GUIButton();
					Common::GUIButton *copyFrom = (Common::GUIButton*)curObj;
					newControl = newButton;
					newButton->TextColor = copyFrom->TextColor;
					newButton->Font = copyFrom->Font;
					newButton->Image = copyFrom->Image;
					newButton->MouseoverImage = copyFrom->MouseOverImage;
					newButton->PushedImage = copyFrom->PushedImage;
					newButton->TextAlignment = (AGS::Types::FrameAlignment)copyFrom->TextAlignment;
                    newButton->ClickAction = (GUIClickAction)copyFrom->ClickAction[Common::kMouseLeft];
					newButton->NewModeNumber = copyFrom->ClickData[Common::kMouseLeft];
                    newButton->ClipImage = copyFrom->IsClippingImage();
					newButton->Text = ToStr(copyFrom->GetText());
					newButton->OnClick = ToStr(copyFrom->EventHandlers[0]);
				}
				break;
				}
			case Common::kGUILabel:
				{
				AGS::Types::GUILabel^ newLabel = gcnew AGS::Types::GUILabel();
				Common::GUILabel *copyFrom = (Common::GUILabel*)curObj;
				newControl = newLabel;
				newLabel->TextColor = copyFrom->TextColor;
				newLabel->Font = copyFrom->Font;
				newLabel->TextAlignment = (AGS::Types::HorizontalAlignment)copyFrom->TextAlignment;
				newLabel->Text = ToStr(copyFrom->GetText());
				break;
				}
			case Common::kGUITextBox:
				{
				  AGS::Types::GUITextBox^ newTextbox = gcnew AGS::Types::GUITextBox();
				  Common::GUITextBox *copyFrom = (Common::GUITextBox*)curObj;
				  newControl = newTextbox;
				  newTextbox->TextColor = copyFrom->TextColor;
				  newTextbox->Font = copyFrom->Font;
                  newTextbox->ShowBorder = copyFrom->IsBorderShown();
				  newTextbox->Text = ToStr(copyFrom->Text);
				  newTextbox->OnActivate = ToStr(copyFrom->EventHandlers[0]);
				  break;
				}
			case Common::kGUIListBox:
				{
				  AGS::Types::GUIListBox^ newListbox = gcnew AGS::Types::GUIListBox();
				  Common::GUIListBox *copyFrom = (Common::GUIListBox*)curObj;
				  newControl = newListbox;
				  newListbox->TextColor = copyFrom->TextColor;
				  newListbox->Font = copyFrom->Font; 
				  newListbox->SelectedTextColor = copyFrom->SelectedTextColor;
				  newListbox->SelectedBackgroundColor = copyFrom->SelectedBgColor;
				  newListbox->TextAlignment = (AGS::Types::HorizontalAlignment)copyFrom->TextAlignment;
				  newListbox->ShowBorder = copyFrom->IsBorderShown();
				  newListbox->ShowScrollArrows = copyFrom->AreArrowsShown();
                  newListbox->Translated = copyFrom->IsTranslated();
				  newListbox->OnSelectionChanged = ToStr(copyFrom->EventHandlers[0]);
				  break;
				}
			case Common::kGUISlider:
				{
				  AGS::Types::GUISlider^ newSlider = gcnew AGS::Types::GUISlider();
				  Common::GUISlider *copyFrom = (Common::GUISlider*)curObj;
				  newControl = newSlider;
				  newSlider->MinValue = copyFrom->MinValue;
				  newSlider->MaxValue = copyFrom->MaxValue;
				  newSlider->Value = copyFrom->Value;
				  newSlider->HandleImage = copyFrom->HandleImage;
			  	  newSlider->HandleOffset = copyFrom->HandleOffset;
				  newSlider->BackgroundImage = copyFrom->BgImage;
				  newSlider->OnChange = ToStr(copyFrom->EventHandlers[0]);
				  break;
				}
			case Common::kGUIInvWindow:
				{
					AGS::Types::GUIInventory^ invwindow = gcnew AGS::Types::GUIInventory();
				    Common::GUIInvWindow *copyFrom = (Common::GUIInvWindow*)curObj;
				    newControl = invwindow;
					invwindow->CharacterID = copyFrom->CharId;
					invwindow->ItemWidth = copyFrom->ItemWidth;
					invwindow->ItemHeight = copyFrom->ItemHeight;
					break;
				}
			default:
				throw gcnew AGSEditorException("Unknown control type found: " + ((int)ctrl_type).ToString());
			}
			newControl->Width = (curObj->Width > 0) ? curObj->Width : 1;
			newControl->Height = (curObj->Height > 0) ? curObj->Height : 1;
			newControl->Left = curObj->X;
			newControl->Top = curObj->Y;
			newControl->ZOrder = curObj->ZOrder;
            newControl->Clickable = curObj->IsClickable();
            newControl->Enabled = curObj->IsEnabled();
            newControl->Visible = curObj->IsVisible();
			newControl->ID = j;
			newControl->Name = ToStr(curObj->Name);
			newGui->Controls->Add(newControl);
		}
		
		game->GUIs->Add(newGui);
	}

	free_old_game_data();

	return game;
}

System::String ^load_room_script(System::String ^fileName)
{
    AGSString roomFileName = ConvertFileNameToNativeString(fileName);

    AGSString scriptText;
    AGS::Common::RoomDataSource src;
    AGS::Common::HRoomFileError err = OpenRoomFile(roomFileName, src);
    if (err)
    {
        err = AGS::Common::ExtractScriptText(scriptText, src.InputStream.get(), src.DataVersion);
        if (err.HasError() && err->Code() == AGS::Common::kRoomFileErr_BlockNotFound)
            return nullptr; // simply did not find the script text
    }
    if (!err)
        quit(AGSString::FromFormat("Unable to load room script source from '%s', error was:\r\n%s", roomFileName.GetCStr(), err->FullMessage()));

	return gcnew String(scriptText.GetCStr(), 0, scriptText.GetLength(), System::Text::Encoding::Default);;
}

int GetCurrentlyLoadedRoomNumber()
{
  return RoomTools->loaded_room_number;
}

AGS::Types::Room^ load_crm_file(UnloadedRoom ^roomToLoad)
{
    AGSString roomFileName = ConvertFileNameToNativeString(roomToLoad->FileName);

	AGSString errorMsg = load_room_file(roomFileName);
	if (!errorMsg.IsEmpty()) 
	{
		throw gcnew AGSEditorException(ToStr(errorMsg));
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
    room->MaskResolution = thisroom.MaskResolution;

	for (size_t i = 0; i < thisroom.MessageCount; ++i) 
	{
		RoomMessage ^newMessage = gcnew RoomMessage(i);
		newMessage->Text = ToStr(thisroom.Messages[i]);
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
        obj->BlendMode = (BlendMode)thisroom.Objects[i].BlendMode;
		obj->StartX = thisroom.Objects[i].X;
		obj->StartY = thisroom.Objects[i].Y;
		obj->Visible = (thisroom.Objects[i].IsOn != 0);
		obj->Clickable = ((thisroom.Objects[i].Flags & OBJF_NOINTERACT) == 0);
		obj->Baseline = thisroom.Objects[i].Baseline;
		obj->Name = ToStr(thisroom.Objects[i].ScriptName);
		obj->Description = ToStr(thisroom.Objects[i].Name);
		obj->UseRoomAreaScaling = ((thisroom.Objects[i].Flags & OBJF_USEROOMSCALING) != 0);
		obj->UseRoomAreaLighting = ((thisroom.Objects[i].Flags & OBJF_USEREGIONTINTS) != 0);
		ConvertCustomProperties(obj->Properties, &thisroom.Objects[i].Properties);

		CopyInteractions(obj->Interactions, thisroom.Objects[i].EventHandlers.get());

		room->Objects->Add(obj);
	}

	for (size_t i = 0; i < thisroom.HotspotCount; ++i) 
	{
		RoomHotspot ^hotspot = room->Hotspots[i];
		hotspot->ID = i;
		hotspot->Description = ToStr(thisroom.Hotspots[i].Name);
		hotspot->Name = (ToStr(thisroom.Hotspots[i].ScriptName))->Trim();
        hotspot->WalkToPoint = System::Drawing::Point(thisroom.Hotspots[i].WalkTo.X, thisroom.Hotspots[i].WalkTo.Y);
		ConvertCustomProperties(hotspot->Properties, &thisroom.Hotspots[i].Properties);

		CopyInteractions(hotspot->Interactions, thisroom.Hotspots[i].EventHandlers.get());
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

		CopyInteractions(area->Interactions, thisroom.Regions[i].EventHandlers.get());
	}

	room->_roomStructPtr = (IntPtr)&thisroom;

	ConvertCustomProperties(room->Properties, &thisroom.Properties);

	CopyInteractions(room->Interactions, thisroom.EventHandlers.get());

	room->GameID = thisroom.GameID;

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
        thisroom.Objects[i].BlendMode = (AGS::Common::BlendMode)obj->BlendMode;
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


void save_room_file(const AGSString &path)
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

    Stream *out = AGSFile::CreateFile(path);
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


void quit(const char * message) 
{
	ThrowManagedException((const char*)message);
}




void update_polled_stuff_if_runtime()
{
	// do nothing
}

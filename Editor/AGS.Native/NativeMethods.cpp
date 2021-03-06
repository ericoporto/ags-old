/* AGS Native interface to .NET

Adventure Game Studio Editor Source Code
Copyright (c) 2006-2010 Chris Jones
------------------------------------------------------

The AGS Editor Source Code is provided under the Artistic License 2.0,
see the license.txt for details.
*/
#include "agsnative.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include "NativeMethods.h"
#include "NativeUtils.h"
#include "ac/game_version.h"
#include "game/plugininfo.h"
#include "util/error.h"
#include "util/multifilelib.h"

using namespace System::Runtime::InteropServices;
typedef AGS::Common::HError HAGSError;

extern bool initialize_native();
extern void shutdown_native();
extern AGS::Types::Game^ import_compiled_game_dta(const AGSString &filename);
extern void free_old_game_data();
extern AGS::Types::Room^ load_crm_file(UnloadedRoom ^roomToLoad);
extern void save_crm_file(Room ^roomToSave);
extern AGSString import_sci_font(const AGSString &filename, int fslot);
extern bool reload_font(int curFont);
// Draws font char sheet on the provided context and returns the height of drawn object;
// may be called with hdc = 0 to get required height without drawing anything
extern int drawFontAt (int hdc, int fontnum, int x, int y, int width);
extern Dictionary<int, Sprite^>^ load_sprite_dimensions();
extern void drawGUI(int hdc, int x,int y, GUI^ gui, int resolutionFactor, float scale, int selectedControl);
extern void drawSprite(int hdc, int x,int y, int spriteNum, bool flipImage);
extern void drawSpriteStretch(int hdc, int x,int y, int width, int height, int spriteNum, bool flipImage);
extern void drawBlockOfColour(int hdc, int x,int y, int width, int height, int colNum);
extern void drawViewLoop (int hdc, ViewLoop^ loopToDraw, int x, int y, int size, int cursel);
extern void SetNewSpriteFromHBitmap(int slot, int hBmp);
extern AGS::Types::SpriteImportResolution SetNewSpriteFromBitmap(int slot, Bitmap^ bmp, int spriteImportMethod, bool remapColours, bool useRoomBackgroundColours, bool alphaChannel);
extern int GetSpriteAsHBitmap(int spriteSlot);
extern Bitmap^ getSpriteAsBitmap32bit(int spriteNum, int width, int height);
extern Bitmap^ getSpriteAsBitmap(int spriteNum);
extern Bitmap^ getBackgroundAsBitmap(Room ^room, int backgroundNumber);
extern unsigned char* GetRawSpriteData(int spriteSlot);
extern int find_free_sprite_slot();
extern int crop_sprite_edges(int numSprites, int *sprites, bool symmetric);
extern void deleteSprite(int sprslot);
extern void GetSpriteInfo(int slot, ::SpriteInfo &info);
extern int GetSpriteWidth(int slot);
extern int GetSpriteHeight(int slot);
extern int GetSpriteColorDepth(int slot);
extern int GetPaletteAsHPalette();
extern bool DoesSpriteExist(int slot);
extern int GetMaxSprites();
extern int GetCurrentlyLoadedRoomNumber();
extern int load_template_file(const AGSString &fileName, char **iconDataBuffer, long *iconDataSize, bool isRoomTemplate);
extern HAGSError extract_template_files(const AGSString &templateFileName);
extern HAGSError extract_room_template_files(const AGSString &templateFileName, int newRoomNumber);
extern void change_sprite_number(int oldNumber, int newNumber);
extern void update_sprite_resolution(int spriteNum);
extern void SaveGame(bool compressSprites);
extern HAGSError reset_sprite_file();
extern void PaletteUpdated(cli::array<PaletteEntry^>^ newPalette);
extern void GameDirChanged(String ^workingDir);
extern void GameUpdated(Game ^game, bool forceUpdate);
extern void GameFontUpdated(Game ^game, int fontNumber, bool forceUpdate);
extern void UpdateNativeSpritesToGame(Game ^game, List<String^> ^errors);
extern void ImportBackground(Room ^room, int backgroundNumber, Bitmap ^bmp, bool useExactPalette, bool sharePalette);
extern void DeleteBackground(Room ^room, int backgroundNumber);
extern void FixRoomMasks(Room ^room);
extern void set_area_mask(void *roomptr, int maskType, Bitmap ^bmp);
extern Bitmap ^export_area_mask(void *roomptr, int maskType);
extern System::String ^load_room_script(System::String ^fileName);
extern void transform_string(char *text);
extern bool spritesModified;

AGSString editorVersionNumber;

System::String^ ToStr(const AGS::Common::String &str)
{
    return gcnew String(str.GetCStr());
}

AGSString ConvertStringToNativeString(System::String^ clrString)
{
    if (clrString == nullptr)
        return AGSString();
    char* stringPointer = (char*)Marshal::StringToHGlobalAnsi(clrString).ToPointer();
    AGSString str = stringPointer;
    Marshal::FreeHGlobal(IntPtr(stringPointer));
    return str;
}

AGSString ConvertStringToNativeString(System::String^ clrString, size_t buf_len)
{
    if (clrString == nullptr)
        return AGSString();
    char* stringPointer = (char*)Marshal::StringToHGlobalAnsi(clrString).ToPointer();
    AGSString str = stringPointer;
    Marshal::FreeHGlobal(IntPtr(stringPointer));
    return str.Left(buf_len - 1);
}

AGSString ConvertFileNameToNativeString(System::String^ clrString)
{
    if (clrString == nullptr)
        return AGSString();
    AGSString str = ConvertStringToNativeString(clrString);
    if (str.FindChar('?') != -1)
        throw gcnew AGSEditorException(String::Format("Filename contains invalid unicode characters: {0}", clrString));
    return str;
}

void ConvertStringToCharArray(System::String^ clrString, char *buf, size_t buf_len)
{
    char* stringPointer = (char*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(clrString).ToPointer();
    size_t ansi_len = min(strlen(stringPointer) + 1, buf_len);
    memcpy(buf, stringPointer, ansi_len);
    buf[ansi_len - 1] = 0;
    System::Runtime::InteropServices::Marshal::FreeHGlobal(IntPtr(stringPointer));
}

void ConvertFileNameToCharArray(System::String^ clrString, char *buf, size_t buf_len)
{
    ConvertStringToCharArray(clrString, buf, buf_len);
    if (strchr(buf, '?') != NULL)
        throw gcnew AGSEditorException(String::Format("Filename contains invalid unicode characters: {0}", clrString));
}

namespace AGS
{
	namespace Native
	{
		NativeMethods::NativeMethods(String ^editorVersion)
		{
			lastPaletteSet = nullptr;
			editorVersionNumber = ConvertStringToNativeString(editorVersion);
		}

		void NativeMethods::Initialize()
		{
			if (!initialize_native())
			{
				throw gcnew AGS::Types::InvalidDataException("Native initialization failed.");
			}
		}

        void NativeMethods::NewWorkingDirSet(String^ workingDir)
        {
            GameDirChanged(workingDir);
        }

		void NativeMethods::NewGameLoaded(Game ^game, List<String^> ^errors)
		{
			this->PaletteColoursUpdated(game);
			GameUpdated(game, true);
			UpdateNativeSpritesToGame(game, errors);
		}

		void NativeMethods::PaletteColoursUpdated(Game ^game)
		{
			lastPaletteSet = game->Palette;
			PaletteUpdated(game->Palette);
		}

		void NativeMethods::LoadNewSpriteFile() 
		{
			HAGSError err = reset_sprite_file();
			if (!err)
			{
				throw gcnew AGSEditorException(gcnew String("Unable to load spriteset from ACSPRSET.SPR.\n") + ToStr(err->FullMessage()));
			}
		}

		void NativeMethods::SaveGame(Game ^game)
		{
			::SaveGame(game->Settings->CompressSprites);
		}

		void NativeMethods::GameSettingsChanged(Game ^game)
		{
			GameUpdated(game, false);
		}

		void NativeMethods::DrawGUI(int hDC, int x, int y, GUI^ gui, int resolutionFactor, float scale, int selectedControl)
		{
			drawGUI(hDC, x, y, gui, resolutionFactor, scale, selectedControl);
		}

		void NativeMethods::DrawSprite(int hDC, int x, int y, int spriteNum, bool flipImage)
		{
			drawSprite(hDC, x, y, spriteNum, flipImage);
		}

		int NativeMethods::DrawFont(int hDC, int x, int y, int width, int fontNum)
		{
			return drawFontAt(hDC, fontNum, x, y, width);
		}

		void NativeMethods::DrawSprite(int hDC, int x, int y, int width, int height, int spriteNum, bool flipImage)
		{
			drawSpriteStretch(hDC, x, y, width, height, spriteNum, flipImage);
		}

		void NativeMethods::DrawBlockOfColour(int hDC, int x, int y, int width, int height, int colourNum)
		{
			drawBlockOfColour(hDC, x, y, width, height, colourNum);
		}

		void NativeMethods::DrawViewLoop(int hdc, ViewLoop^ loopToDraw, int x, int y, int size, int cursel)
		{
			drawViewLoop(hdc, loopToDraw, x, y, size, cursel);
		}

		bool NativeMethods::DoesSpriteExist(int spriteNumber)
        {
			if ((spriteNumber < 0) || (spriteNumber >= GetMaxSprites()))
			{
				return false;
			}
			return ::DoesSpriteExist(spriteNumber);
        }

		void NativeMethods::ImportSCIFont(String ^fileName, int fontSlot) 
		{
			AGSString fileNameAnsi = ConvertFileNameToNativeString(fileName);
			AGSString errorMsg = import_sci_font(fileNameAnsi, fontSlot);
			if (errorMsg != NULL) 
			{
				throw gcnew AGSEditorException(ToStr(errorMsg));
			}
		}

    void NativeMethods::ReloadFont(int fontSlot)
    {
      if (!reload_font(fontSlot))
      {
        throw gcnew AGSEditorException(String::Format("Unable to load font {0}. No supported font renderer was able to load the font.", fontSlot));
      }
    }

    void NativeMethods::OnGameFontUpdated(Game^ game, int fontSlot, bool forceUpdate)
    {
        GameFontUpdated(game, fontSlot, forceUpdate);
    }

        AGS::Types::SpriteInfo^ NativeMethods::GetSpriteInfo(int spriteSlot)
        {
            ::SpriteInfo info;
            ::GetSpriteInfo(spriteSlot, info);
            return gcnew AGS::Types::SpriteInfo(info.Width, info.Height, SpriteImportResolution::Real);
        }

		int NativeMethods::GetSpriteWidth(int spriteSlot) 
		{
			return ::GetSpriteWidth(spriteSlot);
		}

		int NativeMethods::GetSpriteHeight(int spriteSlot) 
		{
			return ::GetSpriteHeight(spriteSlot);
		}

		void NativeMethods::ChangeSpriteNumber(Sprite^ sprite, int newNumber)
		{
			if ((newNumber < 0) || (newNumber >= GetMaxSprites()))
			{
				throw gcnew AGSEditorException(gcnew String("Invalid sprite number"));
			}
			change_sprite_number(sprite->Number, newNumber);
			sprite->Number = newNumber;
		}

		void NativeMethods::SpriteResolutionsChanged(cli::array<Sprite^>^ sprites)
		{
			for each (Sprite^ sprite in sprites)
			{
				update_sprite_resolution(sprite->Number);
			}
		}

		Sprite^ NativeMethods::SetSpriteFromBitmap(int spriteSlot, Bitmap^ bmp, int spriteImportMethod, bool remapColours, bool useRoomBackgroundColours, bool alphaChannel)
		{
            SetNewSpriteFromBitmap(spriteSlot, bmp, spriteImportMethod, remapColours, useRoomBackgroundColours, alphaChannel);
      int colDepth = GetSpriteColorDepth(spriteSlot);
			Sprite^ newSprite = gcnew Sprite(spriteSlot, bmp->Width, bmp->Height, colDepth, alphaChannel);
      int roomNumber = GetCurrentlyLoadedRoomNumber();
      if ((colDepth == 8) && (useRoomBackgroundColours) && (roomNumber >= 0))
      {
        newSprite->ColoursLockedToRoom = roomNumber;
      }
      return newSprite;
		}

		void NativeMethods::ReplaceSpriteWithBitmap(Sprite ^spr, Bitmap^ bmp, int spriteImportMethod, bool remapColours, bool useRoomBackgroundColours, bool alphaChannel)
		{
            SetNewSpriteFromBitmap(spr->Number, bmp, spriteImportMethod, remapColours, useRoomBackgroundColours, alphaChannel);
			spr->ColorDepth = GetSpriteColorDepth(spr->Number);
			spr->Width = bmp->Width;
			spr->Height = bmp->Height;
			spr->AlphaChannel = alphaChannel;
      spr->ColoursLockedToRoom = System::Nullable<int>();
      int roomNumber = GetCurrentlyLoadedRoomNumber();
      if ((spr->ColorDepth == 8) && (useRoomBackgroundColours) && (roomNumber >= 0))
      {
        spr->ColoursLockedToRoom = roomNumber;
      }
		}

		Bitmap^ NativeMethods::GetBitmapForSprite(int spriteSlot, int width, int height)
		{
/*			int spriteWidth = GetSpriteWidth(spriteSlot);
			int spriteHeight = GetSpriteHeight(spriteSlot);
			int colDepth = GetSpriteColorDepth(spriteSlot);
/*			int stride = spriteWidth * ((colDepth + 1) / 8);
			PixelFormat pFormat;
			if (colDepth == 8)
			{
				pFormat = PixelFormat::Format8bppIndexed;
			}
			else if (colDepth == 15)
			{
				pFormat = PixelFormat::Format16bppRgb555;
			}
			else if (colDepth == 16)
			{
				pFormat = PixelFormat::Format16bppRgb565;
			}
			else
			{
				pFormat = PixelFormat::Format32bppRgb;
			}

			unsigned char *spriteData = GetRawSpriteData(spriteSlot);
			/*IntPtr intPtr(spriteData);
			Bitmap ^newBitmap = gcnew Bitmap(spriteWidth, spriteHeight, stride, pFormat, intPtr);*/
/*			Bitmap ^newBitmap = gcnew Bitmap(spriteWidth, spriteHeight, pFormat);
			System::Drawing::Rectangle rect(0, 0, spriteWidth, spriteHeight);
			BitmapData ^bmpData = newBitmap->LockBits(rect, ImageLockMode::WriteOnly, pFormat);
			memcpy(bmpData->Scan0.ToPointer(), spriteData, stride * spriteHeight);
			newBitmap->UnlockBits(bmpData);

			if (pFormat == PixelFormat::Format8bppIndexed)
			{
				for each (PaletteEntry^ palEntry in lastPaletteSet)
				{
					newBitmap->Palette->Entries[palEntry->Index] = palEntry->Colour;
				}
				newBitmap->Palette = newBitmap->Palette;
			}* /
			int hBmp = GetSpriteAsHBitmap(spriteSlot);
			Bitmap^ newBitmap;
			if (GetSpriteColorDepth(spriteSlot) == 8) 
			{
				int hPal = GetPaletteAsHPalette();
				newBitmap = Bitmap::FromHbitmap((IntPtr)hBmp, (IntPtr)hPal);
				DeleteObject((HPALETTE)hPal);
			}
			else
			{
				newBitmap = Bitmap::FromHbitmap((IntPtr)hBmp);
			}
			DeleteObject((HBITMAP)hBmp);
			return newBitmap;
*/
			return getSpriteAsBitmap32bit(spriteSlot, width, height);
		}

		Bitmap^ NativeMethods::GetBitmapForSpritePreserveColDepth(int spriteSlot)
		{
      return getSpriteAsBitmap(spriteSlot);
    }

		void NativeMethods::DeleteSprite(int spriteSlot)
		{
			deleteSprite(spriteSlot);
		}

		int NativeMethods::GetFreeSpriteSlot()
		{
			return find_free_sprite_slot();
		}

		bool NativeMethods::CropSpriteEdges(System::Collections::Generic::IList<Sprite^>^ sprites, bool symmetric)
		{	
			int *spriteSlotList = new int[sprites->Count];
			int i = 0;
			for each (Sprite^ sprite in sprites)
			{
				spriteSlotList[i] = sprite->Number;
				i++;
			}
			bool result = crop_sprite_edges(sprites->Count, spriteSlotList, symmetric) != 0;
			delete spriteSlotList;

			int newWidth = GetSpriteWidth(sprites[0]->Number);
			int newHeight = GetSpriteHeight(sprites[0]->Number);
			for each (Sprite^ sprite in sprites)
			{
				sprite->Width = newWidth;
				sprite->Height = newHeight;
			}
			return result;
		}

		void NativeMethods::Shutdown()
		{
			shutdown_native();
		}

		AGS::Types::Game^ NativeMethods::ImportOldGameFile(String^ fileName)
		{
			AGSString fileNameAnsi = ConvertFileNameToNativeString(fileName);
			Game ^game = import_compiled_game_dta(fileNameAnsi);
			return game;
		}

		Dictionary<int,Sprite^>^ NativeMethods::LoadAllSpriteDimensions()
		{
			return load_sprite_dimensions();
		}

		AGS::Types::Room^ NativeMethods::LoadRoomFile(UnloadedRoom^ roomToLoad)
		{
			return load_crm_file(roomToLoad);
		}

		void NativeMethods::SaveRoomFile(AGS::Types::Room ^roomToSave)
		{
			save_crm_file(roomToSave);
		}

		void NativeMethods::ImportBackground(Room ^room, int backgroundNumber, Bitmap ^bmp, bool useExactPalette, bool sharePalette)
		{
			::ImportBackground(room, backgroundNumber, bmp, useExactPalette, sharePalette);
		}

		void NativeMethods::DeleteBackground(Room ^room, int backgroundNumber)
		{
			::DeleteBackground(room, backgroundNumber);
		}

		Bitmap^ NativeMethods::GetBitmapForBackground(Room ^room, int backgroundNumber)
		{
			return getBackgroundAsBitmap(room, backgroundNumber);
		}

    void NativeMethods::SetAreaMask(Room^ room, RoomAreaMaskType maskType, Bitmap^ bmp)
    {
        set_area_mask((void*)room->_roomStructPtr, (int)maskType, bmp);
    }

    Bitmap ^NativeMethods::ExportAreaMask(Room ^room, RoomAreaMaskType maskType)
    {
        return export_area_mask((void*)room->_roomStructPtr, (int)maskType);
    }

		String ^NativeMethods::LoadRoomScript(String ^roomFileName) 
		{
			return load_room_script(roomFileName);
		}

    BaseTemplate^ NativeMethods::LoadTemplateFile(String ^fileName, bool isRoomTemplate)
    {
      AGSString fileNameAnsi = ConvertFileNameToNativeString(fileName);
      char *iconDataBuffer = NULL;
      long iconDataSize = 0;

      int success = load_template_file(fileNameAnsi, &iconDataBuffer, &iconDataSize, isRoomTemplate);
			if (success) 
			{
				Icon ^icon = nullptr;
				if (iconDataBuffer != NULL)
				{
          cli::array<unsigned char>^ managedArray = gcnew cli::array<unsigned char>(iconDataSize);
          Marshal::Copy(IntPtr(iconDataBuffer), managedArray, 0, iconDataSize);
          ::free(iconDataBuffer);
          System::IO::MemoryStream^ ms = gcnew System::IO::MemoryStream(managedArray);
          try 
          {
					  icon = gcnew Icon(ms);
          } 
          catch (ArgumentException^) 
          {
            // it is not a valid .ICO file, ignore it
            icon = nullptr;
          }
				}
        if (isRoomTemplate)
        {
          return gcnew RoomTemplate(fileName, icon);
        }
        else
        {
				  return gcnew GameTemplate(fileName, icon);
        }
			}
			return nullptr;
    }

		GameTemplate^ NativeMethods::LoadTemplateFile(String ^fileName)
		{
      return (GameTemplate^)LoadTemplateFile(fileName, false);
		}

    RoomTemplate^ NativeMethods::LoadRoomTemplateFile(String ^fileName)
		{
      return (RoomTemplate^)LoadTemplateFile(fileName, true);
		}

		void NativeMethods::ExtractTemplateFiles(String ^templateFileName) 
		{
			AGSString fileNameAnsi = ConvertFileNameToNativeString(templateFileName);
            HAGSError err = extract_template_files(fileNameAnsi);
			if (!err)
			{
				throw gcnew AGSEditorException("Unable to extract template files.\n" + ToStr(err->FullMessage()));
			}
		}

		void NativeMethods::ExtractRoomTemplateFiles(String ^templateFileName, int newRoomNumber) 
		{
			AGSString fileNameAnsi = ConvertFileNameToNativeString(templateFileName);
            HAGSError err = extract_room_template_files(fileNameAnsi, newRoomNumber);
			if (!err)
			{
				throw gcnew AGSEditorException("Unable to extract template files.\n" + ToStr(err->FullMessage()));
			}
		}
				
		cli::array<unsigned char>^ NativeMethods::TransformStringToBytes(String ^text) 
		{
			char* stringPointer = (char*)Marshal::StringToHGlobalAnsi(text).ToPointer();
			int textLength = text->Length + 1;
			cli::array<unsigned char>^ toReturn = gcnew cli::array<unsigned char>(textLength + 4);
			toReturn[0] = textLength % 256;
			toReturn[1] = textLength / 256;
			toReturn[2] = 0;
			toReturn[3] = 0;
	
			transform_string(stringPointer);

			{ pin_ptr<unsigned char> nativeBytes = &toReturn[4];
				memcpy(nativeBytes, stringPointer, textLength);
			}

			Marshal::FreeHGlobal(IntPtr(stringPointer));

			return toReturn;
		}

		bool NativeMethods::HaveSpritesBeenModified()
		{
			return spritesModified;
		}

        /// <summary>
        /// Allows the Editor to reuse constants from the native code. If a constant required by the Editor
        /// is not also required by the Engine, then it should instead by moved into AGS.Types (AGS.Native
        /// references the AGS.Types assembly). Note that this method returns only System::Int32 and
        /// System::String objects -- it is up to the user to determine if the value should be used as a
        /// smaller integral type (additional casting may be required to cast to a non-int integral type).
        /// </summary>
        Object^ NativeMethods::GetNativeConstant(String ^name)
        {
            if (name == nullptr) return nullptr;
            if (name->Equals("GAME_FILE_SIG")) return gcnew String(game_file_sig);
            if (name->Equals("GAME_DATA_VERSION_CURRENT")) return (int)kGameVersion_Current;
            if (name->Equals("MAX_GUID_LENGTH")) return MAX_GUID_LENGTH;
            if (name->Equals("MAX_SG_EXT_LENGTH")) return MAX_SG_EXT_LENGTH;
            if (name->Equals("MAX_SG_FOLDER_LEN")) return MAX_SG_FOLDER_LEN;
            if (name->Equals("MAX_SCRIPT_NAME_LEN")) return MAX_SCRIPT_NAME_LEN;
            if (name->Equals("FFLG_SIZEMULTIPLIER")) return FFLG_SIZEMULTIPLIER;
            if (name->Equals("IFLG_STARTWITH")) return IFLG_STARTWITH;
            if (name->Equals("MCF_ANIMMOVE")) return MCF_ANIMMOVE;
            if (name->Equals("MCF_STANDARD")) return MCF_STANDARD;
            if (name->Equals("MCF_HOTSPOT")) return MCF_HOTSPOT;
            if (name->Equals("CHF_MANUALSCALING")) return CHF_MANUALSCALING;
            if (name->Equals("CHF_NOINTERACT")) return CHF_NOINTERACT;
            if (name->Equals("CHF_NODIAGONAL")) return CHF_NODIAGONAL;
            if (name->Equals("CHF_NOLIGHTING")) return CHF_NOLIGHTING;
            if (name->Equals("CHF_NOTURNING")) return CHF_NOTURNING;
            if (name->Equals("CHF_NOBLOCKING")) return CHF_NOBLOCKING;
            if (name->Equals("CHF_SCALEMOVESPEED")) return CHF_SCALEMOVESPEED;
            if (name->Equals("CHF_SCALEVOLUME")) return CHF_SCALEVOLUME;
            if (name->Equals("CHF_ANTIGLIDE")) return CHF_ANTIGLIDE;
            if (name->Equals("DFLG_ON")) return DFLG_ON;
            if (name->Equals("DFLG_NOREPEAT")) return DFLG_NOREPEAT;
            if (name->Equals("DTFLG_SHOWPARSER")) return DTFLG_SHOWPARSER;
            if (name->Equals("FONT_OUTLINE_AUTO")) return FONT_OUTLINE_AUTO;
            if (name->Equals("MAX_STATIC_SPRITES")) return MAX_STATIC_SPRITES;
            if (name->Equals("MAX_CURSOR")) return MAX_CURSOR;
            if (name->Equals("MAX_PARSER_WORD_LENGTH")) return MAX_PARSER_WORD_LENGTH;
            if (name->Equals("MAX_INV")) return MAX_INV;
            if (name->Equals("MAXLIPSYNCFRAMES")) return MAXLIPSYNCFRAMES;
            if (name->Equals("MAXGLOBALMES")) return MAXGLOBALMES;
            if (name->Equals("MAXTOPICOPTIONS")) return MAXTOPICOPTIONS;
            if (name->Equals("UNIFORM_WALK_SPEED")) return UNIFORM_WALK_SPEED;
            if (name->Equals("GAME_RESOLUTION_CUSTOM")) return (int)kGameResolution_Custom;
            if (name->Equals("CHUNKSIZE")) return CHUNKSIZE;
            if (name->Equals("SPRSET_NAME")) return gcnew String(sprsetname);
            if (name->Equals("SPF_VAR_RESOLUTION")) return SPF_VAR_RESOLUTION;
            if (name->Equals("SPF_HIRES")) return SPF_HIRES;
            if (name->Equals("SPF_ALPHACHANNEL")) return SPF_ALPHACHANNEL;
            if (name->Equals("PASSWORD_ENC_STRING"))
            {
                int len = (int)strlen(passwencstring);
                array<System::Byte>^ bytes = gcnew array<System::Byte>(len);
                System::Runtime::InteropServices::Marshal::Copy( IntPtr( ( char* ) passwencstring ), bytes, 0, len );
                return bytes;
            }
            if (name->Equals("LOOPFLAG_RUNNEXTLOOP")) return LOOPFLAG_RUNNEXTLOOP;
            if (name->Equals("VFLG_FLIPSPRITE")) return VFLG_FLIPSPRITE;
            if (name->Equals("GUIMAGIC")) return GUIMAGIC;
            if (name->Equals("SAVEBUFFERSIZE")) return PLUGIN_SAVEBUFFERSIZE;
            if (name->Equals("GUIMAIN_CLICKABLE")) return (int)Common::kGUIMain_Clickable;
            if (name->Equals("GUIMAIN_VISIBLE")) return (int)Common::kGUIMain_Visible;
            if (name->Equals("GUIF_ENABLED")) return (int)Common::kGUICtrl_Enabled;
            if (name->Equals("GUIF_CLICKABLE")) return (int)Common::kGUICtrl_Clickable;
            if (name->Equals("GUIF_VISIBLE")) return (int)Common::kGUICtrl_Visible;
            if (name->Equals("GUIF_CLIP")) return (int)Common::kGUICtrl_Clip;
            if (name->Equals("GUIF_TRANSLATED")) return (int)Common::kGUICtrl_Translated;
            if (name->Equals("GLF_SHOWBORDER")) return (int)Common::kListBox_ShowBorder;
            if (name->Equals("GLF_SHOWARROWS")) return (int)Common::kListBox_ShowArrows;
            if (name->Equals("GUI_POPUP_MODAL")) return (int)Common::kGUIPopupModal;
            if (name->Equals("GUIMAIN_TEXTWINDOW")) return (int)Common::kGUIMain_TextWindow;
            if (name->Equals("GUIMAIN_LEGACYTEXTWINDOW")) return (int)Common::kGUIMain_LegacyTextWindow;
            if (name->Equals("GTF_SHOWBORDER")) return (int)Common::kTextBox_ShowBorder;
            if (name->Equals("GOBJ_BUTTON")) return (int)Common::kGUIButton;
            if (name->Equals("GOBJ_LABEL")) return (int)Common::kGUILabel;
            if (name->Equals("GOBJ_INVENTORY")) return (int)Common::kGUIInvWindow;
            if (name->Equals("GOBJ_SLIDER")) return (int)Common::kGUISlider;
            if (name->Equals("GOBJ_TEXTBOX")) return (int)Common::kGUITextBox;
            if (name->Equals("GOBJ_LISTBOX")) return (int)Common::kGUIListBox;
            if (name->Equals("TEXTWINDOW_PADDING_DEFAULT")) return TEXTWINDOW_PADDING_DEFAULT;
            if (name->Equals("GUI_VERSION_CURRENT")) return (int)kGuiVersion_Current;
            if (name->Equals("CUSTOM_PROPERTY_SCHEMA_VERSION")) return (int)AGS::Common::kPropertyVersion_Current;
            if (name->Equals("OPT_DEBUGMODE")) return OPT_DEBUGMODE;
            if (name->Equals("OPT_WALKONLOOK")) return OPT_WALKONLOOK;
            if (name->Equals("OPT_DIALOGIFACE")) return OPT_DIALOGIFACE;
            if (name->Equals("OPT_ANTIGLIDE")) return OPT_ANTIGLIDE;
            if (name->Equals("OPT_TWCUSTOM")) return OPT_TWCUSTOM;
            if (name->Equals("OPT_DIALOGGAP")) return OPT_DIALOGGAP;
            if (name->Equals("OPT_NOSKIPTEXT")) return OPT_NOSKIPTEXT;
            if (name->Equals("OPT_DISABLEOFF")) return OPT_DISABLEOFF;
            if (name->Equals("OPT_ALWAYSSPCH")) return OPT_ALWAYSSPCH;
            if (name->Equals("OPT_SPEECHTYPE")) return OPT_SPEECHTYPE;
            if (name->Equals("OPT_PIXPERFECT")) return OPT_PIXPERFECT;
            if (name->Equals("OPT_NOWALKMODE")) return OPT_NOWALKMODE;
            if (name->Equals("OPT_LETTERBOX")) return OPT_LETTERBOX;
            if (name->Equals("OPT_FIXEDINVCURSOR")) return OPT_FIXEDINVCURSOR;
            if (name->Equals("OPT_HIRES_FONTS")) return OPT_HIRES_FONTS;
            if (name->Equals("OPT_SPLITRESOURCES")) return OPT_SPLITRESOURCES;
            if (name->Equals("OPT_ROTATECHARS")) return OPT_ROTATECHARS;
            if (name->Equals("OPT_FADETYPE")) return OPT_FADETYPE;
            if (name->Equals("OPT_HANDLEINVCLICKS")) return OPT_HANDLEINVCLICKS;
            if (name->Equals("OPT_MOUSEWHEEL")) return OPT_MOUSEWHEEL;
            if (name->Equals("OPT_DIALOGNUMBERED")) return OPT_DIALOGNUMBERED;
            if (name->Equals("OPT_DIALOGUPWARDS")) return OPT_DIALOGUPWARDS;
            if (name->Equals("OPT_ANTIALIASFONTS")) return OPT_ANTIALIASFONTS;
            if (name->Equals("OPT_THOUGHTGUI")) return OPT_THOUGHTGUI;
            if (name->Equals("OPT_TURNTOFACELOC")) return OPT_TURNTOFACELOC;
            if (name->Equals("OPT_RIGHTLEFTWRITE")) return OPT_RIGHTLEFTWRITE;
            if (name->Equals("OPT_DUPLICATEINV")) return OPT_DUPLICATEINV;
            if (name->Equals("OPT_SAVESCREENSHOT")) return OPT_SAVESCREENSHOT;
            if (name->Equals("OPT_PORTRAITSIDE")) return OPT_PORTRAITSIDE;
            if (name->Equals("OPT_STRICTSCRIPTING")) return OPT_STRICTSCRIPTING;
            if (name->Equals("OPT_COMPRESSSPRITES")) return OPT_COMPRESSSPRITES;
            if (name->Equals("OPT_STRICTSTRINGS")) return OPT_STRICTSTRINGS;
            if (name->Equals("OPT_RUNGAMEDLGOPTS")) return OPT_RUNGAMEDLGOPTS;
            if (name->Equals("OPT_GLOBALTALKANIMSPD")) return OPT_GLOBALTALKANIMSPD;
            if (name->Equals("OPT_SAFEFILEPATHS")) return OPT_SAFEFILEPATHS;
            if (name->Equals("OPT_DIALOGOPTIONSAPI")) return OPT_DIALOGOPTIONSAPI;
            if (name->Equals("OPT_BASESCRIPTAPI")) return OPT_BASESCRIPTAPI;
            if (name->Equals("OPT_SCRIPTCOMPATLEV")) return OPT_SCRIPTCOMPATLEV;
            if (name->Equals("OPT_RENDERATSCREENRES")) return OPT_RENDERATSCREENRES;
            if (name->Equals("OPT_RELATIVEASSETRES")) return OPT_RELATIVEASSETRES;
            if (name->Equals("OPT_WALKSPEEDABSOLUTE")) return OPT_WALKSPEEDABSOLUTE;
            if (name->Equals("OPT_LIPSYNCTEXT")) return OPT_LIPSYNCTEXT;
			if (name->Equals("MAX_PLUGINS")) return MAX_PLUGINS;
            return nullptr;
        }
	}
}

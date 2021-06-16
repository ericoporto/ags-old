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

#include <SDL_ttf.h>
#include <debug/out.h>
#include <gfx/blender.h>
// #include <hb.h> // needed for HB related codes
#include "allegro/gfx.h"
#include "core/platform.h"
#include "gfx/bitmap.h"

#define AGS_OUTLINE_FONT_FIX (!AGS_PLATFORM_OS_WINDOWS)

#include "core/assetmanager.h"
#include "font/ttffontrenderer.h"
#include "util/stream.h"

#if AGS_OUTLINE_FONT_FIX // TODO: factor out the hack in LoadFromDiskEx
#include "ac/gamestructdefines.h"
#include "font/fonts.h"
#endif

using namespace AGS::Common;

#define algetr32(c) getr32(c)
#define algetg32(c) getg32(c)
#define algetb32(c) getb32(c)
#define algeta32(c) geta32(c)

// project-specific implementation
extern bool ShouldAntiAliasText();

// ***** TTF RENDERER *****
void TTFFontRenderer::AdjustYCoordinateForFont(int *ycoord, int fontNumber)
{
  // TTF fonts already have space at the top, so try to remove the gap
  // TODO: adding -1 was here before (check the comment above),
  // but how universal is this "space at the top"?
  // Also, why is this function used only in one case of text rendering?
  // Review this after we upgrade the font library.
  ycoord[0]--;
}

void TTFFontRenderer::EnsureTextValidForFont(char *text, int fontNumber)
{
  // do nothing, TTF can handle all characters
}

int TTFFontRenderer::GetTextWidth(const char *text, int fontNumber)
{
    int w;
    TTF_SizeUTF8(_fontData[fontNumber].Font , text, &w, nullptr);
    return w;
}

int TTFFontRenderer::GetTextHeight(const char *text, int fontNumber)
{
    int h;
    TTF_SizeUTF8(_fontData[fontNumber].Font , text, nullptr, &h);
    return h;
}

void TTFFontRenderer::RenderText(const char *text, int fontNumber, BITMAP *destination, int x, int y, int colour)
{
    if (y > destination->cb)  // optimisation
        return;

    //TTF_SetScript(HB_SCRIPT_ARABIC);
    //TTF_SetDirection(HB_DIRECTION_RTL);
    const int dest_depth = bitmap_color_depth(destination);
    SDL_Surface* glyph;
    SDL_Color sdlColor = {(Uint8) algetr32(colour),
                          (Uint8) algetg32(colour),
                          (Uint8) algetb32(colour),
                          (Uint8) algeta32(colour)};

    TTF_Font* font = _fontData[fontNumber].Font;
    if ((ShouldAntiAliasText()) && (dest_depth > 8))
        glyph = TTF_RenderUTF8_Blended(font, text, sdlColor); //SDL_PIXELFORMAT_ARGB8888
    else if (dest_depth > 8){
        SDL_Surface * surface = TTF_RenderUTF8_Solid(font, text, sdlColor);
        glyph = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ARGB8888, 0);
        SDL_FreeSurface(surface);
    } else {
        glyph = TTF_RenderUTF8_Solid(font, text, sdlColor);
    }

    if(!glyph) {
        const char *errormsg = TTF_GetError();
        printf("Error : %s", errormsg);
        return;
    }

    BITMAP *sourcebmp = wrap_bitmap_sdl_surface(glyph, dest_depth);

    if(glyph->format->format == SDL_PIXELFORMAT_ARGB8888) {
        set_argb2argb_blender(255);
        draw_trans_sprite(destination, sourcebmp, x, y);
    } else if(glyph->format->palette != nullptr) {
        set_additive_alpha_blender();
        draw_trans_sprite(destination, sourcebmp, x, y);
    }

    SDL_FreeSurface(glyph);
    delete sourcebmp;
}

bool TTFFontRenderer::LoadFromDisk(int fontNumber, int fontSize)
{
  return LoadFromDiskEx(fontNumber, fontSize, nullptr);
}

bool TTFFontRenderer::IsBitmapFont()
{
    return false;
}

bool TTFFontRenderer::LoadFromDiskEx(int fontNumber, int fontSize, const FontRenderParams *params)
{
  String file_name = String::FromFormat("agsfnt%d.ttf", fontNumber);
  soff_t lenof = 0;
  Stream *reader = AssetMgr->OpenAsset(file_name, &lenof);
  char *membuffer;

  if (reader == nullptr)
    return false;

  membuffer = (char *)malloc(lenof);
  reader->ReadArray(membuffer, lenof, 1);
  delete reader;

    // Load the font data into a memory buffer
    SDL_RWops* pFontMem = SDL_RWFromConstMem(membuffer, lenof);
    if(!pFontMem)
    {
        printf("Error when reading font from memory");
    }

    if (fontSize == 0)
        fontSize = 8; // compatibility fix
    if (params && params->SizeMultiplier > 1)
        fontSize *= params->SizeMultiplier;

    // Load the font from the memory buffer
    TTF_Font* pFont = TTF_OpenFontRW(pFontMem, 1, fontSize);

    // free(membuffer);

    if(pFont == nullptr) {
        printf("Error when loading font TTF_OpenFontRW");
        return false;
    }

  // TODO: move this somewhere, should not be right here
#if AGS_OUTLINE_FONT_FIX
  // FIXME: (!!!) this fix should be done differently:
  // 1. Find out which OUTLINE font was causing troubles;
  // 2. Replace outline method ONLY if that troublesome font is used as outline.
  // 3. Move this fix somewhere else!! (right after game load routine?)
  //
  // Check for the LucasFan font since it comes with an outline font that
  // is drawn incorrectly with Freetype versions > 2.1.3.
  // A simple workaround is to disable outline fonts for it and use
  // automatic outline drawing.
  if (get_font_outline(fontNumber) >=0 &&
      strcmp(alfont_get_name(alfptr), "LucasFan-Font") == 0)
      set_font_outline(fontNumber, FONT_OUTLINE_AUTO);
#endif

  _fontData[fontNumber].Font = pFont;
  _fontData[fontNumber].Params = params ? *params : FontRenderParams();
  return true;
}

void TTFFontRenderer::FreeMemory(int fontNumber)
{
   TTF_CloseFont(_fontData[fontNumber].Font);
  _fontData.erase(fontNumber);
}

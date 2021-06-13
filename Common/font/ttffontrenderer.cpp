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

/*
   JJS: These functions replace the standard Allegro blender.
   Code is reverse-engineered from the alfont MSVC library.
   The blender functions are based on the originals with small modifications
   that enable correct drawing of anti-aliased fonts.
*/

/* original: _blender_trans15 in colblend.c */
unsigned long __skiptranspixels_blender_trans15(unsigned long x, unsigned long y, unsigned long n)
{
    unsigned long result;

    if ((y & 0xFFFF) == 0x7C1F)
        return x;

    if (n)
        n = (n + 1) / 8;

    x = ((x & 0xFFFF) | (x << 16)) & 0x3E07C1F;
    y = ((y & 0xFFFF) | (y << 16)) & 0x3E07C1F;

    result = ((x - y) * n / 32 + y) & 0x3E07C1F;

    return ((result & 0xFFFF) | (result >> 16));
}

/* original: _blender_trans16 in colblend.c */
unsigned long __skiptranspixels_blender_trans16(unsigned long x, unsigned long y, unsigned long n)
{
    unsigned long result;

    if ((y & 0xFFFF) == 0xF81F)
        return x;

    if (n)
        n = (n + 1) / 8;

    x = ((x & 0xFFFF) | (x << 16)) & 0x7E0F81F;
    y = ((y & 0xFFFF) | (y << 16)) & 0x7E0F81F;

    result = ((x - y) * n / 32 + y) & 0x7E0F81F;

    return ((result & 0xFFFF) | (result >> 16));
}

/* original: _blender_trans24 in colblend.c */
unsigned long __preservedalpha_blender_trans24(unsigned long x, unsigned long y, unsigned long n)
{
    unsigned long res, g, alpha;

    alpha = (y & 0xFF000000);

    if ((y & 0xFFFFFF) == 0xFF00FF)
        return ((x & 0xFFFFFF) | (n << 24));

    if (n)
        n++;

    res = ((x & 0xFF00FF) - (y & 0xFF00FF)) * n / 256 + y;
    y &= 0xFF00;
    x &= 0xFF00;
    g = (x - y) * n / 256 + y;

    res &= 0xFF00FF;
    g &= 0xFF00;

    return res | g | alpha;
}

/* replaces set_trans_blender() */
void set_preservedalpha_trans_blender(int r, int g, int b, int a)
{
    set_blender_mode(__skiptranspixels_blender_trans15, __skiptranspixels_blender_trans16, __preservedalpha_blender_trans24, r, g, b, a);
}


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
    TTF_SizeText(_fontData[fontNumber].Font , text, &w, nullptr);
    return w;
}

int TTFFontRenderer::GetTextHeight(const char *text, int fontNumber)
{
    int h;
    TTF_SizeText(_fontData[fontNumber].Font , text, nullptr, &h);
    return h;
}

void TTFFontRenderer::RenderText(const char *text, int fontNumber, BITMAP *destination, int x, int y, int colour)
{
    if (y > destination->cb)  // optimisation
        return;

    SDL_Surface* glyph;
    SDL_Color sdlColor = {(Uint8) algetr32(colour),
                          (Uint8) algetg32(colour),
                          (Uint8) algetb32(colour),
                          (Uint8) algeta32(colour)};

    // Y - 1 because it seems to get drawn down a bit
    //x, y - 1, colour
    TTF_Font* font = _fontData[fontNumber].Font;
    if ((ShouldAntiAliasText()) && (bitmap_color_depth(destination) > 8))
        glyph = TTF_RenderText_Blended(font, text, sdlColor);
    else
        glyph = TTF_RenderText_Solid(font, text, sdlColor);

    if(!glyph) {
        const char *errormsg = TTF_GetError();
        printf("Error : %s", errormsg);
        return;
    }

    SDL_Surface * surface = SDL_CreateRGBSurfaceWithFormat(0, glyph->w,glyph->h, 32, SDL_PIXELFORMAT_RGBA8888);
    SDL_BlitSurface(glyph, nullptr, surface, nullptr);

    Bitmap *dest = BitmapHelper::CreateRawBitmapWrapper(destination);

    BITMAP *sourcebmp = wrap_bitmap_sdl_surface(surface);
    Bitmap *source = BitmapHelper::CreateRawBitmapWrapper(sourcebmp);

    set_preservedalpha_trans_blender(0,0,0,255);
    dest->TransBlendBlt(source, x, y);

    SDL_FreeSurface(glyph);
    SDL_FreeSurface(surface);
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

    // TTF_Font *alfptr = alfont_load_font_from_mem(membuffer, lenof);
    // Load the font data into a memory buffer
    SDL_RWops* pFontMem = SDL_RWFromConstMem(membuffer, lenof);
    if(!pFontMem)
    {
        printf("Error when reading font from memory");
        // Some error occurred while trying to read the data, act accordingly to that
    }

    // Load the font from the memory buffer
    TTF_Font* pFont = TTF_OpenFontRW(pFontMem, 1, fontSize);
    if(!pFont) {
        printf("Error when loading font TTF_OpenFontRW");
    }

 // free(membuffer);

  if (pFont == nullptr)
    return false;

  // TODO: move this somewhere, should not be right here
#if 0 // AGS_OUTLINE_FONT_FIX
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
  /*
  if (fontSize == 0)
      fontSize = 8; // compatibility fix
  if (params && params->SizeMultiplier > 1)
      fontSize *= params->SizeMultiplier;
  if (fontSize > 0)
      TTF_SetFontSize(pFont, fontSize);
*/
  _fontData[fontNumber].Font = pFont;
  _fontData[fontNumber].Params = params ? *params : FontRenderParams();
  return true;
}

void TTFFontRenderer::FreeMemory(int fontNumber)
{
   // TTF_CloseFont(_fontData[fontNumber].TTF_Font);
  _fontData.erase(fontNumber);
}

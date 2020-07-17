#ifndef __CSTRECH_H
#define __CSTRECH_H

#include "gfx/allegrobitmap.h"

void Cstretch_blit(BITMAP *src, BITMAP *dst, int sx, int sy, int sw, int sh, int dx, int dy, int dw, int dh);
void Cstretch_sprite(BITMAP *dst, BITMAP *src, int x, int y, int w, int h);

#endif // __CSTRECH_H
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

#ifdef _MANAGED
// ensure this doesn't get compiled to .NET IL
#pragma unmanaged
#endif

#include <stdlib.h>
#include <stdio.h>
#include "ac/common.h"	// quit, update_polled_stuff
#include "gfx/bitmap.h"
#include "util/compress.h"
#include "util/lzw.h"
#include "util/misc.h"
#include "util/stream.h"
#if AGS_PLATFORM_ENDIAN_BIG
#include "util/bbop.h"
#endif

using namespace AGS::Common;

void cpackbitl(const uint8_t *line, int size, Stream *out)
{
  int cnt = 0;                  // bytes encoded

  while (cnt < size) {
    int i = cnt;
    int j = i + 1;
    int jmax = i + 126;
    if (jmax >= size)
      jmax = size - 1;

    if (i == size - 1) {        //................last byte alone
      out->WriteInt8(0);
      out->WriteInt8(line[i]);
      cnt++;

    } else if (line[i] == line[j]) {    //....run
      while ((j < jmax) && (line[j] == line[j + 1]))
        j++;
     
      out->WriteInt8(i - j);
      out->WriteInt8(line[i]);
      cnt += j - i + 1;

    } else {                    //.............................sequence
      while ((j < jmax) && (line[j] != line[j + 1]))
        j++;

      out->WriteInt8(j - i);
      out->WriteArray(line + i, j - i + 1, 1);
      cnt += j - i + 1;

    }
  } // end while
}

void cpackbitl16(const uint16_t *line, int size, Stream *out)
{
  int cnt = 0;                  // bytes encoded

  while (cnt < size) {
    int i = cnt;
    int j = i + 1;
    int jmax = i + 126;
    if (jmax >= size)
      jmax = size - 1;

    if (i == size - 1) {        //................last byte alone
      out->WriteInt8(0);
      out->WriteInt16(line[i]);
      cnt++;

    } else if (line[i] == line[j]) {    //....run
      while ((j < jmax) && (line[j] == line[j + 1]))
        j++;
     
      out->WriteInt8(i - j);
      out->WriteInt16(line[i]);
      cnt += j - i + 1;

    } else {                    //.............................sequence
      while ((j < jmax) && (line[j] != line[j + 1]))
        j++;

      out->WriteInt8(j - i);
      out->WriteArray(line + i, j - i + 1, 2);
      cnt += j - i + 1;

    }
  } // end while
}

void cpackbitl32(const uint32_t *line, int size, Stream *out)
{
  int cnt = 0;                  // bytes encoded

  while (cnt < size) {
    int i = cnt;
    int j = i + 1;
    int jmax = i + 126;
    if (jmax >= size)
      jmax = size - 1;

    if (i == size - 1) {        //................last byte alone
      out->WriteInt8(0);
      out->WriteInt32(line[i]);
      cnt++;

    } else if (line[i] == line[j]) {    //....run
      while ((j < jmax) && (line[j] == line[j + 1]))
        j++;
     
      out->WriteInt8(i - j);
      out->WriteInt32(line[i]);
      cnt += j - i + 1;

    } else {                    //.............................sequence
      while ((j < jmax) && (line[j] != line[j + 1]))
        j++;

      out->WriteInt8(j - i);
      out->WriteArray(line + i, j - i + 1, 4);
      cnt += j - i + 1;

    }
  } // end while
}


void csavecompressed(Stream *out, const unsigned char * tobesaved, const RGB pala[256])
{
  int widt, hit;
  widt = *tobesaved++;
  widt += (*tobesaved++) * 256;
  hit = *tobesaved++;
  hit += (*tobesaved++) * 256;
  // Those were originally written as shorts, although they are ints
  out->WriteInt16(widt);
  out->WriteInt16(hit);

  unsigned char *ress = (unsigned char *)malloc(widt + 1);
  int ww;

  for (ww = 0; ww < hit; ww++) {
    for (int ss = 0; ss < widt; ss++)
      (*ress++) = (*tobesaved++);

    ress -= widt;
    cpackbitl(ress, widt, out);
  }

  for (ww = 0; ww < 256; ww++) {
      out->WriteInt8(pala[ww].r);
      out->WriteInt8(pala[ww].g);
      out->WriteInt8(pala[ww].b);
  }
  free(ress);
}

int cunpackbitl(uint8_t *line, int size, Stream *in)
{
  int n = 0;                    // number of bytes decoded

  while (n < size) {
    int ix = in->ReadByte();     // get index byte
    if (in->HasErrors())
      break;

    char cx = ix;
    if (cx == -128)
      cx = 0;

    if (cx < 0) {                //.............run
      int i = 1 - cx;
      char ch = in->ReadInt8();
      while (i--) {
        // test for buffer overflow
        if (n >= size)
          return -1;

        line[n++] = ch;
      }
    } else {                     //.....................seq
      int i = cx + 1;
      while (i--) {
        // test for buffer overflow
        if (n >= size)
          return -1;

        line[n++] = in->ReadByte();
      }
    }
  }

  return in->HasErrors() ? -1 : 0;
}

int cunpackbitl16(uint16_t *line, int size, Stream *in)
{
  int n = 0;                    // number of bytes decoded

  while (n < size) {
    int ix = in->ReadByte();     // get index byte
    if (in->HasErrors())
      break;

    char cx = ix;
    if (cx == -128)
      cx = 0;

    if (cx < 0) {                //.............run
      int i = 1 - cx;
      unsigned short ch = in->ReadInt16();
      while (i--) {
        // test for buffer overflow
        if (n >= size)
          return -1;

        line[n++] = ch;
      }
    } else {                     //.....................seq
      int i = cx + 1;
      while (i--) {
        // test for buffer overflow
        if (n >= size)
          return -1;

        line[n++] = in->ReadInt16();
      }
    }
  }

  return in->HasErrors() ? -1 : 0;
}

int cunpackbitl32(uint32_t *line, int size, Stream *in)
{
  int n = 0;                    // number of bytes decoded

  while (n < size) {
    int ix = in->ReadByte();     // get index byte
    if (in->HasErrors())
      break;

    char cx = ix;
    if (cx == -128)
      cx = 0;

    if (cx < 0) {                //.............run
      int i = 1 - cx;
      unsigned int ch = in->ReadInt32();
      while (i--) {
        // test for buffer overflow
        if (n >= size)
          return -1;

        line[n++] = ch;
      }
    } else {                     //.....................seq
      int i = cx + 1;
      while (i--) {
        // test for buffer overflow
        if (n >= size)
          return -1;

        line[n++] = (unsigned int)in->ReadInt32();
      }
    }
  }

  return in->HasErrors() ? -1 : 0;
}

//=============================================================================

const char *lztempfnm = "~aclzw.tmp";

void save_lzw(Stream *out, const Bitmap *bmpp, const RGB *pall)
{
  // First write original bitmap into temporary file
  Stream *lz_temp_s = ci_fopen(lztempfnm, kFile_CreateAlways, kFile_Write);
  lz_temp_s->WriteInt32(bmpp->GetWidth() * bmpp->GetBPP());
  lz_temp_s->WriteInt32(bmpp->GetHeight());
  lz_temp_s->WriteArray(bmpp->GetData(), bmpp->GetLineLength(), bmpp->GetHeight());
  delete lz_temp_s;

  // Now open same file for reading, and begin writing compressed data into required output stream
  lz_temp_s = ci_fopen(lztempfnm);
  soff_t temp_sz = lz_temp_s->GetLength();
  out->WriteArray(&pall[0], sizeof(RGB), 256);
  out->WriteInt32(temp_sz);
  soff_t gobacto = out->GetPosition();

  // reserve space for compressed size
  out->WriteInt32(temp_sz);
  lzwcompress(lz_temp_s, out);
  soff_t toret = out->GetPosition();
  out->Seek(gobacto, kSeekBegin);
  soff_t compressed_sz = (toret - gobacto) - 4;
  out->WriteInt32(compressed_sz);      // write compressed size

  // Delete temp file
  delete lz_temp_s;
  File::DeleteFile(lztempfnm);

  // Seek back to the end of the output stream
  out->Seek(toret, kSeekBegin);
}

void load_lzw(Stream *in, Bitmap **dst_bmp, int dst_bpp, RGB *pall)
{
  soff_t        uncompsiz;
  int           *loptr;
  unsigned char *membuffer;
  int           arin;

  in->Read(&pall[0], sizeof(RGB)*256);
  maxsize = in->ReadInt32();
  uncompsiz = in->ReadInt32();

  uncompsiz += in->GetPosition();
  outbytes = 0; putbytes = 0;

  update_polled_stuff_if_runtime();
  membuffer = lzwexpand_to_mem(in);
  update_polled_stuff_if_runtime();

  loptr = (int *)&membuffer[0];
  membuffer += 8;
#if AGS_PLATFORM_ENDIAN_BIG
  loptr[0] = BBOp::SwapBytesInt32(loptr[0]);
  loptr[1] = BBOp::SwapBytesInt32(loptr[1]);
  int bitmapNumPixels = loptr[0]*loptr[1]/ dst_bpp;
  switch (dst_bpp) // bytes per pixel!
  {
    case 1:
    {
      // all done
      break;
    }
    case 2:
    {
      short *sp = (short *)membuffer;
      for (int i = 0; i < bitmapNumPixels; ++i)
      {
        sp[i] = BBOp::SwapBytesInt16(sp[i]);
      }
      // all done
      break;
    }
    case 4:
    {
      int *ip = (int *)membuffer;
      for (int i = 0; i < bitmapNumPixels; ++i)
      {
        ip[i] = BBOp::SwapBytesInt32(ip[i]);
      }
      // all done
      break;
    }
  }
#endif // AGS_PLATFORM_ENDIAN_BIG

  update_polled_stuff_if_runtime();

  Bitmap *bmm = BitmapHelper::CreateBitmap((loptr[0] / dst_bpp), loptr[1], dst_bpp * 8);
  if (bmm == nullptr)
    quit("!load_room: not enough memory to load room background");

  update_polled_stuff_if_runtime();

  for (arin = 0; arin < loptr[1]; arin++)
    memcpy(&bmm->GetScanLineForWriting(arin)[0], &membuffer[arin * loptr[0]], loptr[0]);

  update_polled_stuff_if_runtime();

  free(membuffer-8);

  if (in->GetPosition() != uncompsiz)
    in->Seek(uncompsiz, kSeekBegin);

  update_polled_stuff_if_runtime();

  *dst_bmp = bmm;
}

void savecompressed_allegro(Stream *out, const Bitmap *bmpp, const RGB *pall)
{
  unsigned char *wgtbl = (unsigned char *)malloc(bmpp->GetWidth() * bmpp->GetHeight() + 4);
  short         *sss = (short *)wgtbl;

  sss[0] = bmpp->GetWidth();
  sss[1] = bmpp->GetHeight();

  memcpy(&wgtbl[4], bmpp->GetData(), bmpp->GetWidth() * bmpp->GetHeight());

  csavecompressed(out, wgtbl, pall);
  free(wgtbl);
}

void loadcompressed_allegro(Stream *in, Bitmap **bimpp, RGB *pall)
{
  short widd,hitt;
  int   ii;

  widd = in->ReadInt16();
  hitt = in->ReadInt16();
  Bitmap *bim = BitmapHelper::CreateBitmap(widd, hitt, 8);
  if (bim == nullptr)
    quit("!load_room: not enough memory to decompress masks");

  for (ii = 0; ii < hitt; ii++) {
    cunpackbitl(&bim->GetScanLineForWriting(ii)[0], widd, in);
    if (ii % 20 == 0)
      update_polled_stuff_if_runtime();
  }

  in->Seek(768);  // skip palette
  *bimpp = bim;
}

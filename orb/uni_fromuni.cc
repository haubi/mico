/*
 *  Code Set Converters for MICO
 *  Copyright (C) 1997 Marcus Mueller & Thomas Holubar
 *  Copyright (c) 1998-2006 by The Mico Team
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  For more information, visit the MICO Home Page at
 *  http://www.mico.org/
 *
 * 	date     vers.  name            reason
 *	97/07/01 0.1    mueller         CREATED
 */

#ifdef __COMO__
#pragma no_pch
#endif // __COMO__
#include <stdio.h>
#include <string.h>

#include "uni_unicode.h"
#include "uni_base64.h"
#include "uni_strtypes.h"
#include "uni_errors.h"
#include "uni_conversion.h"

#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#pragma warning (disable: 4305 4309 4244 4018)
// pragma warning (default: 4305 ) // turn on

#endif


static char FindException(const cexcept *array, uni_ubyte arrlen,
                          register uni_ushort unicode);

static const char uni2ebcArray[] =  /* beginning with 0x20 (32) */
{
  (char)0x40, (char)0x5A, (char)0x7E, (char)0x7B, (char)0x5B, (char)0x6C, (char)0x50, (char)0x7D,  /* 0x20-0x27 ( 32- 39) */
  (char)0x4D, (char)0x5D, (char)0x5C, (char)0x4E, (char)0x6B, (char)0x6D, (char)0x4B, (char)0x61,  /* 0x28-0x2F ( 40- 47) */
  (char)0xF0, (char)0xF1, (char)0xF2, (char)0xF3, (char)0xF4, (char)0xF5, (char)0xF6, (char)0xF7,  /* 0x30-0x37 ( 48- 55) */
  (char)0xF8, (char)0xF9, (char)0x7A, (char)0x5E, (char)0x4C, (char)0x7E, (char)0x6E, (char)0x6F,  /* 0x38-0x3F ( 56- 63) */
  (char)0x7C, (char)0xC1, (char)0xC2, (char)0xC3, (char)0xC4, (char)0xC5, (char)0xC6, (char)0xC7,  /* 0x40-0x47 ( 64- 71) */
  (char)0xC8, (char)0xC9, (char)0xD1, (char)0xD2, (char)0xD3, (char)0xD4, (char)0xD5, (char)0xD6,  /* 0x48-0x4F ( 72- 79) */
  (char)0xD7, (char)0xD8, (char)0xD9, (char)0xE2, (char)0xE3, (char)0xE4, (char)0xE5, (char)0xE6,  /* 0x50-0x57 ( 80- 87) */
  (char)0xE7, (char)0xE8, (char)0xE9, (char)'\0', (char)'\0', (char)'\0', (char)'\0', (char)0x6D,  /* 0x58-0x5F ( 88- 95) */
  (char)'\0', (char)0x81, (char)0x82, (char)0x83, (char)0x84, (char)0x85, (char)0x86, (char)0x87,  /* 0x60-0x67 ( 96-103) */
  (char)0x88, (char)0x89, (char)0x91, (char)0x92, (char)0x93, (char)0x94, (char)0x95, (char)0x96,  /* 0x68-0x6F (104-111) */
  (char)0x97, (char)0x98, (char)0x99, (char)0xA2, (char)0xA3, (char)0xA4, (char)0xA5, (char)0xA6,  /* 0x70-0x77 (112-119) */
  (char)0xA7, (char)0xA8, (char)0xA9, (char)0x8B, (char)0x6A, (char)0x9B, (char)'\0', (char)'\0'   /* 0x78-0x7F (120-127) */
};

//  static const cexcept exc2ebcArray[] =
//  {
//    { 0x00A2 , 0x4A } , { 0x00AC , 0x5F } , { 0x00DF , 0xE1 }
//  };

static const uni_ubyte uni2htmlArray[] =  /* beginning with 0xa0 (160) */
{
   69,  59,  43,  83,  /* 0xA0-0xA3 (160-163) */
   45, 102,  40,  87,  /* 0xA4-0xA7 (164-167) */
   99,  44,  76,  63,  /* 0xA8-0xAB (168-171) */
   71,  88,  86,  65,  /* 0xAC-0xAF (172-175) */

   46,  82,  90,  91,  /* 0xB0-0xB3 (176-179) */
   33,  67,  81,  68,  /* 0xB4-0xB7 (180-183) */
   42,  89,  77,  85,  /* 0xB8-0xBB (184-187) */
   54,  53,  55,  61,  /* 0xBC-0xBF (188-191) */

    4,   2,   3,   6,  /* 0xC0-0xC3 (192-195) */
    7,   5,   1,   8,  /* 0xC4-0xC7 (196-199) */
   12,  10,  11,  13,  /* 0xC8-0xCB (200-203) */
   16,  14,  15,  17,  /* 0xCC-0xCF (204-207) */

    9,  18,  21,  19,  /* 0xD0-0xD3 (208-211) */
   20,  23,  24,  94,  /* 0xD4-0xD7 (212-215) */
   22,  27,  25,  26,  /* 0xD8-0xDB (216-219) */
   28,  30,  29,  92,  /* 0xDC-0xDF (220-223) */

   35,  31,  32,  38,  /* 0xE0-0xE3 (224-227) */
   39,  37,  34,  41,  /* 0xE4-0xE7 (228-231) */
   50,  48,  49,  52,  /* 0xE8-0xEB (232-235) */
   60,  57,  58,  62,  /* 0xEC-0xEF (236-239) */

   51,  72,  75,  73,  /* 0xF0-0xF3 (240-243) */
   74,  79,  80,  47,  /* 0xF4-0xF7 (244-247) */
   78,  98,  96,  97,  /* 0xF8-0xFB (248-251) */
  100, 101,  93, 103   /* 0xFC-0xFF (252-255) */

  /* special characters : 0x22   (unicode) => "quot"  (HTML) ('"')
  **                      0x26   (unicode) => "amp"   (HTML) (&)
  **                      0x3C   (unicode) => "lt"    (HTML) (<)
  **                      0x3E   (unicode) => "gt"    (HTML) (>)
  **                      0x2122 (unicode) => "trade" (HTML) (tm)
  **                      0x2013 (unicode) => "ndash" (HTML) (en dash)
  **                      0x2014 (unicode) => "mdash" (HTML) (em dash) */
};

static const char uni2macArray[] =  /* beginning with 0xA0 (160) */
{
  (char)0xCA, (char)0xC1, (char)0xA2, (char)0xA3,  /* 0xA0-0xA3 (160-163) */
  (char)0xDB, (char)0xB4, (char)'\0', (char)0xA4,  /* 0xA4-0xA7 (164-167) */
  (char)0xAC, (char)0xA9, (char)0xAB, (char)0xC7,  /* 0xA8-0xAB (168-171) */
  (char)0xC2, (char)0xD0, (char)0xA8, (char)'\0',  /* 0xAC-0xAF (172-175) */

  (char)0xA1, (char)0xB1, (char)'\0', (char)'\0',  /* 0xB0-0xB3 (176-179) */
  (char)0xAB, (char)0xB5, (char)0xA6, (char)0xE1,  /* 0xB4-0xB7 (180-183) */
  (char)'\0', (char)0xF5, (char)0xAC, (char)0xC8,  /* 0xB8-0xBB (184-187) */
  (char)'\0', (char)'\0', (char)'\0', (char)0xC0,  /* 0xBC-0xBF (188-191) */

  (char)0xCB, (char)0xE7, (char)0xE5, (char)0xCC,  /* 0xC0-0xC3 (192-195) */
  (char)0x80, (char)0x81, (char)0xA6, (char)0x82,  /* 0xC4-0xC7 (196-199) */
  (char)0xE9, (char)0x83, (char)0xE6, (char)0xE8,  /* 0xC8-0xCB (200-203) */
  (char)0xCD, (char)0xCA, (char)0xCB, (char)0xCC,  /* 0xCC-0xCF (204-207) */

  (char)'\0', (char)0x84, (char)0xF1, (char)0xCE,  /* 0xD0-0xD3 (208-211) */
  (char)0xCF, (char)0xCD, (char)0x85, (char)'\0',  /* 0xD4-0xD7 (212-215) */
  (char)0xAF, (char)0xF5, (char)0xF3, (char)0xF4,  /* 0xD8-0xDB (216-219) */
  (char)0x86, (char)'\0', (char)'\0', (char)0xA7,  /* 0xDC-0xDF (220-223) */

  (char)0x88, (char)0x87, (char)0x89, (char)0x8B,  /* 0xE0-0xE3 (224-227) */
  (char)0x8A, (char)0x8C, (char)0xBE, (char)0x8D,  /* 0xE4-0xE7 (228-231) */
  (char)0x8F, (char)0x8E, (char)0x90, (char)0x91,  /* 0xE8-0xEB (232-235) */
  (char)0x93, (char)0x92, (char)0x94, (char)0x95,  /* 0xEC-0xEF (236-239) */

  (char)'\0', (char)0x96, (char)0x98, (char)0x97,  /* 0xF0-0xF3 (240-243) */
  (char)0x99, (char)0x9B, (char)0x9A, (char)0xD6,  /* 0xF4-0xF7 (244-247) */
  (char)0xBF, (char)0x9D, (char)0x9C, (char)0x9E,  /* 0xF8-0xFB (248-251) */
  (char)0x9F, (char)'\0', (char)'\0', (char)0xD8   /* 0xFC-0xFF (252-255) */
};

static const cexcept exc2macArray[] =
{
  { (uni_ushort)0x0152 , (char)0xCE } , { (uni_ushort)0x0153 , (char)0xCF } , { (uni_ushort)0x0178 , (char)0xD9 } ,
  { (uni_ushort)0x0192 , (char)0xC4 } , { (uni_ushort)0x02C6 , (char)0xF6 } , { (uni_ushort)0x02C7 , (char)0xF9 } ,
  { (uni_ushort)0x02C9 , (char)0xF8 } , { (uni_ushort)0x02CE , (char)0xFE } , { (uni_ushort)0x02D8 , (char)0xFF } ,
  { (uni_ushort)0x02D9 , (char)0xFA } , { (uni_ushort)0x02DA , (char)0xFB } , { (uni_ushort)0x02DB , (char)0xFC } ,
  { (uni_ushort)0x02DC , (char)0xF7 } , { (uni_ushort)0x03A9 , (char)0xBD } , { (uni_ushort)0x03C0 , (char)0xB9 } ,
  { (uni_ushort)0x2013 , (char)0xD0 } , { (uni_ushort)0x2014 , (char)0xD1 } , { (uni_ushort)0x2018 , (char)0xD4 } ,
  { (uni_ushort)0x2019 , (char)0xD5 } , { (uni_ushort)0x201A , (char)0xE2 } , { (uni_ushort)0x201C , (char)0xD2 } ,
  { (uni_ushort)0x201D , (char)0xD3 } , { (uni_ushort)0x201E , (char)0xE3 } , { (uni_ushort)0x2020 , (char)0xA0 } ,
  { (uni_ushort)0x2021 , (char)0xE0 } , { (uni_ushort)0x2026 , (char)0xC9 } , { (uni_ushort)0x2031 , (char)0xE4 } ,
  { (uni_ushort)0x2039 , (char)0xDC } , { (uni_ushort)0x20A0 , (char)0xDD } , { (uni_ushort)0x2122 , (char)0xAA } ,
  { (uni_ushort)0x2190 , (char)0x1C } , { (uni_ushort)0x2191 , (char)0x1E } , { (uni_ushort)0x2192 , (char)0x1D } ,
  { (uni_ushort)0x2193 , (char)0x1F } , { (uni_ushort)0x2202 , (char)0xB6 } , { (uni_ushort)0x2205 , (char)0xD7 } ,
  { (uni_ushort)0x2206 , (char)0xC6 } , { (uni_ushort)0x220E , (char)0xA5 } , { (uni_ushort)0x220F , (char)0xB8 } ,
  { (uni_ushort)0x2211 , (char)0xB7 } , { (uni_ushort)0x2215 , (char)0xDA } , { (uni_ushort)0x221A , (char)0xC3 } ,
  { (uni_ushort)0x221E , (char)0xB0 } , { (uni_ushort)0x222B , (char)0xBA } , { (uni_ushort)0x2248 , (char)0xC5 } ,
  { (uni_ushort)0x2260 , (char)0xAD } , { (uni_ushort)0x2264 , (char)0xB2 } , { (uni_ushort)0x2265 , (char)0xB3 } ,
  { (uni_ushort)0xFB01 , (char)0xDE } , { (uni_ushort)0xFB02 , (char)0xDF }
};

static const char uni2koiArray1[] =  /* beginning with 0x0410 (1040) */
{
  (char)0xE1, (char)0xE2, (char)0xF7, (char)0xE7,  /* 0x0410-0x0413 (1040-1043) */
  (char)0xE4, (char)0xE5, (char)0xF6, (char)0xFA,  /* 0x0414-0x0417 (1044-1047) */
  (char)0xE9, (char)0xEA, (char)0xEB, (char)0xEC,  /* 0x0418-0x041B (1048-1051) */
  (char)0xED, (char)0xEE, (char)0xEF, (char)0xF0,  /* 0x041C-0x041F (1042-1055) */

  (char)0xF2, (char)0xF3, (char)0xF4, (char)0xF5,  /* 0x0420-0x0423 (1056-1059) */
  (char)0xE6, (char)0xE8, (char)0xE3, (char)0xFE,  /* 0x0424-0x0427 (1060-1063) */
  (char)0xFB, (char)0xFD, (char)0xFF, (char)0xF9,  /* 0x0428-0x042B (1064-1067) */
  (char)0xF8, (char)0xFC, (char)0xE0, (char)0xF1,  /* 0x042C-0x042F (1068-1071) */

  (char)0xC1, (char)0xC2, (char)0xD7, (char)0xC7,  /* 0x0430-0x0433 (1072-1075) */
  (char)0xC4, (char)0xC5, (char)0xD6, (char)0xDA,  /* 0x0434-0x0437 (1076-1079) */
  (char)0xC9, (char)0xCA, (char)0xCB, (char)0xCC,  /* 0x0438-0x043B (1080-1083) */
  (char)0xCD, (char)0xCE, (char)0xCF, (char)0xD0,  /* 0x043C-0x043F (1084-1087) */

  (char)0xD2, (char)0xD3, (char)0xD4, (char)0xD5,  /* 0x0440-0x0443 (1088-1091) */
  (char)0xC6, (char)0xC8, (char)0xC3, (char)0xDE,  /* 0x0444-0x0447 (1092-1095) */
  (char)0xDB, (char)0xDD, (char)0xDF, (char)0xD9,  /* 0x0448-0x044B (1096-1099) */
  (char)0xD8, (char)0xDC, (char)0xC0, (char)0xD1   /* 0x044C-0x044F (1100-1103) */
};

static const char uni2koiArray2[] =  /* beginning with 0x2550 (9552) */
{
  (char)0xA0, (char)0xA1, (char)0xA2, (char)0xA4,  /* 0x0250-0x0253 (9552-9555) */
  (char)0xA5, (char)0xA6, (char)0xA7, (char)0xA8,  /* 0x0254-0x0257 (9556-9559) */
  (char)0xA9, (char)0xAA, (char)0xAB, (char)0xAC,  /* 0x0258-0x025B (9560-9563) */
  (char)0xAD, (char)0xAE, (char)0xAF, (char)0xB0,  /* 0x025C-0x025F (9564-9567) */

  (char)0xB1, (char)0xB2, (char)0xB4, (char)0xB5,  /* 0x0260-0x0263 (9568-9571) */
  (char)0xB6, (char)0xB7, (char)0xB8, (char)0xB9,  /* 0x0264-0x0267 (9572-9575) */
  (char)0xBA, (char)0xBB, (char)0xBC, (char)0xBD,  /* 0x0268-0x026B (9576-9579) */
  (char)0xBE                                       /* 0x026C        (9578     ) */
};

static const cexcept exc2koiArray[] =
{
  { 0xA0   , (char)0x9A } , { 0xA9   , (char)0xBF } , { 0xB0   , (char)0x9C } , 
  { 0xB2   , (char)0x9D } , { 0xB7   , (char)0x9E } , { 0xF7   , (char)0x9F } , 
  { 0x0401 , (char)0xB3 } , { 0x0451 , (char)0xA3 } , { 0x220E , (char)0x94 } , 
  { 0x2219 , (char)0x95 } , { 0x221A , (char)0x96 } , { 0x2248 , (char)0x97 } , 
  { 0x2264 , (char)0x98 } , { 0x2265 , (char)0x99 } , { 0x2320 , (char)0x93 } , 
  { 0x2321 , (char)0x9B } , { 0x2500 , (char)0x80 } , { 0x2502 , (char)0x81 } , 
  { 0x250C , (char)0x82 } , { 0x2510 , (char)0x83 } , { 0x2514 , (char)0x84 } , 
  { 0x2518 , (char)0x85 } , { 0x251C , (char)0x86 } , { 0x2524 , (char)0x87 } , 
  { 0x252C , (char)0x88 } , { 0x2534 , (char)0x89 } , { 0x253C , (char)0x8A } , 
  { 0x2580 , (char)0x8B } , { 0x2584 , (char)0x8C } , { 0x2588 , (char)0x8D } , 
  { 0x258C , (char)0x8E } , { 0x2590 , (char)0x8F } , { 0x2591 , (char)0x90 } , 
  { 0x2592 , (char)0x91 } , { 0x2593 , (char)0x92 }
};

static const char uni2ibm866Array1[] =  /* beginning with 0x0410 (1040) */
{
  (char)0x80, (char)0x81, (char)0x82, (char)0x83,  /* 0x0410-0x0413 (1040-1043) */
  (char)0x84, (char)0x85, (char)0x86, (char)0x87,  /* 0x0414-0x0417 (1044-1047) */
  (char)0x88, (char)0x89, (char)0x8A, (char)0x8B,  /* 0x0418-0x041B (1048-1051) */
  (char)0x8C, (char)0x8D, (char)0x8E, (char)0x8F,  /* 0x041C-0x041F (1042-1055) */

  (char)0x90, (char)0x91, (char)0x92, (char)0x93,  /* 0x0420-0x0423 (1056-1059) */
  (char)0x94, (char)0x95, (char)0x96, (char)0x97,  /* 0x0424-0x0427 (1060-1063) */
  (char)0x98, (char)0x99, (char)0x9A, (char)0x9B,  /* 0x0428-0x042B (1064-1067) */
  (char)0x9C, (char)0x9D, (char)0x9E, (char)0x9F,  /* 0x042C-0x042F (1068-1071) */

  (char)0xA0, (char)0xA1, (char)0xA2, (char)0xA3,  /* 0x0430-0x0433 (1072-1075) */
  (char)0xA4, (char)0xA5, (char)0xA6, (char)0xA7,  /* 0x0434-0x0437 (1076-1079) */
  (char)0xA8, (char)0xA9, (char)0xAA, (char)0xAB,  /* 0x0438-0x043B (1080-1083) */
  (char)0xAC, (char)0xAD, (char)0xAE, (char)0xAF,  /* 0x043C-0x043F (1084-1087) */

  (char)0xE0, (char)0xE1, (char)0xE2, (char)0xE3,  /* 0x0440-0x0443 (1088-1091) */
  (char)0xE4, (char)0xE5, (char)0xE6, (char)0xE7,  /* 0x0444-0x0447 (1092-1095) */
  (char)0xE8, (char)0xE9, (char)0xEA, (char)0xEB,  /* 0x0448-0x044B (1096-1099) */
  (char)0xEC, (char)0xED, (char)0xEE, (char)0xEF,  /* 0x044C-0x044F (1100-1103) */
};

static const char uni2ibm866Array2[] =  /* beginning with 0x2550 (9552) */
{
  (char)0xCD, (char)0xBA, (char)0xD5, (char)0xD6,  /* 0x2550-0x2553 (9552-9555) */
  (char)0xC9, (char)0xB8, (char)0xB7, (char)0xBB,  /* 0x2554-0x2557 (9556-9559) */
  (char)0xD4, (char)0xD3, (char)0xC8, (char)0xBE,  /* 0x2558-0x255B (9560-9563) */
  (char)0xBD, (char)0xBC, (char)0xC6, (char)0xC7,  /* 0x255C-0x255F (9564-9567) */

  (char)0xCC, (char)0xB5, (char)0xB6, (char)0xB9,  /* 0x2560-0x2563 (9568-9571) */
  (char)0xD1, (char)0xD2, (char)0xCB, (char)0xCF,  /* 0x2564-0x2567 (9572-9575) */
  (char)0xD0, (char)0xCA, (char)0xD8, (char)0xD7,  /* 0x2568-0x256B (9576-9579) */
  (char)0xCE,                                      /* 0x256C        (9578     ) */
};

static const cexcept exc2ibm866Array[] =
{
{ 0xA0   , (char)0xFF } , { 0xA4   , (char)0xFD } , { 0xB0   , (char)0xF8 } ,
{ 0xB7   , (char)0xFA } , { 0x0401 , (char)0xF0 } , { 0x0404 , (char)0xF2 } ,
{ 0x0407 , (char)0xF4 } , { 0x040E , (char)0xF6 } , { 0x0451 , (char)0xF1 } ,
{ 0x0454 , (char)0xF3 } , { 0x0457 , (char)0xF5 } , { 0x045E , (char)0xF7 } ,
{ 0x2116 , (char)0xFC } , { 0x2219 , (char)0xF9 } , { 0x221A , (char)0xFB } ,
{ 0x2500 , (char)0xC4 } , { 0x2502 , (char)0xB3 } , { 0x250C , (char)0xDA } ,
{ 0x2510 , (char)0xBF } , { 0x2514 , (char)0xC0 } , { 0x2518 , (char)0xD9 } ,
{ 0x251C , (char)0xC3 } , { 0x2524 , (char)0xB4 } , { 0x252C , (char)0xC2 } ,
{ 0x2534 , (char)0xC1 } , { 0x253C , (char)0xC5 } , { 0x2580 , (char)0xDF } ,
{ 0x2584 , (char)0xDC } , { 0x2588 , (char)0xDB } , { 0x258C , (char)0xDD } ,
{ 0x2590 , (char)0xDE } , { 0x2591 , (char)0xB0 } , { 0x2592 , (char)0xB1 } ,
{ 0x2593 , (char)0xB2 } , { 0x25A0 , (char)0xFE } ,
};

static const char uni2isoArray[] =  /* beginning with 0x0100 (256) */
{
  (char)0xC0, (char)0xE0, (char)0xC3, (char)0xE3,  /* 0x0100-0x0103 (256-259) */
  (char)0xA1, (char)0xB1, (char)0xC6, (char)0xE6,  /* 0x0104-0x0107 (260-263) */
  (char)0xC6, (char)0xE6, (char)0xC5, (char)0xE5,  /* 0x0108-0x010B (264-267) */
  (char)0xC8, (char)0xE8, (char)0xCF, (char)0xEF,  /* 0x010C-0x010F (268-271) */

  (char)0xD0, (char)0xF0, (char)0xAA, (char)0xBA,  /* 0x0110-0x0113 (272-275) */
  (char)'\0', (char)'\0', (char)0xCC, (char)0xEC,  /* 0x0114-0x0117 (276-279) */
  (char)0xCA, (char)0xEA, (char)0xCC, (char)0xEC,  /* 0x0118-0x011B (280-283) */
  (char)0xD8, (char)0xF8, (char)0xAB, (char)0xBB,  /* 0x011C-0x011F (284-287) */

  (char)0xD5, (char)0xF5, (char)0xAB, (char)0xBB,  /* 0x0120-0x0123 (288-291) */
  (char)0xA6, (char)0xB6, (char)0xA1, (char)0xB1,  /* 0x0124-0x0127 (292-295) */
  (char)0xA5, (char)0xB5, (char)0xCF, (char)0xEF,  /* 0x0128-0x012B (296-299) */
  (char)'\0', (char)'\0', (char)0xC7, (char)0xE7,  /* 0x012C-0x012F (300-302) */

  (char)0xA9, (char)0xB9, (char)'\0', (char)'\0',  /* 0x0130-0x0133 (304-307) */
  (char)0xAC, (char)0xBC, (char)0xD3, (char)0xF3,  /* 0x0134-0x0137 (308-311) */
  (char)0xA2, (char)0xC5, (char)0xE5, (char)0xA6,  /* 0x0138-0x013B (312-315) */
  (char)0xB6, (char)0xA5, (char)0xB5, (char)'\0',  /* 0x013C-0x013F (316-319) */

  (char)'\0', (char)0xA3, (char)0xB3, (char)0xD1,  /* 0x0140-0x0143 (320-323) */
  (char)0xF1, (char)0xD1, (char)0xF1, (char)0xD2,  /* 0x0144-0x0147 (324-327) */
  (char)0xF2, (char)'\0', (char)0xBD, (char)0xBF,  /* 0x0148-0x014B (328-331) */
  (char)0xD2, (char)0xF2, (char)'\0', (char)'\0',  /* 0x014C-0x014F (332-335) */

  (char)0xD5, (char)0xF5, (char)'\0', (char)'\0',  /* 0x0150-0x0153 (336-339) */
  (char)0xC0, (char)0xE0, (char)0xA3, (char)0xB3,  /* 0x0154-0x0157 (340-343) */
  (char)0xD8, (char)0xF8, (char)0xA6, (char)0xB6,  /* 0x0158-0x015B (344-347) */
  (char)0xDE, (char)0xFE, (char)0xAA, (char)0xBA,  /* 0x015C-0x015F (348-351) */

  (char)0xA9, (char)0xB9, (char)0xDE, (char)0xFE,  /* 0x0160-0x0163 (352-355) */
  (char)0xAB, (char)0xBB, (char)0xFC, (char)0xAD,  /* 0x0164-0x0167 (356-359) */
  (char)0xDD, (char)0xFD, (char)0xDE, (char)0xFE,  /* 0x0168-0x016B (360-363) */
  (char)0xDD, (char)0xFD, (char)0xD9, (char)0xF9,  /* 0x016C-0x016F (364-367) */

  (char)0xDB, (char)0xFB, (char)0xD9, (char)0xF9,  /* 0x0170-0x0173 (368-371) */
  (char)'\0', (char)'\0', (char)'\0', (char)'\0',  /* 0x0174-0x0177 (372-375) */
  (char)'\0', (char)0xAC, (char)0xBC, (char)0xAF,  /* 0x0178-0x017B (376-379) */
  (char)0xBF, (char)0xAE, (char)0xBE, (char)'\0'   /* 0x017C-0x017F (380-383) */
};

static const cexcept exc2iso2Array[] =
{
  { 0x02C7 , (char)0xB7 } , { 0x02D8 , (char)0xA2 } , { 0x02D9 , (char)0xFF } , 
  { 0x02DB , (char)0xB2 } , { 0x02DD , (char)0xBD } 
};

static const cexcept exc2iso3Array[] =
{
  { 0x02D8 , (char)0xA2 } , { 0x02D9 , (char)0xFF }
};

static const cexcept exc2iso4Array[] =
{
  { 0x02C7 , (char)0xB7 } , { 0x02C9 , (char)0xAF } , { 0x02D9 , (char)0xFF } ,
  { 0x02DB , (char)0xB2 }
};

static const cexcept exc2iso7Array[] =
{
  { 0x02BC , (char)0xA2 } , { 0x02BD , (char)0xA1 } , { 0x0385 , (char)0xB5 } ,
  { 0x0386 , (char)0xB6 } , { 0x0388 , (char)0xB8 } , { 0x0389 , (char)0xB9 } ,
  { 0x038A , (char)0xBA } , { 0x038C , (char)0xBC } , { 0x038E , (char)0xBE } ,
  { 0x038F , (char)0xBF }
};

static const cexcept exc2iso8Array[] =
{
  { 0x00D7 , (char)0xAA } , { 0x00E7 , (char)0xBA } , { 0x0333 , (char)0xDF }
};

static const cexcept exc2iso9Array[] =
{
  { 0x011E , (char)0xD0 } , { 0x011F , (char)0xF0 } , { 0x0130 , (char)0xDD } ,
  { 0x0131 , (char)0xFD } , { 0x015E , (char)0xDE } , { 0x015F , (char)0xFE }
};

static const cexcept exc2iso10Array[] =
{
  { 0x0110 , (char)0xA9 } , { 0x0111 , (char)0xB9 } , { 0x0112 , (char)0xA2 } ,
  { 0x0113 , (char)0xB2 } , { 0x0122 , (char)0xA3 } , { 0x0123 , (char)0xB3 } ,
  { 0x012A , (char)0xA4 } , { 0x012B , (char)0xB4 } , { 0x0130 , (char)0xBD } ,
  { 0x0136 , (char)0xA6 } , { 0x0137 , (char)0xB6 } , { 0x0138 , (char)0xFF } ,
  { 0x013B , (char)0xA8 } , { 0x013C , (char)0xA8 } , { 0x014A , (char)0xAF } ,
  { 0x015E , (char)0xBE } , { 0x0160 , (char)0xAA } , { 0x0161 , (char)0xBA } ,
  { 0x0166 , (char)0xAB } , { 0x0167 , (char)0xBB } , { 0x0168 , (char)0xD7 } ,
  { 0x0169 , (char)0xF7 } , { 0x016A , (char)0xAE } , { 0x016B , (char)0xBE } ,
  { 0x017D , (char)0xAC } , { 0x017E , (char)0xBC } , { 0x0335 , (char)0xAD } ,
  { 0x0336 , (char)0xBD }
};

static const char uni2ibmArray1[] =  /* beginning with 0xA0 (160) */
{
  (char)0xFF, (char)0xAD, (char)0x9B, (char)0x9C,  /* 0xA0-0xA3 (160-163) */
  (char)0xCF, (char)0x9D, (char)0xDD, (char)0x15,  /* 0xA4-0xA7 (164-167) */
  (char)0xF9, (char)0xB7, (char)0xA6, (char)0xAE,  /* 0xA8-0xAB (168-171) */
  (char)0xAA, (char)0xF0, (char)0xA9, (char)0xEE,  /* 0xAC-0xAF (172-175) */

  (char)0xF8, (char)0xF1, (char)0xFD, (char)0xFC,  /* 0xB0-0xB3 (176-179) */
  (char)0xEF, (char)0xE6, (char)0x14, (char)0xF9,  /* 0xB4-0xB7 (180-183) */
  (char)0xF7, (char)0xD5, (char)0xA7, (char)0xAF,  /* 0xB8-0xBB (184-187) */
  (char)0xAC, (char)0xAB, (char)0xF3, (char)0xA8,  /* 0xBC-0xBF (188-191) */

  (char)0xB7, (char)0xB5, (char)0xB6, (char)0xC7,  /* 0xC0-0xC3 (192-195) */
  (char)0x8E, (char)0x8F, (char)0x92, (char)0x80,  /* 0xC4-0xC7 (196-199) */
  (char)0xD4, (char)0x90, (char)0xD2, (char)0xD3,  /* 0xC8-0xCB (200-203) */
  (char)0xDE, (char)0xD6, (char)0xD7, (char)0xD8,  /* 0xCC-0xCF (204-207) */

  (char)0xD1, (char)0xA5, (char)0xE3, (char)0xE0,  /* 0xD0-0xD3 (208-211) */
  (char)0xE2, (char)0xE5, (char)0x99, (char)0x8E,  /* 0xD4-0xD7 (202-215) */
  (char)0x9D, (char)0xEB, (char)0xE9, (char)0xEA,  /* 0xD8-0xDB (216-219) */
  (char)0x9A, (char)0xED, (char)0xE8, (char)0xE1,  /* 0xDC-0xDF (220-223) */

  (char)0x85, (char)0xA0, (char)0x83, (char)0xC6,  /* 0xE0-0xE3 (224-227) */
  (char)0x84, (char)0x86, (char)0x91, (char)0x87,  /* 0xE4-0xE7 (228-231) */
  (char)0x8A, (char)0x82, (char)0x88, (char)0x89,  /* 0xE8-0xEB (232-235) */
  (char)0x8D, (char)0xA1, (char)0x8C, (char)0x8B,  /* 0xEC-0xEF (236-239) */

  (char)0xD0, (char)0xA4, (char)0x95, (char)0xA2,  /* 0xF0-0xF3 (240-243) */
  (char)0x93, (char)0xE4, (char)0x94, (char)0xF6,  /* 0xF4-0xF7 (244-247) */
  (char)0x9B, (char)0x97, (char)0xA3, (char)0x96,  /* 0xF8-0xFB (248-251) */
  (char)0x81, (char)0xEC, (char)0xE7, (char)0x98,  /* 0xFC-0xFF (252-255) */

  (char)'\0', (char)'\0', (char)0xC6, (char)0xC7,  /* 0x0100-0x0103 (256-259) */
  (char)0xA4, (char)0xA5, (char)0x8F, (char)0x86,  /* 0x0104-0x0107 (260-263) */
  (char)'\0', (char)'\0', (char)'\0', (char)'\0',  /* 0x0108-0x010B (264-267) */
  (char)0xAC, (char)0x9F, (char)0xD2, (char)0xD4,  /* 0x010C-0x010F (268-271) */

  (char)0xD1, (char)0xD0, (char)'\0', (char)'\0',  /* 0x0110-0x0113 (272-275) */
  (char)'\0', (char)'\0', (char)'\0', (char)'\0',  /* 0x0114-0x0117 (276-279) */
  (char)0xA8, (char)0xA9, (char)0xB7, (char)0xD8,  /* 0x0118-0x011B (280-283) */
  (char)'\0', (char)'\0', (char)'\0', (char)'\0',  /* 0x011C-0x011F (284-287) */

  (char)'\0', (char)'\0', (char)'\0', (char)'\0',  /* 0x0120-0x0123 (288-291) */
  (char)'\0', (char)'\0', (char)'\0', (char)'\0',  /* 0x0124-0x0127 (292-295) */
  (char)'\0', (char)'\0', (char)'\0', (char)'\0',  /* 0x0128-0x012B (296-299) */
  (char)'\0', (char)'\0', (char)'\0', (char)'\0',  /* 0x012C-0x012F (300-302) */

  (char)'\0', (char)'\0', (char)'\0', (char)'\0',  /* 0x0130-0x0133 (304-307) */
  (char)'\0', (char)'\0', (char)'\0', (char)'\0',  /* 0x0134-0x0137 (308-311) */
  (char)'\0', (char)0x91, (char)0x92, (char)'\0',  /* 0x0138-0x013B (312-315) */
  (char)'\0', (char)0x95, (char)0x96, (char)'\0',  /* 0x013C-0x013F (316-319) */

  (char)'\0', (char)0x9D, (char)0x88, (char)0xE5,  /* 0x0140-0x0143 (320-323) */
  (char)0xE4, (char)'\0', (char)'\0', (char)0xD5,  /* 0x0144-0x0147 (324-327) */
  (char)0xE5, (char)'\0', (char)'\0', (char)'\0',  /* 0x0148-0x014B (328-331) */
  (char)'\0', (char)'\0', (char)'\0', (char)'\0',  /* 0x014C-0x014F (332-335) */

  (char)0x8A, (char)0x8B, (char)'\0', (char)'\0',  /* 0x0150-0x0153 (336-339) */
  (char)0xE8, (char)0xEA, (char)'\0', (char)'\0',  /* 0x0154-0x0157 (340-343) */
  (char)0xFC, (char)0xFD, (char)0x97, (char)0x98,  /* 0x0158-0x015B (344-347) */
  (char)'\0', (char)'\0', (char)0xB8, (char)0xAD,  /* 0x015C-0x015F (348-351) */

  (char)0xE6, (char)0xE7, (char)0xDD, (char)0xEE,  /* 0x0160-0x0163 (352-355) */
  (char)0x9B, (char)0x9C, (char)'\0', (char)'\0',  /* 0x0164-0x0167 (356-359) */
  (char)'\0', (char)'\0', (char)'\0', (char)'\0',  /* 0x0168-0x016B (360-363) */
  (char)'\0', (char)'\0', (char)0xDE, (char)0x85,  /* 0x016C-0x016F (364-367) */

  (char)0xEB, (char)0xFB, (char)'\0', (char)'\0',  /* 0x0170-0x0173 (368-371) */
  (char)'\0', (char)'\0', (char)'\0', (char)'\0',  /* 0x0174-0x0177 (372-375) */
  (char)'\0', (char)0x8D, (char)0xAB, (char)0xBD,  /* 0x0178-0x017B (376-379) */
  (char)0xBE, (char)0xA6, (char)0xA7, (char)'\0'   /* 0x017C-0x017F (380-383) */
};

static const char uni2ibmArray2[] =  /* beginning with 0x2550 (9552) */
{
  (char)0xCD, (char)0xBA, (char)0xD5, (char)0xD6,  /* 0x0250-0x0253 (9552-9555) */
  (char)0xC9, (char)0xB8, (char)0xB7, (char)0xBB,  /* 0x0254-0x0257 (9556-9559) */
  (char)0xD4, (char)0xD3, (char)0xC8, (char)0xBE,  /* 0x0258-0x025B (9560-9563) */
  (char)0xBD, (char)0xBC, (char)0xC6, (char)0xC7,  /* 0x025C-0x025F (9564-9567) */

  (char)0xCC, (char)0xB5, (char)0xB6, (char)0xB9,  /* 0x0260-0x0263 (9568-9571) */
  (char)0xD1, (char)0xD2, (char)0xCB, (char)0xCF,  /* 0x0264-0x0267 (9572-9575) */
  (char)0xD0, (char)0xCA, (char)0xD8, (char)0xD7,  /* 0x0268-0x026B (9576-9579) */
  (char)0xCE                                       /* 0x026C        (9578     ) */
};

static const cexcept exc2ibm437Array[] =
{
  { 0x0192 , (char)0x9F } , { 0x0393 , (char)0xE2 } , { 0x0398 , (char)0xE9 } ,
  { 0x03A0 , (char)0xE3 } , { 0x03A3 , (char)0xE4 } , { 0x03A6 , (char)0xE8 } ,
  { 0x03A9 , (char)0xEA } , { 0x03B1 , (char)0xE0 } , { 0x03B4 , (char)0xEB } ,
  { 0x03B7 , (char)0xFC } , { 0x03C3 , (char)0xE5 } , { 0x03D2 , (char)0xE7 } ,
  { 0x20C3 , (char)0x13 } , { 0x2191 , (char)0x18 } , { 0x2192 , (char)0x1A } ,
  { 0x2193 , (char)0x19 } , { 0x2194 , (char)0x1D } , { 0x2195 , (char)0x12 } ,
  { 0x21A8 , (char)0x17 } , { 0x2205 , (char)0xED } , { 0x2208 , (char)0xEE } ,
  { 0x220E , (char)0xFE } , { 0x2219 , (char)0xFA } , { 0x221A , (char)0xFB } ,
  { 0x221E , (char)0xEC } , { 0x2229 , (char)0xEF } , { 0x2248 , (char)0xF7 } ,
  { 0x2261 , (char)0xF0 } , { 0x2264 , (char)0xF3 } , { 0x2265 , (char)0xF2 } ,
  { 0x2310 , (char)0xA9 } , { 0x2319 , (char)0x1C } , { 0x2320 , (char)0xF4 } ,
  { 0x2321 , (char)0xF5 } , { 0x2500 , (char)0xC4 } , { 0x2502 , (char)0xB3 } ,
  { 0x250C , (char)0xDA } , { 0x2510 , (char)0xBF } , { 0x2514 , (char)0xC0 } ,
  { 0x2518 , (char)0xD9 } , { 0x251C , (char)0xC3 } , { 0x2524 , (char)0xB4 } ,
  { 0x252C , (char)0xC2 } , { 0x2534 , (char)0xC1 } , { 0x253C , (char)0xC5 } ,
  { 0x2580 , (char)0xDF } , { 0x2584 , (char)0xDC } , { 0x2588 , (char)0xDB } ,
  { 0x258C , (char)0xDD } , { 0x2590 , (char)0xDE } , { 0x2591 , (char)0xB0 } ,
  { 0x2592 , (char)0xB1 } , { 0x2593 , (char)0xB2 } , { 0x25B2 , (char)0x1E } ,
  { 0x25B6 , (char)0x10 } , { 0x25BC , (char)0x1F } , { 0x25C0 , (char)0x11 } ,
  { 0x263A , (char)0x01 } , { 0x263B , (char)0x02 } , { 0x2660 , (char)0x06 } ,
  { 0x2663 , (char)0x05 } , { 0x2665 , (char)0x03 } , { 0x2666 , (char)0x04 }
};

static const cexcept exc2ibm850Array[] =
{
  { 0x0192 , (char)0x9F } , { 0x20C3 , (char)0x13 } , { 0x2191 , (char)0x18 } ,
  { 0x2192 , (char)0x1A } , { 0x2193 , (char)0x19 } , { 0x2194 , (char)0x1D } ,
  { 0x2195 , (char)0x12 } , { 0x21A8 , (char)0x17 } , { 0x220E , (char)0xFE } ,
  { 0x2219 , (char)0xFA } , { 0x2319 , (char)0x1C } , { 0x2500 , (char)0xC4 } ,
  { 0x2502 , (char)0xB3 } , { 0x250C , (char)0xDA } , { 0x2510 , (char)0xBF } ,
  { 0x2514 , (char)0xC0 } , { 0x2518 , (char)0xD9 } , { 0x251C , (char)0xC3 } ,
  { 0x252C , (char)0xC2 } , { 0x2534 , (char)0xC1 } , { 0x253C , (char)0xC5 } ,
  { 0x2580 , (char)0xDF } , { 0x2584 , (char)0xDC } , { 0x2588 , (char)0xDB } ,
  { 0x2591 , (char)0xB0 } , { 0x2592 , (char)0xB1 } , { 0x2593 , (char)0xB2 } ,
  { 0x25B2 , (char)0x1E } , { 0x25B6 , (char)0x10 } , { 0x25BC , (char)0x1F } ,
  { 0x25C0 , (char)0x11 } , { 0x263A , (char)0x01 } , { 0x263B , (char)0x02 } ,
  { 0x2660 , (char)0x06 } , { 0x2663 , (char)0x05 } , { 0x2665 , (char)0x03 } ,
  { 0x2666 , (char)0x04 }
};

static const cexcept exc2ibm852Array[] =
{
  { 0x02C7 , (char)0xF3 } , { 0x02D8 , (char)0xF4 } , { 0x02DB , (char)0xF2 } ,
  { 0x02DD , (char)0xF1 } , { 0x20C3 , (char)0x13 } , { 0x2191 , (char)0x18 } ,
  { 0x2192 , (char)0x1A } , { 0x2193 , (char)0x19 } , { 0x2194 , (char)0x1D } ,
  { 0x2195 , (char)0x12 } , { 0x21A8 , (char)0x17 } , { 0x220E , (char)0xFE } ,
  { 0x2219 , (char)0xFA } , { 0x2319 , (char)0x1C } , { 0x2500 , (char)0xC4 } ,
  { 0x2502 , (char)0xB3 } , { 0x250C , (char)0xDA } , { 0x2510 , (char)0xBF } ,
  { 0x2514 , (char)0xC0 } , { 0x2518 , (char)0xD9 } , { 0x251C , (char)0xC3 } ,
  { 0x252C , (char)0xC2 } , { 0x2534 , (char)0xC1 } , { 0x253C , (char)0xC5 } ,
  { 0x2580 , (char)0xDF } , { 0x2584 , (char)0xDC } , { 0x2588 , (char)0xDB } ,
  { 0x2591 , (char)0xB0 } , { 0x2592 , (char)0xB1 } , { 0x2593 , (char)0xB2 } ,
  { 0x25B2 , (char)0x1E } , { 0x25B6 , (char)0x10 } , { 0x25BC , (char)0x1F } ,
  { 0x25C0 , (char)0x11 } , { 0x263A , (char)0x01 } , { 0x263B , (char)0x02 } ,
  { 0x2660 , (char)0x06 } , { 0x2663 , (char)0x05 } , { 0x2665 , (char)0x03 } ,
  { 0x2666 , (char)0x04 }
};

static const cexcept exc2ibm860Array[] =
{
  { 0xC0 , (char)0x91 } , { 0xC1 , (char)0x86 } , { 0xC2 , (char)0x8F }, { 0xC3 , (char)0x8E } ,
  { 0xC8 , (char)0x92 } , { 0xC9 , (char)0x86 } , { 0xCA , (char)0x89 }, { 0xCC , (char)0x98 } ,
  { 0xCD , (char)0x8B } , { 0xD2 , (char)0xA9 } , { 0xD3 , (char)0x9F }, { 0xD4 , (char)0x8C } ,
  { 0xD5 , (char)0x99 } , { 0xD9 , (char)0x9D } , { 0xDA , (char)0x96 }, { 0xE3 , (char)0x84 } ,
  { 0xF5 , (char)0x94 }
};

static const cexcept exc2ibm863Array[] =
{
  { 0xA4 , (char)0x98 } , { 0xA6 , (char)0xA0 } , { 0xA8 , (char)0xA4 }, { 0xAF , (char)0xA7 } ,
  { 0xB3 , (char)0xA6 } , { 0xB4 , (char)0xA1 } , { 0xB8 , (char)0xA5 }, { 0xBE , (char)0xAD } ,
  { 0xC0 , (char)0x8E } , { 0xC2 , (char)0x84 } , { 0xC8 , (char)0x91 }, { 0xCA , (char)0x92 } ,
  { 0xCB , (char)0x94 } , { 0xCE , (char)0xA8 } , { 0xCF , (char)0x95 }, { 0xD4 , (char)0x99 } ,
  { 0xD9 , (char)0x9D } , { 0xDB , (char)0x9E } , { 0xFA , (char)0x97 }
};

static const cexcept exc2ibm865Array[] =
{
  { 0xA4 , (char)0xAF } , { 0xFA , (char)0x97 }
};

static const cexcept exc2winArray[] =
{
  { 0x0118 , (char)0x9F } , { 0x0152 , (char)0x8C } , { 0x0153 , (char)0x9C } ,
  { 0x0160 , (char)0x8A } , { 0x0161 , (char)0x9A } , { 0x0192 , (char)0x83 } ,
  { 0x02C6 , (char)0x88 } , { 0x02DC , (char)0x98 } , { 0x2013 , (char)0x96 } ,
  { 0x2014 , (char)0x97 } , { 0x2018 , (char)0x91 } , { 0x2019 , (char)0x92 } ,
  { 0x201A , (char)0x82 } , { 0x201C , (char)0x93 } , { 0x201D , (char)0x94 } ,
  { 0x201E , (char)0x84 } , { 0x2020 , (char)0x86 } , { 0x2021 , (char)0x87 } ,
  { 0x2026 , (char)0x85 } , { 0x2031 , (char)0x89 } , { 0x2039 , (char)0x8B } ,
  { 0x20A0 , (char)0x9B } , { 0x2122 , (char)0x99 } , { 0x220F , (char)0x95 }
};


static const cfromunitable uni2ebcTable =
  { uni2ebcArray, 0x0020 /*   32 */, sizeof(uni2ebcArray),
    0x00000000 };

static const cfromunitable uni2macTable =
  { uni2macArray, 0x00A0 /*  160 */, sizeof(uni2macArray),
    0xF0000000 };

static const cfromunitable uni2koiTable1 =
  { uni2koiArray1, 0x0410 /* 1040 */, sizeof(uni2koiArray1),
    0x00000000 };

static const cfromunitable uni2koiTable2 =
  { uni2koiArray2, 0x2550 /* 9552 */, sizeof(uni2koiArray2),
    0x00000000 };

static const cfromunitable uni2ibm866Table1 =
  { uni2ibm866Array1, 0x0410 /* 1040 */, sizeof(uni2ibm866Array1),
    0x00000000 };

static const cfromunitable uni2ibm866Table2 =
  { uni2ibm866Array2, 0x2550 /* 9552 */, sizeof(uni2ibm866Array2),
    0x00000000 };

static const cfromunitable uni2isoTable =
  { uni2isoArray, 0x0100 /*  256 */, sizeof(uni2isoArray),
    0x00000000 };

static const cfromunitable uni2ibmTable1 =
  { uni2ibmArray1, 0x00A0 /*  160 */, sizeof(uni2ibmArray1), 0xF7FF007E };

static const cfromunitable uni2ibmTable2 =
  { uni2ibmArray2, 0x2550 /* 9552 */, sizeof(uni2ibmArray2), 0xF7FF007E };


//  static const cexcepttable exc2ebcTable =
//    { exc2ebcArray, 3 };

static const cexcepttable exc2macTable =
  { exc2macArray, 50 };

static const cexcepttable exc2koiTable =
  { exc2koiArray, 35 };

static const cexcepttable exc2ibm866Table =
  { exc2ibm866Array, 35 };

static const cexcepttable exc2iso2Table =
  { exc2iso2Array, 5 };

static const cexcepttable exc2iso3Table =
  { exc2iso3Array, 3 };

static const cexcepttable exc2iso4Table =
  { exc2iso4Array, 4 };

static const cexcepttable exc2iso7Table =
  { exc2iso7Array, 10 };

static const cexcepttable exc2iso8Table =
  { exc2iso8Array, 3 };

static const cexcepttable exc2iso9Table =
  { exc2iso9Array, 6 };

static const cexcepttable exc2iso10Table =
  { exc2iso10Array, 28 };

static const cexcepttable exc2ibm437Table =
  { exc2ibm437Array, 63 };

static const cexcepttable exc2ibm850Table =
  { exc2ibm850Array, 37 };

static const cexcepttable exc2ibm852Table =
  { exc2ibm852Array, 37 };

static const cexcepttable exc2ibm860Table1 =
  { &exc2ibm437Array[1], 62 };

static const cexcepttable exc2ibm860Table2 =
  { exc2ibm860Array, 17 };

static const cexcepttable exc2ibm863Table1 =
  { &exc2ibm437Array[1], 62 };

static const cexcepttable exc2ibm863Table2 =
  { exc2ibm863Array, 19 };

static const cexcepttable exc2ibm865Table1 =
  { &exc2ibm437Array[1], 62 };

static const cexcepttable exc2ibm865Table2 =
  { exc2ibm865Array, 2 };

static const cexcepttable exc2winTable =
  { exc2winArray, 24 };


static const uni_ushort uni2htmlOffset   = 0x00A0; /* ( 160) */
static const uni_ubyte uni2htmlSize   = sizeof(uni2htmlArray);

static const bits32 iso7Mask = 0x289FBBC9;
static const bits32 iso8Mask = 0x7BFFFBFD;
       /* for uni=>iso8859_x translation; for region 0xA0-0xB0;
       ** if bit+1 is set, 0xA0+i will be translated to 0xA0+i;
       ** otherwise 0xA0+i cannot be translated (bit 0 is the 
       ** lowest one)  */ 

static char FindException(const cexcept *array, uni_ubyte arrlen,
                          register uni_ushort unicode)
{
  register uni_ubyte min, max, mid;
  if(array==NULL) return('\0');

  min = 0;
  max = arrlen-1;
  if(unicode <  array[min].unicode) return(0l);
  if(unicode == array[min].unicode) return(array[min].code);

  if(unicode >  array[max].unicode) return(0l);
  if(unicode == array[max].unicode) return(array[max].code);

  while(min+1 < max)
  {
    mid = (min+max)/2;
    if( unicode < array[mid].unicode )
      max = mid;
    else if( unicode > array[mid].unicode )
      min = mid;
    else /*  unicode == array[mid].unicode  */
      return(array[mid].code);
  }

  return('\0');
}


static uni_ulong
fromCRLF(const char *utf8, uni_uword *si, uni_uword chars, uni_ubyte line_type)
{
  switch(utf8[*si])
  {
    case '\r' :
     if((*si)+1 < chars)
     {
       if( (utf8[(*si)+1] == '\n') && (line_type == C_LINE_LF) )
       { (*si)++; return((uni_ulong)'\n'); }

       if( (utf8[(*si)+1] == '\n') && (line_type == C_LINE_CR) )
       { (*si)++; return((uni_ulong)'\r'); }
     }
     return((uni_ulong)'\r');

    case '\n' :
     if((*si)+1 < chars)
     {
       if( (utf8[(*si)+1] == '\r') && (line_type == C_LINE_LF) )
       { (*si)++; return((uni_ulong)'\n'); }

       if( (utf8[(*si)+1] == '\r') && (line_type == C_LINE_CR) )
       { (*si)++; return((uni_ulong)'\r'); }
     }
     return((uni_ulong)'\n');
  }

  return(utf8[*si]);
}

uni_slong uni_fromUTF8(char *dest, const char *utf8, uni_uword *chars, uni_uword *utf_read,
		   uni_ulong string_type, uni_ubyte line_type,
		   uni_uword *written)
{
  // const int somechars = (chars!=0) ? 1 : 0;  
  uni_ulong   utf7array[C_BASE64_DATA+1];
  uni_ubyte   pos=0;
  uni_ushort *utf16;
  uni_ulong  *ucs4, character;
  uni_slong    RC;
  uni_uword   ci, si, di;
  char    buffer[20], *html;
  int    iscommand, isquot, isunderline, trans;

  const cfromunitable *table1, *table2;
  const ctounitable   *check, *ibmcheck;
  const cexcepttable  *except, *ibmexcept;
  uni_ulong unknown;

  if( (string_type==C_UTF8) || (string_type==(uni_ulong)C_ASCII7) ||
      (string_type==C_ISO646) )
  {
    for(ci=si=di=0; ci < (*chars); si++)
      switch(utf8[si])
      {
        case '\0' :
          dest[di] = '\0';
          *chars = ci;
          *utf_read = si+1;
	  *written = di+1;
        return(C_OK);

        case '\r': case '\n' :
          dest[di++] = (char)fromCRLF(utf8,&si,*chars,line_type);
        break;

        default:
          dest[di++] = utf8[si];
          if( ((utf8[si] & 0x80) == 0) || ((utf8[si] & 0xC0) == 0x80) )
            ci++;

          if(string_type==(uni_ulong)C_ASCII7 ||
	     string_type==C_ISO646)
            if((utf8[si] & 0x80) != 0)
              if((utf8[si] & 0x40) != 0)
                di--;
              else
                dest[di-1] = C_ASCII7_UNKNOWN;
      }

    dest[di] = '\0';
    *utf_read = si;
    *written = di+1;
    return(C_OK);
  }

  if(string_type==(uni_ulong)C_UTF7)
  {
    dest[0] = '\0';

    for(ci=si=di=0; ci < *(chars); ci++)
      switch(utf8[si])
      {
        case '\0' :
          if(di!=0)
          {
            RC = uni_ucs4arraytoutf7
            (
              &dest[strlen(dest)],
              utf7array,
              di
            );
            if(RC!=C_OK) return(RC);
          }
          *chars = ci;
          *utf_read = si+1;
	  *written = strlen (dest) + 1;
        return(C_OK);

        case '\r': case '\n' :
          utf7array[di++] = fromCRLF(utf8,&si,*chars,line_type);
        break;

        default:
          if( (utf8[si] & 0x80) == 0 )
            utf7array[di] = (uni_ulong)utf8[si];
          else
          {
            RC = uni_utf8toucs4(&utf7array[di], &utf8[si], &pos);
            if(RC != C_OK) return(RC);
            si += pos;
          }
  
          di++;
          if(di == C_BASE64_DATA)
          {
            RC = uni_ucs4arraytoutf7
            (
              &dest[strlen(dest)],
              utf7array,
              di
            );

            if(RC!=C_OK) return(RC);
            di=0;
          }
      }

    *utf_read = si;
    *written = strlen (dest) + 1;
    return(C_OK);
  }

  if(string_type==C_UTF16 || string_type==C_UCS2)
  {
    utf16 = (uni_ushort*)dest;
    for(ci=si=di=0; si < (*chars); si++, ci++, di++)
    {
      // XXX added cast
      if((uni_uchar)utf8[si] < 0x80)
        switch(utf8[si])
        {
          case '\0' :
            *chars = ci;
            *utf_read = si+1;
          goto END_UTF16;

          case '\r': case '\n' :
            utf16[di] = (uni_ushort)fromCRLF(utf8,&si,*chars,line_type);
          break;

          default:
            utf16[di] = utf8[si];
        }
      else
      {
        RC = uni_utf8toutf16(&utf16[di], &utf8[si], &pos);
        if(RC != C_OK) return(RC);
        si += pos-1;

        if (utf16[di+1] != 0l) di++;
      }
    }

    *utf_read = si;

END_UTF16:
    *written = di;
    if(di>0)
    {
      if(utf16[di-1] != 0l) {
	utf16[di] = 0l;
	*written += 1;
      }
    }
    else
      utf16[0] = 0l;

    return(C_OK);
  }
  
  if(string_type==C_UCS4)
  {
    ucs4 = (uni_ulong*)dest;
    for(si=di=0; si < (*chars); si++, di++)
    {
      // XXX added cast
      if((uni_uchar)utf8[si] < 0x80)
        switch(utf8[si])
        {
          case '\0' :
            *chars = di;
            *utf_read = si+1;
          goto END_UCS4;

          case '\r': case '\n' :
            ucs4[di] = fromCRLF(utf8,&si,*chars,line_type);
          break;

          default:
            ucs4[di] = utf8[si];
        }
      else
      {
        RC = uni_utf8toucs4(ucs4, utf8, &pos);
        if(RC != C_OK) return(RC);
        si += pos-1;
      }
    }

    *utf_read = si;

END_UCS4:
    *written = di;
    if(di>0)
    {
      if(ucs4[di-1] != 0l)
      {
	ucs4[di] = 0l;
	*written += 1;
      }
    }
    else
      ucs4[0] = 0l;

    return(C_OK);
  }

  if(string_type==(uni_ulong)C_HTML3)
  {
    iscommand=isquot=isunderline=false;

    for(ci=si=di=0; si < (*chars); ci++)
    {
      if( (utf8[si] & 0x80) == 0 )
      {
        switch(utf8[si])
        {
          case '\0' :
            dest[di] = '\0';
            *chars = ci;
            *utf_read = si+1;
	    *written = di+1;
          return(C_OK);

          case '\n' : case '\r' :
            dest[di++] = (char)fromCRLF(utf8,&si,*chars,line_type);
          break;

          case '"' :
            if(iscommand)
            {
              dest[di++] = '"';
              isquot = !isquot;
            }
            else
            {
              strcpy(&dest[di], "&quot;");
              di += 6;
            }
          break;

          case '&' :
            if(iscommand)
              dest[di++] = '&';
            else
            {
              strcpy(&dest[di], "&amp;");
              di += 5;
            }
          break;

          case '<' :
            if(iscommand)
              dest[di++] = '<';
            else
            {
              strcpy(&dest[di], "&lt;");
              di += 4;
            }
          break;

          case '>' :
            if(iscommand)
            {
              dest[di++] = '>';
              if(!isquot) iscommand=false;
            }
            else
            {
              strcpy(&dest[di], "&gt;");
              di += 4;
            }
          break;

          case '\033' /* ESC */ :
            if(iscommand)
              dest[di++] = '\033';
            else
            {
              if(utf8[si+1] == '<')
              {
                iscommand=true;
                if( (utf8[si+1] == '_') && (utf8[si+2] != '\033') )
                {
                  isunderline=true;
                  strcpy(&dest[di],"<U>");
                  di+=3;
                  si++;
                }
              }
            }
          break;

          default :
            dest[di++] = utf8[si];
        }

        si++;
      }
      else if(iscommand)
      {
        dest[di] = utf8[si];
        di++; si++;
      }
      else
      {
        RC = uni_utf8toucs4(&character, &utf8[si], &pos);
        if(RC != C_OK) return(RC);
        si+=pos;

        switch(character)
        {
          case 0x2122 :
            strcpy(&dest[di], "&trade;");
            di += 7;
          break;

          case 0x2013 :
            strcpy(&dest[di], "&ndash;");
            di += 7;
          break;

          case 0x2014 :
            strcpy(&dest[di], "&mdash;");
            di += 7;
          break;

          default :
            if( (character >= uni2htmlOffset) &&
                (character <  uni2htmlOffset+uni2htmlSize) )
            {
              pos = uni2htmlArray[ character-uni2htmlOffset ];
              dest[di++] = '&';
              html = (char*)&uni_html2uniArray[pos-1].upper;
#ifdef C_LITTLE_ENDIAN
              if(html[3] != ' ') dest[di++] = html[3]; else goto cont;
              if(html[2] != ' ') dest[di++] = html[2]; else goto cont;
              if(html[1] != ' ') dest[di++] = html[1]; else goto cont;
              if(html[0] != ' ') dest[di++] = html[0]; else goto cont;
              html = (char*)&uni_html2uniArray[pos-1].lower;
              if(html[3] != ' ') dest[di++] = html[3]; else goto cont;
              if(html[2] != ' ') dest[di++] = html[2]; else goto cont;
              if(html[1] != ' ') dest[di++] = html[1]; else goto cont;
              if(html[0] != ' ') dest[di++] = html[0]; else goto cont;
#else
              if(html[0] != ' ') dest[di++] = html[0]; else goto cont;
              if(html[1] != ' ') dest[di++] = html[1]; else goto cont;
              if(html[2] != ' ') dest[di++] = html[2]; else goto cont;
              if(html[3] != ' ') dest[di++] = html[3]; else goto cont;
              html = (char*)&uni_html2uniArray[pos-1].lower;
              if(html[0] != ' ') dest[di++] = html[0]; else goto cont;
              if(html[1] != ' ') dest[di++] = html[1]; else goto cont;
              if(html[2] != ' ') dest[di++] = html[2]; else goto cont;
              if(html[3] != ' ') dest[di++] = html[3]; else goto cont;
#endif
cont:         dest[di++] = ';';
            }
            else
            {
              sprintf(buffer, "&#%lu;", character);
              strcpy(&dest[di], buffer);
              di += strlen(buffer);
            }
          break;
        }
      }

      if(isunderline && (utf8[si-1] != '_'))
      {
        strcpy(&dest[di],"</U>");
        di += 4;
        isunderline=false;
      }
    }

    dest[di] = '\0';
    *utf_read = si;
    *written = di+1;
    return(C_OK);
  }

  if( (string_type==C_ISO8859_1) || (string_type==C_ISO8859_5) ||
      (string_type==C_ISO8859_6) || (string_type==C_ISO8859_7) ||
      (string_type==C_ISO8859_8) || (string_type==C_ISO8859_9) ||
      (string_type==(uni_ulong)C_WIN31_LATIN1) )
  {
    for(ci=si=di=0; si < (*chars); ci++, di++)
    {
      if( (utf8[si] & 0x80) == 0)
      {
        switch(utf8[si])
        {
          case '\0' :
            dest[di] = '\0';
            *chars = ci;
            *utf_read = si+1;
	    *written = di+1;
          return(C_OK);

          case '\n' : case '\r' :
            dest[di] = (char)fromCRLF(utf8,&si,*chars,line_type);
          break;

          default:
            dest[di] = utf8[si];
        }

        si++;
      }
      else
      {
        RC = uni_utf8toucs4(&character, &utf8[si], &pos);
        if(RC != C_OK) return(RC);
        si+=pos;

        switch(string_type)
        {
          case C_ISO8859_1 :
            if(character <= 0xFF)
              dest[di] = (char)character;
            else
              dest[di] = C_ISO8859_1_UNKNOWN;
          break;

          case C_ISO8859_5 :
            if((character >= 0x0400) && (character <= 0x045F))
              if( (character != 0x040D) && (character != 0x045E) )
                dest[di] = (char)(character - 0x0400 + 0xA0);
              else dest[di] = C_ISO8859_6_UNKNOWN;
            else switch(character)
            {
              case 0xAD : dest[di] = 0xAD;
              case 0xA7 : dest[di] = 0xFE;
              default   : dest[di] = C_ISO8859_5_UNKNOWN;
            }
          break;

          case C_ISO8859_6 :
            if((character >= 0x0600) && (character <= 0x065F))
              if( (character != 0x0604) && (character != 0x060D) )
                dest[di] = (char)(character - 0x0600 + 0xA0);
              else dest[di] = C_ISO8859_6_UNKNOWN;
            else switch(character)
            {
              case 0xA4 : dest[di] = 0xA4;
              case 0xAD : dest[di] = 0xAD;
              default   : dest[di] = C_ISO8859_6_UNKNOWN;
            }
          break;

          case C_ISO8859_7 :
            if((character >= 0x0390) && (character <= 0x03CF))
              dest[di] = (char)(character - 0x0390 + 0xC0);
            else
            {
              if( (character >= 0x00A0) && (character <= 0x00BF) )
                if( (( 1 << (character - 0x00A0) ) & iso7Mask) != 0 )
                  dest[di] = (char)character;
                else
                  dest[di] = C_ISO8859_7_UNKNOWN;
              else
              {
                dest[di] = FindException(exc2iso7Table.array,
                  exc2iso7Table.size, character);
                if(dest[di] == '\0') dest[di] = C_ISO8859_7_UNKNOWN;
              }
            }
          break;

          case C_ISO8859_8 :
            if((character >= 0x05D0) && (character <= 0x05EF))
              dest[di] = (char)(character - 0x05D0 + 0xE0);
            else
            {
              if( (character >= 0x00A0) && (character <= 0x00BF) )
                if( (( 1 << (character - 0x00A0) ) & iso8Mask) != 0 )
                  dest[di] = (char)character;
                else
                  dest[di] = C_ISO8859_8_UNKNOWN;
              else
              {
                dest[di] = FindException(exc2iso8Table.array,
                  exc2iso8Table.size, character);
                if(dest[di] == '\0') dest[di] = C_ISO8859_8_UNKNOWN;
              }
            }
          break;

          case C_ISO8859_9 :
            if(character <= 0xFF)
              switch(character)
              {
                case 0xD0: case 0xF0: case 0xDD: case 0xFD: case 0xDE:
                case 0xFE: dest[di] = C_ISO8859_9_UNKNOWN; break;
                default : dest[di] = (char)character;
              }
            else
            {
              dest[di] = FindException(exc2iso9Table.array,
                exc2iso9Table.size, character);
              if(dest[di] == '\0') dest[di] = C_ISO8859_9_UNKNOWN;
            }
          break;

          case (uni_ulong)C_WIN31_LATIN1 :
            trans=false;

            if(character <= 0x81)
              dest[di] = (char)character;
            else if(character <= 0x9F)
              if( ( (character >= 0x8D) && (character <= 0x90) ) ||
                    (character == 0x9D) || (character == 0x9E) )
                dest[di] = C_WIN31_LATIN1_UNKNOWN;
              else trans=true;
            else if(character <= 0xFF)
              dest[di] = (char)character;
            else trans=true;

            if(trans)
            {
              dest[di] = FindException(exc2winTable.array,
                exc2winTable.size, character);
              if(dest[di] == '\0') dest[di] = C_WIN31_LATIN1_UNKNOWN;
            }
          break;

        }
      }
    }

    dest[di] = '\0';
    *utf_read = si;
    *written = di+1;
    return(C_OK);
  }


  table1=table2=NULL;
  check=ibmcheck=NULL;
  except=ibmexcept=NULL;

  switch(string_type)
  {
    case (uni_ulong)C_EBCDIC:
      table1      = &uni2ebcTable;
      unknown     = C_EBCDIC_UNKNOWN;
    break;

    case (uni_ulong)C_MACINTOSH:
      table1      = &uni2macTable;
      unknown     = C_MACINTOSH_UNKNOWN;
      except      = &exc2macTable;
    break;

    case (uni_ulong)C_KOI8_R:
      table1      = &uni2koiTable1;
      table2      = &uni2koiTable2;
      unknown     = C_KOI8_R_UNKNOWN;
      except      = &exc2koiTable;
    break;

    case C_ISO8859_2:
      table1      = &uni2isoTable;
      unknown     = C_ISO8859_2_UNKNOWN;
      except      = &exc2iso2Table;
      check       = &uni_iso22uniTable;
    break;

    case C_ISO8859_3:
      table1      = &uni2isoTable;
      unknown     = C_ISO8859_3_UNKNOWN;
      except      = &exc2iso3Table;
      check       = &uni_iso32uniTable;
    break;

    case C_ISO8859_4:
      table1      = &uni2isoTable;
      unknown     = C_ISO8859_4_UNKNOWN;
      except      = &exc2iso4Table;
      check       = &uni_iso42uniTable;
    break;

    case C_ISO8859_10:
      table1      = &uni2isoTable;
      unknown     = C_ISO8859_10_UNKNOWN;
      except      = &exc2iso10Table;
      check       = &uni_iso102uniTable;
    break;

    case C_IBM_437:
      table1      = &uni2ibmTable1;
      table2      = &uni2ibmTable2;
      unknown     = C_IBM_437_UNKNOWN;
      except      = &exc2ibm437Table;
      check       = &uni_ibm4372uniTable;
    break;

    case C_IBM_850:
      table1      = &uni2ibmTable1;
      table2      = &uni2ibmTable2;
      unknown     = C_IBM_850_UNKNOWN;
      except      = &exc2ibm850Table;
      check       = &uni_ibm8502uniTable;
    break;

    case C_IBM_852:
      table1      = &uni2ibmTable1;
      table2      = &uni2ibmTable2;
      unknown     = C_IBM_852_UNKNOWN;
      except      = &exc2ibm852Table;
      check       = &uni_ibm8522uniTable;
    break;

    case C_IBM_860:
      table1      = &uni2ibmTable1;
      table2      = &uni2ibmTable2;
      unknown     = C_IBM_860_UNKNOWN;
      except      = &exc2ibm860Table1;
      ibmexcept   = &exc2ibm860Table2;
      check       = &uni_ibm8602uniTable;
      ibmcheck    = &uni_ibm4372uniTable;
    break;

    case C_IBM_863:
      table1      = &uni2ibmTable1;
      table2      = &uni2ibmTable2;
      unknown     = C_IBM_863_UNKNOWN;
      except      = &exc2ibm863Table1;
      ibmexcept   = &exc2ibm863Table2;
      check       = &uni_ibm8632uniTable;
      ibmcheck    = &uni_ibm4372uniTable;
    break;

    case C_IBM_865:
      table1      = &uni2ibmTable1;
      table2      = &uni2ibmTable2;
      unknown     = C_IBM_865_UNKNOWN;
      except      = &exc2ibm865Table1;
      ibmexcept   = &exc2ibm865Table2;
      check       = &uni_ibm8652uniTable;
      ibmcheck    = &uni_ibm4372uniTable;
    break;

    case C_IBM_866:
      table1      = &uni2ibm866Table1;
      table2      = &uni2ibm866Table2;
      unknown     = C_IBM_866_UNKNOWN;
      except      = &exc2ibm866Table;
    break;

    default: return(C_UNKNOWN_FORMAT);
  }


  for(ci=si=di=0; si < (*chars); ci++, di++)
  {
    if( (utf8[si] & 0x80) == 0 )
    {
      character = (uni_ubyte)utf8[si];
      if( character < 32)
      {
        trans = ( (table1->transMask & (1 << character)) != 0 );
        switch(utf8[si])
        {
          case '\0' :
            dest[di] = '\0';
            *chars = ci;
            *utf_read = si+1;
	    *written = di+1;
          return(C_OK);

          case '\n' : case '\r' :
            character = fromCRLF(utf8,&si,*chars,line_type);
          break;
        }
      }
      else
        trans = false;

      si++;
    }
    else
    {
      RC = uni_utf8toucs4(&character, &utf8[si], &pos);
      if(RC != C_OK) return(RC);
      si+=pos;
      trans=true;
    }

    if(trans)
    {
      if( (check!=NULL) && (character <= 0xFF) )
        if( (character >= check->offset) &&
            ((uni_slong)character <  check->offset + check->size) )
          if( check->array[ character - check->offset ] == character )
          {
            dest[di] = (char)character;
            trans=false;
          }

      if( trans && (character >= table1->offset) && 
                   ((uni_slong)character < table1->offset + table1->size) )
      {
        dest[di] = table1->array[character - table1->offset];
        if(check!=NULL)
          if( ((uni_ubyte)dest[di] >= check->offset) &&
              ((uni_ubyte)dest[di] <  check->offset + check->size) )
            if(check->array[ (uni_ubyte)dest[di] - check->offset ] == character)
              trans=false;
            else dest[di]='\0';
          else if(ibmcheck!=NULL)
            if( ((uni_ubyte)dest[di] >= ibmcheck->offset) &&
                ((uni_ubyte)dest[di] <  ibmcheck->offset + ibmcheck->size) )
              if(ibmcheck->array[ (uni_ubyte)dest[di] - ibmcheck->offset ]
                              == character)
                trans=false;
              else dest[di]='\0';
            else trans=false;
          else trans=false;
        else trans=false;
      }

      if( trans && (table2!=NULL) )
        if( (character >= table2->offset) && 
            ((uni_slong)character <  table2->offset + table2->size) )
        {
          dest[di] = table2->array[character - table2->offset];
          if(check!=NULL)
            if( ((uni_ubyte)dest[di] >= check->offset) &&
                ((uni_ubyte)dest[di] <  check->offset + check->size) )
              if(check->array[ (uni_ubyte)dest[di] - check->offset ] == character)
                trans=false;
              else dest[di]='\0';
            else trans=false;
          else trans=false;
        }

      if(trans)
      {
        dest[di] = FindException(except->array, except->size, character);
        if( (ibmexcept!=NULL) && (dest[di] == '\0') &&
            (character <= 0xFF) )
          dest[di] = FindException(ibmexcept->array, ibmexcept->size,
                                   character);
      }

      if(dest[di] == '\0') dest[di] = unknown;
    }
    else dest[di] = (char)character;
  }

  dest[di] = '\0';
  *utf_read = si;
  *written = di+1;
  return(C_OK);
}

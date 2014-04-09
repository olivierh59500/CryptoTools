/* $Id: SKIPJACK.C 1.1 1999/06/27 02:53:47 tjones Rel $ */

/* Skipjack - A portable 80-bit block cipher designed by NSA.
   Copyright (C) 1998  Thobias Jones.  All rights reserved.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this program; if not, write to the
   Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA  02111-1307, USA.
*/

/*
   Skipjack was declassified by the NSA on 23 June 1998.
   Skipjack can be found at:
      http://csrc.nist.gov/encryption/skipjack-kea.htm

   Revisions:
         6/10/99 - Modified code to work with new directives in header file.
                   Made pointers in function headers const correct.
          6/4/99 - Added URL to NIST web site with Skipjack specification.
                   Added comments at end of functions.
                   Turned Ftable into a const table.
                   Put blank lines between code and data definitions.
        12/23/98 - Repackaged and re-released under GPL.
         7/11/98 - Code built and checked against test vectors.
*/

#include "skipjack.h"
#include "endian.h"

const unsigned char Ftable[];

void SJ_Encrypt(unsigned char *text, const unsigned char *key)
{
   int counter;

   for(counter = 1; counter <= ITER_PER_FUNC * 1; ++counter)
      RuleA((word *)text, key, counter);
   for(; counter <= ITER_PER_FUNC * 2; ++counter)
      RuleB((word *)text, key, counter);
   for(; counter <= ITER_PER_FUNC * 3; ++counter)
      RuleA((word *)text, key, counter);
   for(; counter <= ITER_PER_FUNC * 4; ++counter)
      RuleB((word *)text, key, counter);
} /* Encrypt */

void SJ_Decrypt(unsigned char *text, const unsigned char *key)
{
   int counter;

   for(counter = ROUNDS; counter > ITER_PER_FUNC * 3; --counter)
      RuleB_1((word *)text, key, counter);
   for(; counter > ITER_PER_FUNC * 2; --counter)
      RuleA_1((word *)text, key, counter);
   for(; counter > ITER_PER_FUNC * 1; --counter)
      RuleB_1((word *)text, key, counter);
   for(; counter > ITER_PER_FUNC * 0; --counter)
      RuleA_1((word *)text, key, counter);
} /* Decrypt */

void RuleA(word *w, const unsigned char *key, word counter)
{
   word temp;

   temp = G(w[0], key, counter - 1);
   w[0] = temp ^ w[3] ^ lswap16(counter);
   w[3] = w[2];
   w[2] = w[1];
   w[1] = temp;
} /* RuleA */

void RuleB(word *w, const unsigned char *key, word counter)
{
   word temp;

   temp = w[2];
   w[2] = w[0] ^ w[1] ^ lswap16(counter);
   w[1] = G(w[0], key, counter - 1);
   w[0] = w[3];
   w[3] = temp;
} /* RuleB */

void RuleA_1(word *w, const unsigned char *key, word counter)
{
   word temp;

   temp = w[0] ^ w[1] ^ lswap16(counter);
   w[0] = G_1(w[1], key, counter - 1);
   w[1] = w[2];
   w[2] = w[3];
   w[3] = temp;
} /* RuleA_1 */

void RuleB_1(word *w, const unsigned char *key, word counter)
{
   word temp;

   temp = G_1(w[1], key, counter - 1);
   w[1] = temp ^ w[2] ^ lswap16(counter);
   w[2] = w[3];
   w[3] = w[0];
   w[0] = temp;
} /* RuleB_1 */

/* G Permutation */
word G(word g, const unsigned char *key, int step)
{
   unsigned char g1, g2, g3, g4;

   step <<= (NUM_FEISTELS >> 1);
   g1 = (lswap16(g) >> 8) & 0xff;
   g2 = lswap16(g) & 0xff;
   g3 = (g1 ^ Ftable[(g2 ^ key[step % KEYLENGTH])]);
   g4 = (g2 ^ Ftable[(g3 ^ key[(step + 1) % KEYLENGTH])]);
   g1 = (g3 ^ Ftable[(g4 ^ key[(step + 2) % KEYLENGTH])]);
   g2 = (g4 ^ Ftable[(g1 ^ key[(step + 3) % KEYLENGTH])]);
   return lswap16(((word)g1 << 8) + g2);
} /* G */

/* G^(-1) Permutation */
word G_1(word g, const unsigned char *key, int step)
{
   unsigned char g1, g2, g3, g4;

   step <<= (NUM_FEISTELS >> 1);
   g1 = (lswap16(g) >> 8) & 0xff;
   g2 = lswap16(g) & 0xff;
   g3 = (g2 ^ Ftable[(g1 ^ key[(step + 3) % KEYLENGTH])]);
   g4 = (g1 ^ Ftable[(g3 ^ key[(step + 2) % KEYLENGTH])]);
   g2 = (g3 ^ Ftable[(g4 ^ key[(step + 1) % KEYLENGTH])]);
   g1 = (g4 ^ Ftable[(g2 ^ key[step % KEYLENGTH])]);
   return lswap16(((word)g1 << 8) + g2);
} /* G_1 */

const unsigned char Ftable[] = {
0xa3, 0xd7, 0x09, 0x83, 0xf8, 0x48, 0xf6, 0xf4, 0xb3, 0x21, 0x15, 0x78,
0x99, 0xb1, 0xaf, 0xf9, 0xe7, 0x2d, 0x4d, 0x8a, 0xce, 0x4c, 0xca, 0x2e,
0x52, 0x95, 0xd9, 0x1e, 0x4e, 0x38, 0x44, 0x28, 0x0a, 0xdf, 0x02, 0xa0,
0x17, 0xf1, 0x60, 0x68, 0x12, 0xb7, 0x7a, 0xc3, 0xe9, 0xfa, 0x3d, 0x53,
0x96, 0x84, 0x6b, 0xba, 0xf2, 0x63, 0x9a, 0x19, 0x7c, 0xae, 0xe5, 0xf5,
0xf7, 0x16, 0x6a, 0xa2, 0x39, 0xb6, 0x7b, 0x0f, 0xc1, 0x93, 0x81, 0x1b,
0xee, 0xb4, 0x1a, 0xea, 0xd0, 0x91, 0x2f, 0xb8, 0x55, 0xb9, 0xda, 0x85,
0x3f, 0x41, 0xbf, 0xe0, 0x5a, 0x58, 0x80, 0x5f, 0x66, 0x0b, 0xd8, 0x90,
0x35, 0xd5, 0xc0, 0xa7, 0x33, 0x06, 0x65, 0x69, 0x45, 0x00, 0x94, 0x56,
0x6d, 0x98, 0x9b, 0x76, 0x97, 0xfc, 0xb2, 0xc2, 0xb0, 0xfe, 0xdb, 0x20,
0xe1, 0xeb, 0xd6, 0xe4, 0xdd, 0x47, 0x4a, 0x1d, 0x42, 0xed, 0x9e, 0x6e,
0x49, 0x3c, 0xcd, 0x43, 0x27, 0xd2, 0x07, 0xd4, 0xde, 0xc7, 0x67, 0x18,
0x89, 0xcb, 0x30, 0x1f, 0x8d, 0xc6, 0x8f, 0xaa, 0xc8, 0x74, 0xdc, 0xc9,
0x5d, 0x5c, 0x31, 0xa4, 0x70, 0x88, 0x61, 0x2c, 0x9f, 0x0d, 0x2b, 0x87,
0x50, 0x82, 0x54, 0x64, 0x26, 0x7d, 0x03, 0x40, 0x34, 0x4b, 0x1c, 0x73,
0xd1, 0xc4, 0xfd, 0x3b, 0xcc, 0xfb, 0x7f, 0xab, 0xe6, 0x3e, 0x5b, 0xa5,
0xad, 0x04, 0x23, 0x9c, 0x14, 0x51, 0x22, 0xf0, 0x29, 0x79, 0x71, 0x7e,
0xff, 0x8c, 0x0e, 0xe2, 0x0c, 0xef, 0xbc, 0x72, 0x75, 0x6f, 0x37, 0xa1,
0xec, 0xd3, 0x8e, 0x62, 0x8b, 0x86, 0x10, 0xe8, 0x08, 0x77, 0x11, 0xbe,
0x92, 0x4f, 0x24, 0xc5, 0x32, 0x36, 0x9d, 0xcf, 0xf3, 0xa6, 0xbb, 0xac,
0x5e, 0x6c, 0xa9, 0x13, 0x57, 0x25, 0xb5, 0xe3, 0xbd, 0xa8, 0x3a, 0x01,
0x05, 0x59, 0x2a, 0x46 };

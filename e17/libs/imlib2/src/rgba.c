#include "common.h"
#include <X11/Xlib.h>
#include "context.h"
#include "rgba.h"
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#define IS_ALIGNED_64(val) (!((val) & 0x7))
#define IS_ALIGNED_32(val) (!((val) & 0x3))
#define IS_ALIGNED_16(val) (!((val) & 0x1))

#define IS_MULTIPLE_2(val) (!((val) & 0x1))
#define IS_MULTIPLE_4(val) (!((val) & 0x3))

/* for PPC / Motorola / SPARC, not x86, ALPHA */
/* dont uncomment this - i have this here for my own testing */
/*#define WORDS_BIGENDIAN*/
/* for data in ABGR memory model */

/* NOTES: */
/* x86: RGBA in byte order = ABGR when read as an int (in register/int) */

/* lookup table to see what color index to use */
static DATA8 *_dither_color_lut;
static DATA8 _pal_type;

/* using DATA32 - major speedup for aligned memory reads */

/* these data structs global rather than context-based for speed */
static DATA16 *_dither_r16;
static DATA16 *_dither_g16;
static DATA16 *_dither_b16;
static DATA8 *_dither_r8;
static DATA8 *_dither_g8;
static DATA8 *_dither_b8;
static DATA8 *_dither_666r;
static DATA8 *_dither_666g;
static DATA8 *_dither_666b;
static int dither_a_init = 0;
static DATA8 _dither_a1[8 * 8 * 256];

/*****************************************************************************/
/* Actual rendering routines                                                 */
/* RGBA -> RGB565                                                            */
/*****************************************************************************/

/*****************************************************************************/
/* MACROS for plain RGBA -> RGB565 conversion */
#define WRITE1_RGBA_RGB565(src, dest)        \
*dest = ((*src >> 8) & 0xf800) |           \
        ((*src >> 5) & 0x7e0) |            \
        ((*src >> 3) & 0x1f); dest++; src++
#ifdef WORDS_BIGENDIAN
#define WRITE2_RGBA_RGB565(src, dest)                   \
{                                                       \
 *((DATA32 *)dest) = ((src[1] >> 8) & 0xf800) |         \
                     ((src[1] >> 5) & 0x7e0) |          \
                     ((src[1] >> 3) & 0x1f) |           \
                     ((src[0] << 8) & 0xf8000000) |     \
                     ((src[0] << 11) & 0x7e00000) |     \
                     ((src[0] << 13) & 0x1f0000);       \
                     dest += 2; src += 2;               \
}
#else
#define WRITE2_RGBA_RGB565(src, dest)                    \
{                                                        \
 *((DATA32 *)dest) = ((src[0] >> 8) & 0xf800) |          \
                     ((src[0] >> 5) & 0x7e0) |           \
                     ((src[0] >> 3) & 0x1f) |            \
                     ((src[1] << 8) & 0xf8000000) |      \
                     ((src[1] << 11) & 0x7e00000) |      \
                     ((src[1] << 13) & 0x1f0000);        \
                     dest += 2; src += 2;                \
}
#endif
/*****************************************************************************/
/* MACROS for dithered RGBA -> RGB565 conversion */
#define DITHER_RGBA_565_LUT_R(num) \
(_dither_r16[(((x + num) & 0x3) << 10) | ((y & 0x3) << 8) | ((src[num] >> 16 ) & 0xff)])
#define DITHER_RGBA_565_LUT_G(num) \
(_dither_g16[(((x + num) & 0x3) << 10) | ((y & 0x3) << 8) | ((src[num] >> 8 ) & 0xff)])
#define DITHER_RGBA_565_LUT_B(num) \
(_dither_b16[(((x + num) & 0x3) << 10) | ((y & 0x3) << 8) | ((src[num] >> 0 ) & 0xff)])

#define WRITE1_RGBA_RGB565_DITHER(src, dest)                  \
*dest = (DITHER_RGBA_565_LUT_R(0)) |                          \
        (DITHER_RGBA_565_LUT_G(0)) |                          \
        (DITHER_RGBA_565_LUT_B(0)); dest++; src++
#ifdef WORDS_BIGENDIAN
#define WRITE2_RGBA_RGB565_DITHER(src, dest)                  \
{                                                             \
 *((DATA32 *)dest) = ((DITHER_RGBA_565_LUT_R(1))) |           \
                     ((DITHER_RGBA_565_LUT_G(1))) |           \
                     ((DITHER_RGBA_565_LUT_B(1))) |           \
                     ((DITHER_RGBA_565_LUT_R(0) << 16)) |     \
                     ((DITHER_RGBA_565_LUT_G(0) << 16)) |     \
                     ((DITHER_RGBA_565_LUT_B(0) << 16));      \
                     dest += 2; src += 2;                     \
}
#else
#define WRITE2_RGBA_RGB565_DITHER(src, dest)                  \
{                                                             \
 *((DATA32 *)dest) = ((DITHER_RGBA_565_LUT_R(0))) |           \
                     ((DITHER_RGBA_565_LUT_G(0))) |           \
                     ((DITHER_RGBA_565_LUT_B(0))) |           \
                     ((DITHER_RGBA_565_LUT_R(1) << 16)) |     \
                     ((DITHER_RGBA_565_LUT_G(1) << 16)) |     \
                     ((DITHER_RGBA_565_LUT_B(1) << 16));      \
                     dest += 2; src += 2;                     \
}
#endif
/*****************************************************************************/
/* MACROS for plain RGBA -> BGR565 conversion */
#define WRITE1_RGBA_BGR565(src, dest)        \
*dest = ((*src << 8) & 0xf800) |           \
        ((*src >> 5) & 0x7e0) |            \
        ((*src >> 19)& 0x1f); dest++; src++
#ifdef WORDS_BIGENDIAN
#define WRITE2_RGBA_BGR565(src, dest)                   \
{                                                       \
 *((DATA32 *)dest) = ((src[1] << 8) & 0xf800) |         \
                     ((src[1] >> 5) & 0x7e0) |          \
                     ((src[1] >> 19)& 0x1f) |           \
                     ((src[0] << 24) & 0xf8000000) |     \
                     ((src[0] << 11) & 0x7e00000) |     \
                     ((src[0] >>  3) & 0x1f0000);       \
                     dest += 2; src += 2;               \
}
#else
#define WRITE2_RGBA_BGR565(src, dest)                    \
{                                                        \
 *((DATA32 *)dest) = ((src[0] << 8) & 0xf800) |          \
                     ((src[0] >> 5) & 0x7e0) |           \
                     ((src[0] >> 19)& 0x1f) |            \
                     ((src[1] << 24) & 0xf8000000) |      \
                     ((src[1] << 11) & 0x7e00000) |      \
                     ((src[1] >>  3) & 0x1f0000);        \
                     dest += 2; src += 2;                \
}
#endif
/*****************************************************************************/
/* MACROS for dithered RGBA -> BGR565 conversion */
#define DITHER_RGBA_BGR565_LUT_R(num) \
(_dither_r16[(((x + num) & 0x3) << 10) | ((y & 0x3) << 8) | ((src[num] >> 0 ) & 0xff)])
#define DITHER_RGBA_BGR565_LUT_G(num) \
(_dither_g16[(((x + num) & 0x3) << 10) | ((y & 0x3) << 8) | ((src[num] >> 8 ) & 0xff)])
#define DITHER_RGBA_BGR565_LUT_B(num) \
(_dither_b16[(((x + num) & 0x3) << 10) | ((y & 0x3) << 8) | ((src[num] >> 16) & 0xff)])

#define WRITE1_RGBA_BGR565_DITHER(src, dest)                  \
*dest = (DITHER_RGBA_BGR565_LUT_R(0)) |                       \
        (DITHER_RGBA_BGR565_LUT_G(0)) |                       \
        (DITHER_RGBA_BGR565_LUT_B(0)); dest++; src++
#ifdef WORDS_BIGENDIAN
#define WRITE2_RGBA_BGR565_DITHER(src, dest)                  \
{                                                             \
 *((DATA32 *)dest) = ((DITHER_RGBA_BGR565_LUT_R(1))) |        \
                     ((DITHER_RGBA_BGR565_LUT_G(1))) |        \
                     ((DITHER_RGBA_BGR565_LUT_B(1))) |        \
                     ((DITHER_RGBA_BGR565_LUT_R(0) << 16)) |  \
                     ((DITHER_RGBA_BGR565_LUT_G(0) << 16)) |  \
                     ((DITHER_RGBA_BGR565_LUT_B(0) << 16));   \
                     dest += 2; src += 2;                     \
}
#else
#define WRITE2_RGBA_BGR565_DITHER(src, dest)                  \
{                                                             \
 *((DATA32 *)dest) = ((DITHER_RGBA_BGR565_LUT_R(0))) |        \
                     ((DITHER_RGBA_BGR565_LUT_G(0))) |        \
                     ((DITHER_RGBA_BGR565_LUT_B(0))) |        \
                     ((DITHER_RGBA_BGR565_LUT_R(1) << 16)) |  \
                     ((DITHER_RGBA_BGR565_LUT_G(1) << 16)) |  \
                     ((DITHER_RGBA_BGR565_LUT_B(1) << 16));   \
                     dest += 2; src += 2;                     \
}
#endif

/*****************************************************************************/
/* Actual rendering routines                                                 */
/* RGBA -> RGB555                                                            */
/*****************************************************************************/

/*****************************************************************************/
/* MACROS for plain RGBA -> RGB555 conversion */
#define WRITE1_RGBA_RGB555(src, dest)        \
*dest = ((*src >> 9) & 0x7c00) |           \
        ((*src >> 6) & 0x3e0) |            \
        ((*src >> 3) & 0x1f); dest++; src++
#ifdef WORDS_BIGENDIAN
#define WRITE2_RGBA_RGB555(src, dest)                   \
{                                                       \
 *((DATA32 *)dest) = ((src[1] >> 9) & 0x7c00) |         \
                     ((src[1] >> 6) & 0x3e0) |          \
                     ((src[1] >> 3) & 0x1f) |          \
                     ((src[0] << 7) & 0x7c000000) |    \
                     ((src[0] << 10) & 0x3e00000) |     \
                     ((src[0] << 13) & 0x1f0000);        \
                     dest += 2; src += 2;               \
}
#else
#define WRITE2_RGBA_RGB555(src, dest)                    \
{                                                        \
 *((DATA32 *)dest) = ((src[0] >> 9) & 0x7c00) |          \
                     ((src[0] >> 6) & 0x3e0) |           \
                     ((src[0] >> 3) & 0x1f) |           \
                     ((src[1] << 7) & 0x7c000000) |     \
                     ((src[1] << 10) & 0x3e00000) |      \
                     ((src[1] << 13) & 0x1f0000);         \
                     dest += 2; src += 2;                \
}
#endif
/*****************************************************************************/
/* MACROS for dithered RGBA -> RGB555 conversion */
#define DITHER_RGBA_555_LUT_R(num) \
(_dither_r16[(((x + num) & 0x3) << 10) | ((y & 0x3) << 8) | ((src[num] >> 16 ) & 0xff)])
#define DITHER_RGBA_555_LUT_G(num) \
(_dither_g16[(((x + num) & 0x3) << 10) | ((y & 0x3) << 8) | ((src[num] >> 8 ) & 0xff)])
#define DITHER_RGBA_555_LUT_B(num) \
(_dither_b16[(((x + num) & 0x3) << 10) | ((y & 0x3) << 8) | ((src[num] >> 0 ) & 0xff)])

#define WRITE1_RGBA_RGB555_DITHER(src, dest)                  \
*dest = (DITHER_RGBA_555_LUT_R(0)) |                          \
        (DITHER_RGBA_555_LUT_G(0)) |                          \
        (DITHER_RGBA_555_LUT_B(0)); dest++; src++
#ifdef WORDS_BIGENDIAN
#define WRITE2_RGBA_RGB555_DITHER(src, dest)                  \
{                                                             \
 *((DATA32 *)dest) = ((DITHER_RGBA_555_LUT_R(1))) |           \
                     ((DITHER_RGBA_555_LUT_G(1))) |           \
                     ((DITHER_RGBA_555_LUT_B(1))) |           \
                     ((DITHER_RGBA_555_LUT_R(0) << 16)) |     \
                     ((DITHER_RGBA_555_LUT_G(0) << 16)) |     \
                     ((DITHER_RGBA_555_LUT_B(0) << 16));      \
                     dest += 2; src += 2;                     \
}
#else
#define WRITE2_RGBA_RGB555_DITHER(src, dest)                  \
{                                                             \
 *((DATA32 *)dest) = ((DITHER_RGBA_555_LUT_R(0))) |           \
                     ((DITHER_RGBA_555_LUT_G(0))) |           \
                     ((DITHER_RGBA_555_LUT_B(0))) |           \
                     ((DITHER_RGBA_555_LUT_R(1) << 16)) |     \
                     ((DITHER_RGBA_555_LUT_G(1) << 16)) |     \
                     ((DITHER_RGBA_555_LUT_B(1) << 16));      \
                     dest += 2; src += 2;                     \
}
#endif

/*****************************************************************************/
/* MACROS for plain RGBA -> BGR555 conversion */
#define WRITE1_RGBA_BGR555(src, dest)        \
*dest = ((*src << 7) & 0x7c00) |           \
        ((*src >> 6) & 0x3e0) |            \
        ((*src >> 19)& 0x1f); dest++; src++
#ifdef WORDS_BIGENDIAN
#define WRITE2_RGBA_BGR555(src, dest)                   \
{                                                       \
 *((DATA32 *)dest) = ((src[1] << 7) & 0x7c00) |         \
                     ((src[1] >> 6) & 0x3e0) |          \
                     ((src[1] >> 19)& 0x1f) |          \
                     ((src[0] << 23) & 0x7c000000) |    \
                     ((src[0] << 10) & 0x3e00000) |     \
                     ((src[0] >>  3) & 0x1f0000);        \
                     dest += 2; src += 2;               \
}
#else
#define WRITE2_RGBA_BGR555(src, dest)                    \
{                                                        \
 *((DATA32 *)dest) = ((src[0] << 7) & 0x7c00) |          \
                     ((src[0] >> 6) & 0x3e0) |           \
                     ((src[0] >> 19)& 0x1f) |           \
                     ((src[1] << 23) & 0x7c000000) |     \
                     ((src[1] << 10) & 0x3e00000) |      \
                     ((src[1] >>  3) & 0x1f0000);         \
                     dest += 2; src += 2;                \
}
#endif
/*****************************************************************************/
/* MACROS for dithered RGBA -> BGR555 conversion */
#define DITHER_RGBA_BGR555_LUT_R(num) \
(_dither_r16[(((x + num) & 0x3) << 10) | ((y & 0x3) << 8) | ((src[num] >> 0 ) & 0xff)])
#define DITHER_RGBA_BGR555_LUT_G(num) \
(_dither_g16[(((x + num) & 0x3) << 10) | ((y & 0x3) << 8) | ((src[num] >> 8 ) & 0xff)])
#define DITHER_RGBA_BGR555_LUT_B(num) \
(_dither_b16[(((x + num) & 0x3) << 10) | ((y & 0x3) << 8) | ((src[num] >> 16 ) & 0xff)])

#define WRITE1_RGBA_BGR555_DITHER(src, dest)                  \
*dest = (DITHER_RGBA_BGR555_LUT_R(0)) |                       \
        (DITHER_RGBA_BGR555_LUT_G(0)) |                       \
        (DITHER_RGBA_BGR555_LUT_B(0)); dest++; src++
#ifdef WORDS_BIGENDIAN
#define WRITE2_RGBA_BGR555_DITHER(src, dest)                  \
{                                                             \
 *((DATA32 *)dest) = ((DITHER_RGBA_BGR555_LUT_R(1))) |        \
                     ((DITHER_RGBA_BGR555_LUT_G(1))) |        \
                     ((DITHER_RGBA_BGR555_LUT_B(1))) |        \
                     ((DITHER_RGBA_BGR555_LUT_R(0) << 16)) |  \
                     ((DITHER_RGBA_BGR555_LUT_G(0) << 16)) |  \
                     ((DITHER_RGBA_BGR555_LUT_B(0) << 16));   \
                     dest += 2; src += 2;                     \
}
#else
#define WRITE2_RGBA_BGR555_DITHER(src, dest)                  \
{                                                             \
 *((DATA32 *)dest) = ((DITHER_RGBA_BGR555_LUT_R(0))) |        \
                     ((DITHER_RGBA_BGR555_LUT_G(0))) |        \
                     ((DITHER_RGBA_BGR555_LUT_B(0))) |        \
                     ((DITHER_RGBA_BGR555_LUT_R(1) << 16)) |  \
                     ((DITHER_RGBA_BGR555_LUT_G(1) << 16)) |  \
                     ((DITHER_RGBA_BGR555_LUT_B(1) << 16));   \
                     dest += 2; src += 2;                     \
}
#endif

/*****************************************************************************/
/* Actual rendering routines                                                 */
/* RGBA -> RGB332                                                            */
/*****************************************************************************/

/*****************************************************************************/
/* MACROS for plain RGBA -> RGB332 conversion */
#define WRITE1_RGBA_RGB332(src, dest)        \
*dest = _dither_color_lut[((*src >> 6)  & 0x03) |                          \
                          ((*src >> 11) & 0x1c) |                          \
                          ((*src >> 16) & 0xe0)]; dest++; src++
#ifdef WORDS_BIGENDIAN
#define WRITE2_RGBA_RGB332(src, dest)                                      \
{                                                                          \
 *((DATA16 *)dest) = (_dither_color_lut[((src[1] >> 6)  & 0x03) |          \
                                        ((src[1] >> 11) & 0x1c) |          \
                                        ((src[1] >> 16) & 0xe0)]) |        \
                     (_dither_color_lut[((src[0] >> 6)  & 0x03) |          \
                                        ((src[0] >> 11) & 0x1c) |          \
                                        ((src[0] >> 16) & 0xe0)] << 8);    \
                     dest += 2; src += 2;                                  \
}
#define WRITE4_RGBA_RGB332(src, dest)                                      \
{                                                                          \
 *((DATA32 *)dest) = (_dither_color_lut[((src[3] >> 6)  & 0x03) |          \
                                        ((src[3] >> 11) & 0x1c) |          \
                                        ((src[3] >> 16) & 0xe0)]) |        \
                     (_dither_color_lut[((src[2] >> 6)  & 0x03) |          \
                                        ((src[2] >> 11) & 0x1c) |          \
                                        ((src[2] >> 16) & 0xe0)] << 8) |   \
                     (_dither_color_lut[((src[1] >> 6)  & 0x03) |          \
                                        ((src[1] >> 11) & 0x1c) |          \
                                        ((src[1] >> 16) & 0xe0)] << 16) |  \
                     (_dither_color_lut[((src[0] >> 6)  & 0x03) |          \
                                        ((src[0] >> 11) & 0x1c) |          \
                                        ((src[0] >> 16) & 0xe0)] << 24);   \
                     dest += 4; src += 4;                                  \
}
#else
#define WRITE2_RGBA_RGB332(src, dest)                                      \
{                                                                          \
 *((DATA16 *)dest) = (_dither_color_lut[((src[0] >> 6)  & 0x03) |          \
                                        ((src[0] >> 11) & 0x1c) |          \
                                        ((src[0] >> 16) & 0xe0)]) |        \
                     (_dither_color_lut[((src[1] >> 6)  & 0x03) |          \
                                        ((src[1] >> 11) & 0x1c) |          \
                                        ((src[1] >> 16) & 0xe0)] << 8);    \
                     dest += 2; src += 2;                                  \
}
#define WRITE4_RGBA_RGB332(src, dest)                                      \
{                                                                          \
 *((DATA32 *)dest) = (_dither_color_lut[((src[0] >> 6)  & 0x03) |          \
                                        ((src[0] >> 11) & 0x1c) |          \
                                        ((src[0] >> 16) & 0xe0)]) |        \
                     (_dither_color_lut[((src[1] >> 6)  & 0x03) |          \
                                        ((src[1] >> 11) & 0x1c) |          \
                                        ((src[1] >> 16) & 0xe0)] << 8) |   \
                     (_dither_color_lut[((src[2] >> 6)  & 0x03) |          \
                                        ((src[2] >> 11) & 0x1c) |          \
                                        ((src[2] >> 16) & 0xe0)] << 16) |  \
                     (_dither_color_lut[((src[3] >> 6)  & 0x03) |          \
                                        ((src[3] >> 11) & 0x1c) |          \
                                        ((src[3] >> 16) & 0xe0)] << 24);   \
                     dest += 4; src += 4;                                  \
}
#endif
/*****************************************************************************/
/* MACROS for dithered RGBA -> RGB332 conversion */
#define DITHER_RGBA_332_LUT_R(num) \
(_dither_r8[(((x + num) & 0x7) << 11) | ((y & 0x7) << 8) | ((src[num] >> 16) & 0xff)])
#define DITHER_RGBA_332_LUT_G(num) \
(_dither_g8[(((x + num) & 0x7) << 11) | ((y & 0x7) << 8) | ((src[num] >> 8)  & 0xff)])
#define DITHER_RGBA_332_LUT_B(num) \
(_dither_b8[(((x + num) & 0x7) << 11) | ((y & 0x7) << 8) | ((src[num] >> 0) & 0xff)])

#define WRITE1_RGBA_RGB332_DITHER(src, dest)                                   \
*dest = _dither_color_lut[(DITHER_RGBA_332_LUT_R(0)) |                         \
                          (DITHER_RGBA_332_LUT_G(0)) |                         \
                          (DITHER_RGBA_332_LUT_B(0))]; dest++; src++
#ifdef WORDS_BIGENDIAN
#define WRITE4_RGBA_RGB332_DITHER(src, dest)                                   \
{                                                                              \
 *((DATA32 *)dest) = (_dither_color_lut[((DITHER_RGBA_332_LUT_R(3))) |         \
                                        ((DITHER_RGBA_332_LUT_G(3))) |         \
                                        ((DITHER_RGBA_332_LUT_B(3)))]) |       \
                     (_dither_color_lut[((DITHER_RGBA_332_LUT_R(2))) |         \
                                        ((DITHER_RGBA_332_LUT_G(2))) |         \
                                        ((DITHER_RGBA_332_LUT_B(2)))] << 8) |  \
                     (_dither_color_lut[((DITHER_RGBA_332_LUT_R(1))) |         \
                                        ((DITHER_RGBA_332_LUT_G(1))) |         \
                                        ((DITHER_RGBA_332_LUT_B(1)))] << 16) | \
                     (_dither_color_lut[((DITHER_RGBA_332_LUT_R(0))) |         \
                                        ((DITHER_RGBA_332_LUT_G(0))) |         \
                                        ((DITHER_RGBA_332_LUT_B(0)))] << 24);  \
                     dest += 4; src += 4;                                      \
}
#define WRITE2_RGBA_RGB332_DITHER(src, dest)                                   \
{                                                                              \
 *((DATA32 *)dest) = (_dither_color_lut[((DITHER_RGBA_332_LUT_R(1))) |         \
                                        ((DITHER_RGBA_332_LUT_G(1))) |         \
                                        ((DITHER_RGBA_332_LUT_B(1)))]) |       \
                     (_dither_color_lut[((DITHER_RGBA_332_LUT_R(0))) |         \
                                        ((DITHER_RGBA_332_LUT_G(0))) |         \
                                        ((DITHER_RGBA_332_LUT_B(0)))] << 8);   \
                     dest += 2; src += 2;                                      \
}
#else
#define WRITE4_RGBA_RGB332_DITHER(src, dest)                                   \
{                                                                              \
 *((DATA32 *)dest) = (_dither_color_lut[((DITHER_RGBA_332_LUT_R(0))) |         \
                                        ((DITHER_RGBA_332_LUT_G(0))) |         \
                                        ((DITHER_RGBA_332_LUT_B(0)))]) |       \
                     (_dither_color_lut[((DITHER_RGBA_332_LUT_R(1))) |         \
                                        ((DITHER_RGBA_332_LUT_G(1))) |         \
                                        ((DITHER_RGBA_332_LUT_B(1)))] << 8) |  \
                     (_dither_color_lut[((DITHER_RGBA_332_LUT_R(2))) |         \
                                        ((DITHER_RGBA_332_LUT_G(2))) |         \
                                        ((DITHER_RGBA_332_LUT_B(2)))] << 16) | \
                     (_dither_color_lut[((DITHER_RGBA_332_LUT_R(3))) |         \
                                        ((DITHER_RGBA_332_LUT_G(3))) |         \
                                        ((DITHER_RGBA_332_LUT_B(3)))] << 24);  \
                     dest += 4; src += 4;                                      \
}
#define WRITE2_RGBA_RGB332_DITHER(src, dest)                                   \
{                                                                              \
 *((DATA32 *)dest) = (_dither_color_lut[((DITHER_RGBA_332_LUT_R(0))) |         \
                                        ((DITHER_RGBA_332_LUT_G(0))) |         \
                                        ((DITHER_RGBA_332_LUT_B(0)))]) |       \
                     (_dither_color_lut[((DITHER_RGBA_332_LUT_R(1))) |         \
                                        ((DITHER_RGBA_332_LUT_G(1))) |         \
                                        ((DITHER_RGBA_332_LUT_B(1)))] << 8);   \
                     dest += 2; src += 2;                                      \
}
#endif


/*****************************************************************************/
/* Actual rendering routines                                                 */
/* RGBA -> RGB666                                                            */
/*****************************************************************************/

/*****************************************************************************/
/* MACROS for plain RGBA -> RGB666 conversion */
#define WRITE1_RGBA_RGB666(src, dest)        \
{ \
*dest = _dither_color_lut[(_dither_666r[(*src >> 16) & 0xff]     ) +        \
                          (_dither_666g[(*src >> 8 ) & 0xff]     ) +        \
                          (_dither_666b[(*src      ) & 0xff]     )]; dest++; src++; \
}
#ifdef WORDS_BIGENDIAN
#define WRITE2_RGBA_RGB666(src, dest)                                      \
{                                                                          \
 *((DATA16 *)dest) = (_dither_color_lut[(_dither_666r[(src[1] >> 16) & 0xff]     ) + \
                                        (_dither_666g[(src[1] >> 8 ) & 0xff]     ) + \
                                        (_dither_666b[(src[1]      ) & 0xff]     )]) | \
                     (_dither_color_lut[(_dither_666r[(src[0] >> 16) & 0xff]     ) + \
                                        (_dither_666g[(src[0] >> 8 ) & 0xff]     ) + \
                                        (_dither_666b[(src[0]      ) & 0xff]     )] << 8); \
                     dest += 2; src += 2;                                  \
}
#define WRITE4_RGBA_RGB666(src, dest)                                      \
{                                                                          \
 *((DATA16 *)dest) = (_dither_color_lut[(_dither_666r[(src[3] >> 16) & 0xff]     ) + \
                                        (_dither_666g[(src[3] >> 8 ) & 0xff]     ) + \
                                        (_dither_666b[(src[3]      ) & 0xff]     )]) | \
                     (_dither_color_lut[(_dither_666r[(src[2] >> 16) & 0xff]     ) + \
                                        (_dither_666g[(src[2] >> 8 ) & 0xff]     ) + \
                                        (_dither_666b[(src[2]      ) & 0xff]     )] << 8) | \
                     (_dither_color_lut[(_dither_666r[(src[1] >> 16) & 0xff]     ) + \
                                        (_dither_666g[(src[1] >> 8 ) & 0xff]     ) + \
                                        (_dither_666b[(src[1]      ) & 0xff]     )] << 16) | \
                     (_dither_color_lut[(_dither_666r[(src[0] >> 16) & 0xff]     ) + \
                                        (_dither_666g[(src[0] >> 8 ) & 0xff]     ) + \
                                        (_dither_666b[(src[0]      ) & 0xff]     )] << 24); \
                     dest += 4; src += 4;                                  \
}
#else
#define WRITE2_RGBA_RGB666(src, dest)                                      \
{                                                                          \
 *((DATA16 *)dest) = (_dither_color_lut[(_dither_666r[(src[0] >> 16) & 0xff]     ) + \
                                        (_dither_666g[(src[0] >> 8 ) & 0xff]     ) + \
                                        (_dither_666b[(src[0]      ) & 0xff]     )]) | \
                     (_dither_color_lut[(_dither_666r[(src[1] >> 16) & 0xff]     ) + \
                                        (_dither_666g[(src[1] >> 8 ) & 0xff]     ) + \
                                        (_dither_666b[(src[1]      ) & 0xff]     )] << 8); \
                     dest += 2; src += 2;                                  \
}
#define WRITE4_RGBA_RGB666(src, dest)                                      \
{                                                                          \
 *((DATA16 *)dest) = (_dither_color_lut[(_dither_666r[(src[0] >> 16) & 0xff]     ) + \
                                        (_dither_666g[(src[0] >> 8 ) & 0xff]     ) + \
                                        (_dither_666b[(src[0]      ) & 0xff]     )]) | \
                     (_dither_color_lut[(_dither_666r[(src[1] >> 16) & 0xff]     ) + \
                                        (_dither_666g[(src[1] >> 8 ) & 0xff]     ) + \
                                        (_dither_666b[(src[1]      ) & 0xff]     )] << 8) | \
                     (_dither_color_lut[(_dither_666r[(src[2] >> 16) & 0xff]     ) + \
                                        (_dither_666g[(src[2] >> 8 ) & 0xff]     ) + \
                                        (_dither_666b[(src[2]      ) & 0xff]     )] << 16) | \
                     (_dither_color_lut[(_dither_666r[(src[3] >> 16) & 0xff]     ) + \
                                        (_dither_666g[(src[3] >> 8 ) & 0xff]     ) + \
                                        (_dither_666b[(src[3]      ) & 0xff]     )] << 24); \
                     dest += 4; src += 4;                                  \
}
#endif
/*****************************************************************************/
/* MACROS for dithered RGBA -> RGB666 conversion */
#define DITHER_RGBA_666_LUT_R(num) \
(_dither_r8[(((x + num) & 0x7) << 11) | ((y & 0x7) << 8) | ((src[num] >> 16) & 0xff)])
#define DITHER_RGBA_666_LUT_G(num) \
(_dither_g8[(((x + num) & 0x7) << 11) | ((y & 0x7) << 8) | ((src[num] >> 8)  & 0xff)])
#define DITHER_RGBA_666_LUT_B(num) \
(_dither_b8[(((x + num) & 0x7) << 11) | ((y & 0x7) << 8) | ((src[num] >> 0) & 0xff)])

#define WRITE1_RGBA_RGB666_DITHER(src, dest)                                   \
*dest = _dither_color_lut[(DITHER_RGBA_666_LUT_R(0)) +                         \
                          (DITHER_RGBA_666_LUT_G(0)) +                         \
                          (DITHER_RGBA_666_LUT_B(0))]; dest++; src++
#ifdef WORDS_BIGENDIAN
#define WRITE4_RGBA_RGB666_DITHER(src, dest)                                   \
{                                                                              \
 *((DATA32 *)dest) = (_dither_color_lut[((DITHER_RGBA_666_LUT_R(3))) +         \
                                        ((DITHER_RGBA_666_LUT_G(3))) +         \
                                        ((DITHER_RGBA_666_LUT_B(3)))]) |       \
                     (_dither_color_lut[((DITHER_RGBA_666_LUT_R(2))) +         \
                                        ((DITHER_RGBA_666_LUT_G(2))) +         \
                                        ((DITHER_RGBA_666_LUT_B(2)))] << 8) |  \
                     (_dither_color_lut[((DITHER_RGBA_666_LUT_R(1))) +         \
                                        ((DITHER_RGBA_666_LUT_G(1))) +         \
                                        ((DITHER_RGBA_666_LUT_B(1)))] << 16) | \
                     (_dither_color_lut[((DITHER_RGBA_666_LUT_R(0))) +         \
                                        ((DITHER_RGBA_666_LUT_G(0))) +         \
                                        ((DITHER_RGBA_666_LUT_B(0)))] << 24);  \
                     dest += 4; src += 4;                                      \
}
#define WRITE2_RGBA_RGB666_DITHER(src, dest)                                   \
{                                                                              \
 *((DATA32 *)dest) = (_dither_color_lut[((DITHER_RGBA_666_LUT_R(1))) +         \
                                        ((DITHER_RGBA_666_LUT_G(1))) +         \
                                        ((DITHER_RGBA_666_LUT_B(1)))]) |       \
                     (_dither_color_lut[((DITHER_RGBA_666_LUT_R(0))) +         \
                                        ((DITHER_RGBA_666_LUT_G(0))) +         \
                                        ((DITHER_RGBA_666_LUT_B(0)))] << 8);   \
                     dest += 2; src += 2;                                      \
}
#else
#define WRITE4_RGBA_RGB666_DITHER(src, dest)                                   \
{                                                                              \
 *((DATA32 *)dest) = (_dither_color_lut[((DITHER_RGBA_666_LUT_R(0))) +         \
                                        ((DITHER_RGBA_666_LUT_G(0))) +         \
                                        ((DITHER_RGBA_666_LUT_B(0)))]) |       \
                     (_dither_color_lut[((DITHER_RGBA_666_LUT_R(1))) +         \
                                        ((DITHER_RGBA_666_LUT_G(1))) +         \
                                        ((DITHER_RGBA_666_LUT_B(1)))] << 8) |  \
                     (_dither_color_lut[((DITHER_RGBA_666_LUT_R(2))) +         \
                                        ((DITHER_RGBA_666_LUT_G(2))) +         \
                                        ((DITHER_RGBA_666_LUT_B(2)))] << 16) | \
                     (_dither_color_lut[((DITHER_RGBA_666_LUT_R(3))) +         \
                                        ((DITHER_RGBA_666_LUT_G(3))) +         \
                                        ((DITHER_RGBA_666_LUT_B(3)))] << 24);  \
                     dest += 4; src += 4;                                      \
}
#define WRITE2_RGBA_RGB666_DITHER(src, dest)                                   \
{                                                                              \
 *((DATA32 *)dest) = (_dither_color_lut[((DITHER_RGBA_666_LUT_R(0))) +         \
                                        ((DITHER_RGBA_666_LUT_G(0))) +         \
                                        ((DITHER_RGBA_666_LUT_B(0)))]) |       \
                     (_dither_color_lut[((DITHER_RGBA_666_LUT_R(1))) +         \
                                        ((DITHER_RGBA_666_LUT_G(1))) +         \
                                        ((DITHER_RGBA_666_LUT_B(1)))] << 8);   \
                     dest += 2; src += 2;                                      \
}
#endif

/*****************************************************************************/
/* Actual rendering routines                                                 */
/* RGBA -> RGB232                                                            */
/*****************************************************************************/

/*****************************************************************************/
/* MACROS for plain RGBA -> RGB232 conversion */
#define RGB232_BSHIFT >> 6
#define RGB232_GSHIFT >> 11
#define RGB232_RSHIFT >> 17
#define RGB232_BMASK & 0x03
#define RGB232_GMASK & 0x1c
#define RGB232_RMASK & 0x60

#define WRITE1_RGBA_RGB232(src, dest)        \
*dest = _dither_color_lut[((*src RGB232_BSHIFT) RGB232_BMASK) |                          \
                          ((*src RGB232_GSHIFT) RGB232_GMASK) |                          \
                          ((*src RGB232_RSHIFT)  RGB232_RMASK)]; dest++; src++
#ifdef WORDS_BIGENDIAN
#define WRITE2_RGBA_RGB232(src, dest)                                      \
{                                                                          \
 *((DATA16 *)dest) = (_dither_color_lut[((src[1] RGB232_BSHIFT) RGB232_BMASK) |          \
                                        ((src[1] RGB232_GSHIFT) RGB232_GMASK) |          \
                                        ((src[1] RGB232_RSHIFT) RGB232_RMASK)]) |        \
                     (_dither_color_lut[((src[0] RGB232_BSHIFT) RGB232_BMASK) |          \
                                        ((src[0] RGB232_GSHIFT) RGB232_GMASK) |          \
                                        ((src[0] RGB232_RSHIFT) RGB232_RMASK)] << 8);    \
                     dest += 2; src += 2;                                  \
}
#define WRITE4_RGBA_RGB232(src, dest)                                      \
{                                                                          \
 *((DATA32 *)dest) = (_dither_color_lut[((src[3] RGB232_BSHIFT) RGB232_BMASK) |          \
                                        ((src[3] RGB232_GSHIFT) RGB232_GMASK) |          \
                                        ((src[3] RGB232_RSHIFT) RGB232_RMASK)]) |        \
                     (_dither_color_lut[((src[2] RGB232_BSHIFT) RGB232_BMASK) |          \
                                        ((src[2] RGB232_GSHIFT) RGB232_GMASK) |          \
                                        ((src[2] RGB232_RSHIFT) RGB232_RMASK)] << 8) |   \
                     (_dither_color_lut[((src[1] RGB232_BSHIFT) RGB232_BMASK) |          \
                                        ((src[1] RGB232_GSHIFT) RGB232_GMASK) |          \
                                        ((src[1] RGB232_RSHIFT) RGB232_RMASK)] << 16) |  \
                     (_dither_color_lut[((src[0] RGB232_BSHIFT) RGB232_BMASK) |          \
                                        ((src[0] RGB232_GSHIFT) RGB232_GMASK) |          \
                                        ((src[0] RGB232_RSHIFT) RGB232_RMASK)] << 24);   \
                     dest += 4; src += 4;                                  \
}
#else
#define WRITE2_RGBA_RGB232(src, dest)                                      \
{                                                                          \
 *((DATA16 *)dest) = (_dither_color_lut[((src[0] RGB232_BSHIFT) RGB232_BMASK) |          \
                                        ((src[0] RGB232_GSHIFT) RGB232_GMASK) |          \
                                        ((src[0] RGB232_RSHIFT) RGB232_RMASK)]) |        \
                     (_dither_color_lut[((src[1] RGB232_BSHIFT) RGB232_BMASK) |          \
                                        ((src[1] RGB232_GSHIFT) RGB232_GMASK) |          \
                                        ((src[1] RGB232_RSHIFT) RGB232_RMASK)] << 8);    \
                     dest += 2; src += 2;                                  \
}
#define WRITE4_RGBA_RGB232(src, dest)                                      \
{                                                                          \
 *((DATA32 *)dest) = (_dither_color_lut[((src[0] RGB232_BSHIFT) RGB232_BMASK) |          \
                                        ((src[0] RGB232_GSHIFT) RGB232_GMASK) |          \
                                        ((src[0] RGB232_RSHIFT) RGB232_RMASK)]) |        \
                     (_dither_color_lut[((src[1] RGB232_BSHIFT) RGB232_BMASK) |          \
                                        ((src[1] RGB232_GSHIFT) RGB232_GMASK) |          \
                                        ((src[1] RGB232_RSHIFT) RGB232_RMASK)] << 8) |   \
                     (_dither_color_lut[((src[2] RGB232_BSHIFT) RGB232_BMASK) |          \
                                        ((src[2] RGB232_GSHIFT) RGB232_GMASK) |          \
                                        ((src[2] RGB232_RSHIFT) RGB232_RMASK)] << 16) |  \
                     (_dither_color_lut[((src[3] RGB232_BSHIFT) RGB232_BMASK) |          \
                                        ((src[3] RGB232_GSHIFT) RGB232_GMASK) |          \
                                        ((src[3] RGB232_RSHIFT) RGB232_RMASK)] << 24);   \
                     dest += 4; src += 4;                                  \
}
#endif
/*****************************************************************************/
/* MACROS for dithered RGBA -> RGB232 conversion */
#define DITHER_RGBA_232_LUT_R(num) \
(_dither_r8[(((x + num) & 0x7) << 11) | ((y & 0x7) << 8) | ((src[num] >> 16) & 0xff)])
#define DITHER_RGBA_232_LUT_G(num) \
(_dither_g8[(((x + num) & 0x7) << 11) | ((y & 0x7) << 8) | ((src[num] >> 8)  & 0xff)])
#define DITHER_RGBA_232_LUT_B(num) \
(_dither_b8[(((x + num) & 0x7) << 11) | ((y & 0x7) << 8) | ((src[num] >> 0) & 0xff)])

#define WRITE1_RGBA_RGB232_DITHER(src, dest)                                   \
*dest = _dither_color_lut[(DITHER_RGBA_232_LUT_R(0)) |                         \
                          (DITHER_RGBA_232_LUT_G(0)) |                         \
                          (DITHER_RGBA_232_LUT_B(0))]; dest++; src++
#ifdef WORDS_BIGENDIAN
#define WRITE4_RGBA_RGB232_DITHER(src, dest)                                   \
{                                                                              \
 *((DATA32 *)dest) = (_dither_color_lut[((DITHER_RGBA_232_LUT_R(3))) |         \
                                        ((DITHER_RGBA_232_LUT_G(3))) |         \
                                        ((DITHER_RGBA_232_LUT_B(3)))]) |       \
                     (_dither_color_lut[((DITHER_RGBA_232_LUT_R(2))) |         \
                                        ((DITHER_RGBA_232_LUT_G(2))) |         \
                                        ((DITHER_RGBA_232_LUT_B(2)))] << 8) |  \
                     (_dither_color_lut[((DITHER_RGBA_232_LUT_R(1))) |         \
                                        ((DITHER_RGBA_232_LUT_G(1))) |         \
                                        ((DITHER_RGBA_232_LUT_B(1)))] << 16) | \
                     (_dither_color_lut[((DITHER_RGBA_232_LUT_R(0))) |         \
                                        ((DITHER_RGBA_232_LUT_G(0))) |         \
                                        ((DITHER_RGBA_232_LUT_B(0)))] << 24);  \
                     dest += 4; src += 4;                                      \
}
#define WRITE2_RGBA_RGB232_DITHER(src, dest)                                   \
{                                                                              \
 *((DATA32 *)dest) = (_dither_color_lut[((DITHER_RGBA_232_LUT_R(1))) |         \
                                        ((DITHER_RGBA_232_LUT_G(1))) |         \
                                        ((DITHER_RGBA_232_LUT_B(1)))]) |       \
                     (_dither_color_lut[((DITHER_RGBA_232_LUT_R(0))) |         \
                                        ((DITHER_RGBA_232_LUT_G(0))) |         \
                                        ((DITHER_RGBA_232_LUT_B(0)))] << 8);   \
                     dest += 2; src += 2;                                      \
}
#else
#define WRITE4_RGBA_RGB232_DITHER(src, dest)                                   \
{                                                                              \
 *((DATA32 *)dest) = (_dither_color_lut[((DITHER_RGBA_232_LUT_R(0))) |         \
                                        ((DITHER_RGBA_232_LUT_G(0))) |         \
                                        ((DITHER_RGBA_232_LUT_B(0)))]) |       \
                     (_dither_color_lut[((DITHER_RGBA_232_LUT_R(1))) |         \
                                        ((DITHER_RGBA_232_LUT_G(1))) |         \
                                        ((DITHER_RGBA_232_LUT_B(1)))] << 8) |  \
                     (_dither_color_lut[((DITHER_RGBA_232_LUT_R(2))) |         \
                                        ((DITHER_RGBA_232_LUT_G(2))) |         \
                                        ((DITHER_RGBA_232_LUT_B(2)))] << 16) | \
                     (_dither_color_lut[((DITHER_RGBA_232_LUT_R(3))) |         \
                                        ((DITHER_RGBA_232_LUT_G(3))) |         \
                                        ((DITHER_RGBA_232_LUT_B(3)))] << 24);  \
                     dest += 4; src += 4;                                      \
}
#define WRITE2_RGBA_RGB232_DITHER(src, dest)                                   \
{                                                                              \
 *((DATA32 *)dest) = (_dither_color_lut[((DITHER_RGBA_232_LUT_R(0))) |         \
                                        ((DITHER_RGBA_232_LUT_G(0))) |         \
                                        ((DITHER_RGBA_232_LUT_B(0)))]) |       \
                     (_dither_color_lut[((DITHER_RGBA_232_LUT_R(1))) |         \
                                        ((DITHER_RGBA_232_LUT_G(1))) |         \
                                        ((DITHER_RGBA_232_LUT_B(1)))] << 8);   \
                     dest += 2; src += 2;                                      \
}
#endif

/*****************************************************************************/
/* Actual rendering routines                                                 */
/* RGBA -> RGB222                                                            */
/*****************************************************************************/

/*****************************************************************************/
/* MACROS for plain RGBA -> RGB222 conversion */
#define RGB222_BSHIFT >> 6
#define RGB222_GSHIFT >> 12
#define RGB222_RSHIFT >> 18
#define RGB222_BMASK & 0x03
#define RGB222_GMASK & 0x0c
#define RGB222_RMASK & 0x30

#define WRITE1_RGBA_RGB222(src, dest)        \
*dest = _dither_color_lut[((*src RGB222_BSHIFT) RGB222_BMASK) |                          \
                          ((*src RGB222_GSHIFT) RGB222_GMASK) |                          \
                          ((*src RGB222_RSHIFT)  RGB222_RMASK)]; dest++; src++
#ifdef WORDS_BIGENDIAN
#define WRITE2_RGBA_RGB222(src, dest)                                      \
{                                                                          \
 *((DATA16 *)dest) = (_dither_color_lut[((src[1] RGB222_BSHIFT) RGB222_BMASK) |          \
                                        ((src[1] RGB222_GSHIFT) RGB222_GMASK) |          \
                                        ((src[1] RGB222_RSHIFT) RGB222_RMASK)]) |        \
                     (_dither_color_lut[((src[0] RGB222_BSHIFT) RGB222_BMASK) |          \
                                        ((src[0] RGB222_GSHIFT) RGB222_GMASK) |          \
                                        ((src[0] RGB222_RSHIFT) RGB222_RMASK)] << 8);    \
                     dest += 2; src += 2;                                  \
}
#define WRITE4_RGBA_RGB222(src, dest)                                      \
{                                                                          \
 *((DATA32 *)dest) = (_dither_color_lut[((src[3] RGB222_BSHIFT) RGB222_BMASK) |          \
                                        ((src[3] RGB222_GSHIFT) RGB222_GMASK) |          \
                                        ((src[3] RGB222_RSHIFT) RGB222_RMASK)]) |        \
                     (_dither_color_lut[((src[2] RGB222_BSHIFT) RGB222_BMASK) |          \
                                        ((src[2] RGB222_GSHIFT) RGB222_GMASK) |          \
                                        ((src[2] RGB222_RSHIFT) RGB222_RMASK)] << 8) |   \
                     (_dither_color_lut[((src[1] RGB222_BSHIFT) RGB222_BMASK) |          \
                                        ((src[1] RGB222_GSHIFT) RGB222_GMASK) |          \
                                        ((src[1] RGB222_RSHIFT) RGB222_RMASK)] << 16) |  \
                     (_dither_color_lut[((src[0] RGB222_BSHIFT) RGB222_BMASK) |          \
                                        ((src[0] RGB222_GSHIFT) RGB222_GMASK) |          \
                                        ((src[0] RGB222_RSHIFT) RGB222_RMASK)] << 24);   \
                     dest += 4; src += 4;                                  \
}
#else
#define WRITE2_RGBA_RGB222(src, dest)                                      \
{                                                                          \
 *((DATA16 *)dest) = (_dither_color_lut[((src[0] RGB222_BSHIFT) RGB222_BMASK) |          \
                                        ((src[0] RGB222_GSHIFT) RGB222_GMASK) |          \
                                        ((src[0] RGB222_RSHIFT) RGB222_RMASK)]) |        \
                     (_dither_color_lut[((src[1] RGB222_BSHIFT) RGB222_BMASK) |          \
                                        ((src[1] RGB222_GSHIFT) RGB222_GMASK) |          \
                                        ((src[1] RGB222_RSHIFT) RGB222_RMASK)] << 8);    \
                     dest += 2; src += 2;                                  \
}
#define WRITE4_RGBA_RGB222(src, dest)                                      \
{                                                                          \
 *((DATA32 *)dest) = (_dither_color_lut[((src[0] RGB222_BSHIFT) RGB222_BMASK) |          \
                                        ((src[0] RGB222_GSHIFT) RGB222_GMASK) |          \
                                        ((src[0] RGB222_RSHIFT) RGB222_RMASK)]) |        \
                     (_dither_color_lut[((src[1] RGB222_BSHIFT) RGB222_BMASK) |          \
                                        ((src[1] RGB222_GSHIFT) RGB222_GMASK) |          \
                                        ((src[1] RGB222_RSHIFT) RGB222_RMASK)] << 8) |   \
                     (_dither_color_lut[((src[2] RGB222_BSHIFT) RGB222_BMASK) |          \
                                        ((src[2] RGB222_GSHIFT) RGB222_GMASK) |          \
                                        ((src[2] RGB222_RSHIFT) RGB222_RMASK)] << 16) |  \
                     (_dither_color_lut[((src[3] RGB222_BSHIFT) RGB222_BMASK) |          \
                                        ((src[3] RGB222_GSHIFT) RGB222_GMASK) |          \
                                        ((src[3] RGB222_RSHIFT) RGB222_RMASK)] << 24);   \
                     dest += 4; src += 4;                                  \
}
#endif
/*****************************************************************************/
/* MACROS for dithered RGBA -> RGB222 conversion */
#define DITHER_RGBA_222_LUT_R(num) \
(_dither_r8[(((x + num) & 0x7) << 11) | ((y & 0x7) << 8) | ((src[num] >> 16) & 0xff)])
#define DITHER_RGBA_222_LUT_G(num) \
(_dither_g8[(((x + num) & 0x7) << 11) | ((y & 0x7) << 8) | ((src[num] >> 8)  & 0xff)])
#define DITHER_RGBA_222_LUT_B(num) \
(_dither_b8[(((x + num) & 0x7) << 11) | ((y & 0x7) << 8) | ((src[num] >> 0) & 0xff)])

#define WRITE1_RGBA_RGB222_DITHER(src, dest)                                   \
*dest = _dither_color_lut[(DITHER_RGBA_222_LUT_R(0)) |                         \
                          (DITHER_RGBA_222_LUT_G(0)) |                         \
                          (DITHER_RGBA_222_LUT_B(0))]; dest++; src++
#ifdef WORDS_BIGENDIAN
#define WRITE4_RGBA_RGB222_DITHER(src, dest)                                   \
{                                                                              \
 *((DATA32 *)dest) = (_dither_color_lut[((DITHER_RGBA_222_LUT_R(3))) |         \
                                        ((DITHER_RGBA_222_LUT_G(3))) |         \
                                        ((DITHER_RGBA_222_LUT_B(3)))]) |       \
                     (_dither_color_lut[((DITHER_RGBA_222_LUT_R(2))) |         \
                                        ((DITHER_RGBA_222_LUT_G(2))) |         \
                                        ((DITHER_RGBA_222_LUT_B(2)))] << 8) |  \
                     (_dither_color_lut[((DITHER_RGBA_222_LUT_R(1))) |         \
                                        ((DITHER_RGBA_222_LUT_G(1))) |         \
                                        ((DITHER_RGBA_222_LUT_B(1)))] << 16) | \
                     (_dither_color_lut[((DITHER_RGBA_222_LUT_R(0))) |         \
                                        ((DITHER_RGBA_222_LUT_G(0))) |         \
                                        ((DITHER_RGBA_222_LUT_B(0)))] << 24);  \
                     dest += 4; src += 4;                                      \
}
#define WRITE2_RGBA_RGB222_DITHER(src, dest)                                   \
{                                                                              \
 *((DATA32 *)dest) = (_dither_color_lut[((DITHER_RGBA_222_LUT_R(1))) |         \
                                        ((DITHER_RGBA_222_LUT_G(1))) |         \
                                        ((DITHER_RGBA_222_LUT_B(1)))]) |       \
                     (_dither_color_lut[((DITHER_RGBA_222_LUT_R(0))) |         \
                                        ((DITHER_RGBA_222_LUT_G(0))) |         \
                                        ((DITHER_RGBA_222_LUT_B(0)))] << 8);   \
                     dest += 2; src += 2;                                      \
}
#else
#define WRITE4_RGBA_RGB222_DITHER(src, dest)                                   \
{                                                                              \
 *((DATA32 *)dest) = (_dither_color_lut[((DITHER_RGBA_222_LUT_R(0))) |         \
                                        ((DITHER_RGBA_222_LUT_G(0))) |         \
                                        ((DITHER_RGBA_222_LUT_B(0)))]) |       \
                     (_dither_color_lut[((DITHER_RGBA_222_LUT_R(1))) |         \
                                        ((DITHER_RGBA_222_LUT_G(1))) |         \
                                        ((DITHER_RGBA_222_LUT_B(1)))] << 8) |  \
                     (_dither_color_lut[((DITHER_RGBA_222_LUT_R(2))) |         \
                                        ((DITHER_RGBA_222_LUT_G(2))) |         \
                                        ((DITHER_RGBA_222_LUT_B(2)))] << 16) | \
                     (_dither_color_lut[((DITHER_RGBA_222_LUT_R(3))) |         \
                                        ((DITHER_RGBA_222_LUT_G(3))) |         \
                                        ((DITHER_RGBA_222_LUT_B(3)))] << 24);  \
                     dest += 4; src += 4;                                      \
}
#define WRITE2_RGBA_RGB222_DITHER(src, dest)                                   \
{                                                                              \
 *((DATA32 *)dest) = (_dither_color_lut[((DITHER_RGBA_222_LUT_R(0))) |         \
                                        ((DITHER_RGBA_222_LUT_G(0))) |         \
                                        ((DITHER_RGBA_222_LUT_B(0)))]) |       \
                     (_dither_color_lut[((DITHER_RGBA_222_LUT_R(1))) |         \
                                        ((DITHER_RGBA_222_LUT_G(1))) |         \
                                        ((DITHER_RGBA_222_LUT_B(1)))] << 8);   \
                     dest += 2; src += 2;                                      \
}
#endif

/*****************************************************************************/
/* Actual rendering routines                                                 */
/* RGBA -> RGB221                                                            */
/*****************************************************************************/

/*****************************************************************************/
/* MACROS for plain RGBA -> RGB221 conversion */
#define RGB221_BSHIFT >> 7
#define RGB221_GSHIFT >> 13
#define RGB221_RSHIFT >> 19
#define RGB221_BMASK & 0x01
#define RGB221_GMASK & 0x06
#define RGB221_RMASK & 0x18

#define WRITE1_RGBA_RGB221(src, dest)        \
*dest = _dither_color_lut[((*src RGB221_BSHIFT) RGB221_BMASK) |                          \
                          ((*src RGB221_GSHIFT) RGB221_GMASK) |                          \
                          ((*src RGB221_RSHIFT)  RGB221_RMASK)]; dest++; src++
#ifdef WORDS_BIGENDIAN
#define WRITE2_RGBA_RGB221(src, dest)                                      \
{                                                                          \
 *((DATA16 *)dest) = (_dither_color_lut[((src[1] RGB221_BSHIFT) RGB221_BMASK) |          \
                                        ((src[1] RGB221_GSHIFT) RGB221_GMASK) |          \
                                        ((src[1] RGB221_RSHIFT) RGB221_RMASK)]) |        \
                     (_dither_color_lut[((src[0] RGB221_BSHIFT) RGB221_BMASK) |          \
                                        ((src[0] RGB221_GSHIFT) RGB221_GMASK) |          \
                                        ((src[0] RGB221_RSHIFT) RGB221_RMASK)] << 8);    \
                     dest += 2; src += 2;                                  \
}
#define WRITE4_RGBA_RGB221(src, dest)                                      \
{                                                                          \
 *((DATA32 *)dest) = (_dither_color_lut[((src[3] RGB221_BSHIFT) RGB221_BMASK) |          \
                                        ((src[3] RGB221_GSHIFT) RGB221_GMASK) |          \
                                        ((src[3] RGB221_RSHIFT) RGB221_RMASK)]) |        \
                     (_dither_color_lut[((src[2] RGB221_BSHIFT) RGB221_BMASK) |          \
                                        ((src[2] RGB221_GSHIFT) RGB221_GMASK) |          \
                                        ((src[2] RGB221_RSHIFT) RGB221_RMASK)] << 8) |   \
                     (_dither_color_lut[((src[1] RGB221_BSHIFT) RGB221_BMASK) |          \
                                        ((src[1] RGB221_GSHIFT) RGB221_GMASK) |          \
                                        ((src[1] RGB221_RSHIFT) RGB221_RMASK)] << 16) |  \
                     (_dither_color_lut[((src[0] RGB221_BSHIFT) RGB221_BMASK) |          \
                                        ((src[0] RGB221_GSHIFT) RGB221_GMASK) |          \
                                        ((src[0] RGB221_RSHIFT) RGB221_RMASK)] << 24);   \
                     dest += 4; src += 4;                                  \
}
#else
#define WRITE2_RGBA_RGB221(src, dest)                                      \
{                                                                          \
 *((DATA16 *)dest) = (_dither_color_lut[((src[0] RGB221_BSHIFT) RGB221_BMASK) |          \
                                        ((src[0] RGB221_GSHIFT) RGB221_GMASK) |          \
                                        ((src[0] RGB221_RSHIFT) RGB221_RMASK)]) |        \
                     (_dither_color_lut[((src[1] RGB221_BSHIFT) RGB221_BMASK) |          \
                                        ((src[1] RGB221_GSHIFT) RGB221_GMASK) |          \
                                        ((src[1] RGB221_RSHIFT) RGB221_RMASK)] << 8);    \
                     dest += 2; src += 2;                                  \
}
#define WRITE4_RGBA_RGB221(src, dest)                                      \
{                                                                          \
 *((DATA32 *)dest) = (_dither_color_lut[((src[0] RGB221_BSHIFT) RGB221_BMASK) |          \
                                        ((src[0] RGB221_GSHIFT) RGB221_GMASK) |          \
                                        ((src[0] RGB221_RSHIFT) RGB221_RMASK)]) |        \
                     (_dither_color_lut[((src[1] RGB221_BSHIFT) RGB221_BMASK) |          \
                                        ((src[1] RGB221_GSHIFT) RGB221_GMASK) |          \
                                        ((src[1] RGB221_RSHIFT) RGB221_RMASK)] << 8) |   \
                     (_dither_color_lut[((src[2] RGB221_BSHIFT) RGB221_BMASK) |          \
                                        ((src[2] RGB221_GSHIFT) RGB221_GMASK) |          \
                                        ((src[2] RGB221_RSHIFT) RGB221_RMASK)] << 16) |  \
                     (_dither_color_lut[((src[3] RGB221_BSHIFT) RGB221_BMASK) |          \
                                        ((src[3] RGB221_GSHIFT) RGB221_GMASK) |          \
                                        ((src[3] RGB221_RSHIFT) RGB221_RMASK)] << 24);   \
                     dest += 4; src += 4;                                  \
}
#endif
/*****************************************************************************/
/* MACROS for dithered RGBA -> RGB221 conversion */
#define DITHER_RGBA_221_LUT_R(num) \
(_dither_r8[(((x + num) & 0x7) << 11) | ((y & 0x7) << 8) | ((src[num] >> 16) & 0xff)])
#define DITHER_RGBA_221_LUT_G(num) \
(_dither_g8[(((x + num) & 0x7) << 11) | ((y & 0x7) << 8) | ((src[num] >> 8)  & 0xff)])
#define DITHER_RGBA_221_LUT_B(num) \
(_dither_b8[(((x + num) & 0x7) << 11) | ((y & 0x7) << 8) | ((src[num] >> 0) & 0xff)])

#define WRITE1_RGBA_RGB221_DITHER(src, dest)                                   \
*dest = _dither_color_lut[(DITHER_RGBA_221_LUT_R(0)) |                         \
                          (DITHER_RGBA_221_LUT_G(0)) |                         \
                          (DITHER_RGBA_221_LUT_B(0))]; dest++; src++
#ifdef WORDS_BIGENDIAN
#define WRITE4_RGBA_RGB221_DITHER(src, dest)                                   \
{                                                                              \
 *((DATA32 *)dest) = (_dither_color_lut[((DITHER_RGBA_221_LUT_R(3))) |         \
                                        ((DITHER_RGBA_221_LUT_G(3))) |         \
                                        ((DITHER_RGBA_221_LUT_B(3)))]) |       \
                     (_dither_color_lut[((DITHER_RGBA_221_LUT_R(2))) |         \
                                        ((DITHER_RGBA_221_LUT_G(2))) |         \
                                        ((DITHER_RGBA_221_LUT_B(2)))] << 8) |  \
                     (_dither_color_lut[((DITHER_RGBA_221_LUT_R(1))) |         \
                                        ((DITHER_RGBA_221_LUT_G(1))) |         \
                                        ((DITHER_RGBA_221_LUT_B(1)))] << 16) | \
                     (_dither_color_lut[((DITHER_RGBA_221_LUT_R(0))) |         \
                                        ((DITHER_RGBA_221_LUT_G(0))) |         \
                                        ((DITHER_RGBA_221_LUT_B(0)))] << 24);  \
                     dest += 4; src += 4;                                      \
}
#define WRITE2_RGBA_RGB221_DITHER(src, dest)                                   \
{                                                                              \
 *((DATA32 *)dest) = (_dither_color_lut[((DITHER_RGBA_221_LUT_R(1))) |         \
                                        ((DITHER_RGBA_221_LUT_G(1))) |         \
                                        ((DITHER_RGBA_221_LUT_B(1)))]) |       \
                     (_dither_color_lut[((DITHER_RGBA_221_LUT_R(0))) |         \
                                        ((DITHER_RGBA_221_LUT_G(0))) |         \
                                        ((DITHER_RGBA_221_LUT_B(0)))] << 8);   \
                     dest += 2; src += 2;                                      \
}
#else
#define WRITE4_RGBA_RGB221_DITHER(src, dest)                                   \
{                                                                              \
 *((DATA32 *)dest) = (_dither_color_lut[((DITHER_RGBA_221_LUT_R(0))) |         \
                                        ((DITHER_RGBA_221_LUT_G(0))) |         \
                                        ((DITHER_RGBA_221_LUT_B(0)))]) |       \
                     (_dither_color_lut[((DITHER_RGBA_221_LUT_R(1))) |         \
                                        ((DITHER_RGBA_221_LUT_G(1))) |         \
                                        ((DITHER_RGBA_221_LUT_B(1)))] << 8) |  \
                     (_dither_color_lut[((DITHER_RGBA_221_LUT_R(2))) |         \
                                        ((DITHER_RGBA_221_LUT_G(2))) |         \
                                        ((DITHER_RGBA_221_LUT_B(2)))] << 16) | \
                     (_dither_color_lut[((DITHER_RGBA_221_LUT_R(3))) |         \
                                        ((DITHER_RGBA_221_LUT_G(3))) |         \
                                        ((DITHER_RGBA_221_LUT_B(3)))] << 24);  \
                     dest += 4; src += 4;                                      \
}
#define WRITE2_RGBA_RGB221_DITHER(src, dest)                                   \
{                                                                              \
 *((DATA32 *)dest) = (_dither_color_lut[((DITHER_RGBA_221_LUT_R(0))) |         \
                                        ((DITHER_RGBA_221_LUT_G(0))) |         \
                                        ((DITHER_RGBA_221_LUT_B(0)))]) |       \
                     (_dither_color_lut[((DITHER_RGBA_221_LUT_R(1))) |         \
                                        ((DITHER_RGBA_221_LUT_G(1))) |         \
                                        ((DITHER_RGBA_221_LUT_B(1)))] << 8);   \
                     dest += 2; src += 2;                                      \
}
#endif

/*****************************************************************************/
/* Actual rendering routines                                                 */
/* RGBA -> RGB121                                                            */
/*****************************************************************************/

/*****************************************************************************/
/* MACROS for plain RGBA -> RGB121 conversion */
#define RGB121_BSHIFT >> 7
#define RGB121_GSHIFT >> 13
#define RGB121_RSHIFT >> 20
#define RGB121_BMASK & 0x01
#define RGB121_GMASK & 0x06
#define RGB121_RMASK & 0x08

#define WRITE1_RGBA_RGB121(src, dest)        \
*dest = _dither_color_lut[((*src RGB121_BSHIFT) RGB121_BMASK) |                          \
                          ((*src RGB121_GSHIFT) RGB121_GMASK) |                          \
                          ((*src RGB121_RSHIFT)  RGB121_RMASK)]; dest++; src++
#ifdef WORDS_BIGENDIAN
#define WRITE2_RGBA_RGB121(src, dest)                                      \
{                                                                          \
 *((DATA16 *)dest) = (_dither_color_lut[((src[1] RGB121_BSHIFT) RGB121_BMASK) |          \
                                        ((src[1] RGB121_GSHIFT) RGB121_GMASK) |          \
                                        ((src[1] RGB121_RSHIFT) RGB121_RMASK)]) |        \
                     (_dither_color_lut[((src[0] RGB121_BSHIFT) RGB121_BMASK) |          \
                                        ((src[0] RGB121_GSHIFT) RGB121_GMASK) |          \
                                        ((src[0] RGB121_RSHIFT) RGB121_RMASK)] << 8);    \
                     dest += 2; src += 2;                                  \
}
#define WRITE4_RGBA_RGB121(src, dest)                                      \
{                                                                          \
 *((DATA32 *)dest) = (_dither_color_lut[((src[3] RGB121_BSHIFT) RGB121_BMASK) |          \
                                        ((src[3] RGB121_GSHIFT) RGB121_GMASK) |          \
                                        ((src[3] RGB121_RSHIFT) RGB121_RMASK)]) |        \
                     (_dither_color_lut[((src[2] RGB121_BSHIFT) RGB121_BMASK) |          \
                                        ((src[2] RGB121_GSHIFT) RGB121_GMASK) |          \
                                        ((src[2] RGB121_RSHIFT) RGB121_RMASK)] << 8) |   \
                     (_dither_color_lut[((src[1] RGB121_BSHIFT) RGB121_BMASK) |          \
                                        ((src[1] RGB121_GSHIFT) RGB121_GMASK) |          \
                                        ((src[1] RGB121_RSHIFT) RGB121_RMASK)] << 16) |  \
                     (_dither_color_lut[((src[0] RGB121_BSHIFT) RGB121_BMASK) |          \
                                        ((src[0] RGB121_GSHIFT) RGB121_GMASK) |          \
                                        ((src[0] RGB121_RSHIFT) RGB121_RMASK)] << 24);   \
                     dest += 4; src += 4;                                  \
}
#else
#define WRITE2_RGBA_RGB121(src, dest)                                      \
{                                                                          \
 *((DATA16 *)dest) = (_dither_color_lut[((src[0] RGB121_BSHIFT) RGB121_BMASK) |          \
                                        ((src[0] RGB121_GSHIFT) RGB121_GMASK) |          \
                                        ((src[0] RGB121_RSHIFT) RGB121_RMASK)]) |        \
                     (_dither_color_lut[((src[1] RGB121_BSHIFT) RGB121_BMASK) |          \
                                        ((src[1] RGB121_GSHIFT) RGB121_GMASK) |          \
                                        ((src[1] RGB121_RSHIFT) RGB121_RMASK)] << 8);    \
                     dest += 2; src += 2;                                  \
}
#define WRITE4_RGBA_RGB121(src, dest)                                      \
{                                                                          \
 *((DATA32 *)dest) = (_dither_color_lut[((src[0] RGB121_BSHIFT) RGB121_BMASK) |          \
                                        ((src[0] RGB121_GSHIFT) RGB121_GMASK) |          \
                                        ((src[0] RGB121_RSHIFT) RGB121_RMASK)]) |        \
                     (_dither_color_lut[((src[1] RGB121_BSHIFT) RGB121_BMASK) |          \
                                        ((src[1] RGB121_GSHIFT) RGB121_GMASK) |          \
                                        ((src[1] RGB121_RSHIFT) RGB121_RMASK)] << 8) |   \
                     (_dither_color_lut[((src[2] RGB121_BSHIFT) RGB121_BMASK) |          \
                                        ((src[2] RGB121_GSHIFT) RGB121_GMASK) |          \
                                        ((src[2] RGB121_RSHIFT) RGB121_RMASK)] << 16) |  \
                     (_dither_color_lut[((src[3] RGB121_BSHIFT) RGB121_BMASK) |          \
                                        ((src[3] RGB121_GSHIFT) RGB121_GMASK) |          \
                                        ((src[3] RGB121_RSHIFT) RGB121_RMASK)] << 24);   \
                     dest += 4; src += 4;                                  \
}
#endif
/*****************************************************************************/
/* MACROS for dithered RGBA -> RGB121 conversion */
#define DITHER_RGBA_121_LUT_R(num) \
(_dither_r8[(((x + num) & 0x7) << 11) | ((y & 0x7) << 8) | ((src[num] >> 16) & 0xff)])
#define DITHER_RGBA_121_LUT_G(num) \
(_dither_g8[(((x + num) & 0x7) << 11) | ((y & 0x7) << 8) | ((src[num] >> 8)  & 0xff)])
#define DITHER_RGBA_121_LUT_B(num) \
(_dither_b8[(((x + num) & 0x7) << 11) | ((y & 0x7) << 8) | ((src[num] >> 0) & 0xff)])

#define WRITE1_RGBA_RGB121_DITHER(src, dest)                                   \
*dest = _dither_color_lut[(DITHER_RGBA_121_LUT_R(0)) |                         \
                          (DITHER_RGBA_121_LUT_G(0)) |                         \
                          (DITHER_RGBA_121_LUT_B(0))]; dest++; src++
#ifdef WORDS_BIGENDIAN
#define WRITE4_RGBA_RGB121_DITHER(src, dest)                                   \
{                                                                              \
 *((DATA32 *)dest) = (_dither_color_lut[((DITHER_RGBA_121_LUT_R(3))) |         \
                                        ((DITHER_RGBA_121_LUT_G(3))) |         \
                                        ((DITHER_RGBA_121_LUT_B(3)))]) |       \
                     (_dither_color_lut[((DITHER_RGBA_121_LUT_R(2))) |         \
                                        ((DITHER_RGBA_121_LUT_G(2))) |         \
                                        ((DITHER_RGBA_121_LUT_B(2)))] << 8) |  \
                     (_dither_color_lut[((DITHER_RGBA_121_LUT_R(1))) |         \
                                        ((DITHER_RGBA_121_LUT_G(1))) |         \
                                        ((DITHER_RGBA_121_LUT_B(1)))] << 16) | \
                     (_dither_color_lut[((DITHER_RGBA_121_LUT_R(0))) |         \
                                        ((DITHER_RGBA_121_LUT_G(0))) |         \
                                        ((DITHER_RGBA_121_LUT_B(0)))] << 24);  \
                     dest += 4; src += 4;                                      \
}
#define WRITE2_RGBA_RGB121_DITHER(src, dest)                                   \
{                                                                              \
 *((DATA32 *)dest) = (_dither_color_lut[((DITHER_RGBA_121_LUT_R(1))) |         \
                                        ((DITHER_RGBA_121_LUT_G(1))) |         \
                                        ((DITHER_RGBA_121_LUT_B(1)))]) |       \
                     (_dither_color_lut[((DITHER_RGBA_121_LUT_R(0))) |         \
                                        ((DITHER_RGBA_121_LUT_G(0))) |         \
                                        ((DITHER_RGBA_121_LUT_B(0)))] << 8);   \
                     dest += 2; src += 2;                                      \
}
#else
#define WRITE4_RGBA_RGB121_DITHER(src, dest)                                   \
{                                                                              \
 *((DATA32 *)dest) = (_dither_color_lut[((DITHER_RGBA_121_LUT_R(0))) |         \
                                        ((DITHER_RGBA_121_LUT_G(0))) |         \
                                        ((DITHER_RGBA_121_LUT_B(0)))]) |       \
                     (_dither_color_lut[((DITHER_RGBA_121_LUT_R(1))) |         \
                                        ((DITHER_RGBA_121_LUT_G(1))) |         \
                                        ((DITHER_RGBA_121_LUT_B(1)))] << 8) |  \
                     (_dither_color_lut[((DITHER_RGBA_121_LUT_R(2))) |         \
                                        ((DITHER_RGBA_121_LUT_G(2))) |         \
                                        ((DITHER_RGBA_121_LUT_B(2)))] << 16) | \
                     (_dither_color_lut[((DITHER_RGBA_121_LUT_R(3))) |         \
                                        ((DITHER_RGBA_121_LUT_G(3))) |         \
                                        ((DITHER_RGBA_121_LUT_B(3)))] << 24);  \
                     dest += 4; src += 4;                                      \
}
#define WRITE2_RGBA_RGB121_DITHER(src, dest)                                   \
{                                                                              \
 *((DATA32 *)dest) = (_dither_color_lut[((DITHER_RGBA_121_LUT_R(0))) |         \
                                        ((DITHER_RGBA_121_LUT_G(0))) |         \
                                        ((DITHER_RGBA_121_LUT_B(0)))]) |       \
                     (_dither_color_lut[((DITHER_RGBA_121_LUT_R(1))) |         \
                                        ((DITHER_RGBA_121_LUT_G(1))) |         \
                                        ((DITHER_RGBA_121_LUT_B(1)))] << 8);   \
                     dest += 2; src += 2;                                      \
}
#endif

/*****************************************************************************/
/* Actual rendering routines                                                 */
/* RGBA -> RGB111                                                            */
/*****************************************************************************/

/*****************************************************************************/
/* MACROS for plain RGBA -> RGB111 conversion */
#define RGB111_BSHIFT >> 7
#define RGB111_GSHIFT >> 14
#define RGB111_RSHIFT >> 21
#define RGB111_BMASK & 0x01
#define RGB111_GMASK & 0x02
#define RGB111_RMASK & 0x04

#define WRITE1_RGBA_RGB111(src, dest)        \
*dest = _dither_color_lut[((*src RGB111_BSHIFT) RGB111_BMASK) |                          \
                          ((*src RGB111_GSHIFT) RGB111_GMASK) |                          \
                          ((*src RGB111_RSHIFT) RGB111_RMASK)]; dest++; src++
#ifdef WORDS_BIGENDIAN
#define WRITE2_RGBA_RGB111(src, dest)                                      \
{                                                                          \
 *((DATA16 *)dest) = (_dither_color_lut[((src[1] RGB111_BSHIFT) RGB111_BMASK) |          \
                                        ((src[1] RGB111_GSHIFT) RGB111_GMASK) |          \
                                        ((src[1] RGB111_RSHIFT) RGB111_RMASK)]) |        \
                     (_dither_color_lut[((src[0] RGB111_BSHIFT) RGB111_BMASK) |          \
                                        ((src[0] RGB111_GSHIFT) RGB111_GMASK) |          \
                                        ((src[0] RGB111_RSHIFT) RGB111_RMASK)] << 8);    \
                     dest += 2; src += 2;                                  \
}
#define WRITE4_RGBA_RGB111(src, dest)                                      \
{                                                                          \
 *((DATA32 *)dest) = (_dither_color_lut[((src[3] RGB111_BSHIFT) RGB111_BMASK) |          \
                                        ((src[3] RGB111_GSHIFT) RGB111_GMASK) |          \
                                        ((src[3] RGB111_RSHIFT) RGB111_RMASK)]) |        \
                     (_dither_color_lut[((src[2] RGB111_BSHIFT) RGB111_BMASK) |          \
                                        ((src[2] RGB111_GSHIFT) RGB111_GMASK) |          \
                                        ((src[2] RGB111_RSHIFT) RGB111_RMASK)] << 8) |   \
                     (_dither_color_lut[((src[1] RGB111_BSHIFT) RGB111_BMASK) |          \
                                        ((src[1] RGB111_GSHIFT) RGB111_GMASK) |          \
                                        ((src[1] RGB111_RSHIFT) RGB111_RMASK)] << 16) |  \
                     (_dither_color_lut[((src[0] RGB111_BSHIFT) RGB111_BMASK) |          \
                                        ((src[0] RGB111_GSHIFT) RGB111_GMASK) |          \
                                        ((src[0] RGB111_RSHIFT) RGB111_RMASK)] << 24);   \
                     dest += 4; src += 4;                                  \
}
#else
#define WRITE2_RGBA_RGB111(src, dest)                                      \
{                                                                          \
 *((DATA16 *)dest) = (_dither_color_lut[((src[0] RGB111_BSHIFT) RGB111_BMASK) |          \
                                        ((src[0] RGB111_GSHIFT) RGB111_GMASK) |          \
                                        ((src[0] RGB111_RSHIFT) RGB111_RMASK)]) |        \
                     (_dither_color_lut[((src[1] RGB111_BSHIFT) RGB111_BMASK) |          \
                                        ((src[1] RGB111_GSHIFT) RGB111_GMASK) |          \
                                        ((src[1] RGB111_RSHIFT) RGB111_RMASK)] << 8);    \
                     dest += 2; src += 2;                                  \
}
#define WRITE4_RGBA_RGB111(src, dest)                                      \
{                                                                          \
 *((DATA32 *)dest) = (_dither_color_lut[((src[0] RGB111_BSHIFT) RGB111_BMASK) |          \
                                        ((src[0] RGB111_GSHIFT) RGB111_GMASK) |          \
                                        ((src[0] RGB111_RSHIFT) RGB111_RMASK)]) |        \
                     (_dither_color_lut[((src[1] RGB111_BSHIFT) RGB111_BMASK) |          \
                                        ((src[1] RGB111_GSHIFT) RGB111_GMASK) |          \
                                        ((src[1] RGB111_RSHIFT) RGB111_RMASK)] << 8) |   \
                     (_dither_color_lut[((src[2] RGB111_BSHIFT) RGB111_BMASK) |          \
                                        ((src[2] RGB111_GSHIFT) RGB111_GMASK) |          \
                                        ((src[2] RGB111_RSHIFT) RGB111_RMASK)] << 16) |  \
                     (_dither_color_lut[((src[3] RGB111_BSHIFT) RGB111_BMASK) |          \
                                        ((src[3] RGB111_GSHIFT) RGB111_GMASK) |          \
                                        ((src[3] RGB111_RSHIFT) RGB111_RMASK)] << 24);   \
                     dest += 4; src += 4;                                  \
}
#endif
/*****************************************************************************/
/* MACROS for dithered RGBA -> RGB111 conversion */
#define DITHER_RGBA_111_LUT_R(num) \
(_dither_r8[(((x + num) & 0x7) << 11) | ((y & 0x7) << 8) | ((src[num] >> 16) & 0xff)])
#define DITHER_RGBA_111_LUT_G(num) \
(_dither_g8[(((x + num) & 0x7) << 11) | ((y & 0x7) << 8) | ((src[num] >> 8)  & 0xff)])
#define DITHER_RGBA_111_LUT_B(num) \
(_dither_b8[(((x + num) & 0x7) << 11) | ((y & 0x7) << 8) | ((src[num] >> 0) & 0xff)])

#define WRITE1_RGBA_RGB111_DITHER(src, dest)                                   \
*dest = _dither_color_lut[(DITHER_RGBA_111_LUT_R(0)) |                         \
                          (DITHER_RGBA_111_LUT_G(0)) |                         \
                          (DITHER_RGBA_111_LUT_B(0))]; dest++; src++
#ifdef WORDS_BIGENDIAN
#define WRITE4_RGBA_RGB111_DITHER(src, dest)                                   \
{                                                                              \
 *((DATA32 *)dest) = (_dither_color_lut[((DITHER_RGBA_111_LUT_R(3))) |         \
                                        ((DITHER_RGBA_111_LUT_G(3))) |         \
                                        ((DITHER_RGBA_111_LUT_B(3)))]) |       \
                     (_dither_color_lut[((DITHER_RGBA_111_LUT_R(2))) |         \
                                        ((DITHER_RGBA_111_LUT_G(2))) |         \
                                        ((DITHER_RGBA_111_LUT_B(2)))] << 8) |  \
                     (_dither_color_lut[((DITHER_RGBA_111_LUT_R(1))) |         \
                                        ((DITHER_RGBA_111_LUT_G(1))) |         \
                                        ((DITHER_RGBA_111_LUT_B(1)))] << 16) | \
                     (_dither_color_lut[((DITHER_RGBA_111_LUT_R(0))) |         \
                                        ((DITHER_RGBA_111_LUT_G(0))) |         \
                                        ((DITHER_RGBA_111_LUT_B(0)))] << 24);  \
                     dest += 4; src += 4;                                      \
}
#define WRITE2_RGBA_RGB111_DITHER(src, dest)                                   \
{                                                                              \
 *((DATA32 *)dest) = (_dither_color_lut[((DITHER_RGBA_111_LUT_R(1))) |         \
                                        ((DITHER_RGBA_111_LUT_G(1))) |         \
                                        ((DITHER_RGBA_111_LUT_B(1)))]) |       \
                     (_dither_color_lut[((DITHER_RGBA_111_LUT_R(0))) |         \
                                        ((DITHER_RGBA_111_LUT_G(0))) |         \
                                        ((DITHER_RGBA_111_LUT_B(0)))] << 8);   \
                     dest += 2; src += 2;                                      \
}
#else
#define WRITE4_RGBA_RGB111_DITHER(src, dest)                                   \
{                                                                              \
 *((DATA32 *)dest) = (_dither_color_lut[((DITHER_RGBA_111_LUT_R(0))) |         \
                                        ((DITHER_RGBA_111_LUT_G(0))) |         \
                                        ((DITHER_RGBA_111_LUT_B(0)))]) |       \
                     (_dither_color_lut[((DITHER_RGBA_111_LUT_R(1))) |         \
                                        ((DITHER_RGBA_111_LUT_G(1))) |         \
                                        ((DITHER_RGBA_111_LUT_B(1)))] << 8) |  \
                     (_dither_color_lut[((DITHER_RGBA_111_LUT_R(2))) |         \
                                        ((DITHER_RGBA_111_LUT_G(2))) |         \
                                        ((DITHER_RGBA_111_LUT_B(2)))] << 16) | \
                     (_dither_color_lut[((DITHER_RGBA_111_LUT_R(3))) |         \
                                        ((DITHER_RGBA_111_LUT_G(3))) |         \
                                        ((DITHER_RGBA_111_LUT_B(3)))] << 24);  \
                     dest += 4; src += 4;                                      \
}
#define WRITE2_RGBA_RGB111_DITHER(src, dest)                                   \
{                                                                              \
 *((DATA32 *)dest) = (_dither_color_lut[((DITHER_RGBA_111_LUT_R(0))) |         \
                                        ((DITHER_RGBA_111_LUT_G(0))) |         \
                                        ((DITHER_RGBA_111_LUT_B(0)))]) |       \
                     (_dither_color_lut[((DITHER_RGBA_111_LUT_R(1))) |         \
                                        ((DITHER_RGBA_111_LUT_G(1))) |         \
                                        ((DITHER_RGBA_111_LUT_B(1)))] << 8);   \
                     dest += 2; src += 2;                                      \
}
#endif

/*****************************************************************************/
/* Actual rendering routines                                                 */
/* RGBA -> RGB1 (mono B&W)                                                   */
/*****************************************************************************/

/*****************************************************************************/
/* MACROS for plain RGBA -> RGB1 conversion */

#define WRITE1_RGBA_RGB1(src, dest)                                   \
*dest = _dither_color_lut[((((*src >> 0) & 0xff) +                    \
                           ((*src >>  8) & 0xff) +                    \
                           ((*src >> 16) & 0xff)) / 3) >> 7]; dest++; src++
/*****************************************************************************/
/* MACROS for dithered RGBA -> RGB1 conversion */
#define DITHER_RGBA_1_LUT(num) \
(_dither_r8[(((x + num) & 0x7) << 11) |   \
            ((y & 0x7)         << 8) |    \
            ((((*src >> 0) & 0xff) +      \
             ((*src >>  8) & 0xff) +      \
             ((*src >> 16) & 0xff)) / 3)])

#define WRITE1_RGBA_RGB1_DITHER(src, dest)                        \
*dest = _dither_color_lut[(DITHER_RGBA_1_LUT(0))]; dest++; src++

/*****************************************************************************/
/* Actual rendering routines                                                 */
/* RGBA -> A1 (mono B&W - alpha mas)                                         */
/*****************************************************************************/

/*****************************************************************************/
/* MACROS for plain RGBA -> A1 conversion */

#ifdef WORDS_BIGENDIAN
# define WRITE1_RGBA_A1(src, dest)                   \
*dest |= ((*src & 0x80000000) >> (24 + (x & 0x7)));  \
if ((x & 0x7) == 0x7) dest++;                        \
src++
#else
# define WRITE1_RGBA_A1(src, dest)                   \
*dest |= ((*src & 0x80000000) >> (31 - (x & 0x7))); \
if ((x & 0x7) == 0x7) dest++;                       \
src++
#endif

/*****************************************************************************/
/* MACROS for dithered RGBA -> A1 conversion */
# define DITHER_RGBA_A1_LUT(num) \
(_dither_a1[(((x + num) & 0x7) << 11) | ((y & 0x7) << 8) | ((src[num] >> 24))])
#ifdef WORDS_BIGENDIAN
#define WRITE1_RGBA_A1_DITHER(src, dest)              \
*dest |= (DITHER_RGBA_A1_LUT(0)) << (7 - (x & 0x7)); \
if ((x & 0x7) == 0x7) dest++;                         \
src++;
#else
#define WRITE1_RGBA_A1_DITHER(src, dest)              \
*dest |= (DITHER_RGBA_A1_LUT(0)) << (0 + (x & 0x7)); \
if ((x & 0x7) == 0x7) dest++;                         \
src++;
#endif

/*****************************************************************************/
/* Actual rendering routines                                                 */
/* RGBA -> RGB8888                                                           */
/*****************************************************************************/

/*****************************************************************************/
/* MACROS for plain RGBA -> RGB8888 conversion */
#define WRITE1_RGBA_RGB8888(src, dest)             \
*dest = *src; dest++; src++;
# define WRITE1_RGBA_BGR8888(src, dest)            \
*dest = (((*src) >> 16) & 0x0000ff) |              \
        (((*src)      ) & 0x00ff00) |              \
        (((*src) << 16) & 0xff0000); dest++; src++;

/*****************************************************************************/
/* Actual rendering routines                                                 */
/* RGBA -> RGB888                                                            */
/*****************************************************************************/

/*****************************************************************************/
/* MACROS for plain RGBA -> RGB888 conversion */
#define WRITE1_RGBA_RGB888(src, dest)             \
*dest = ((*src >> 0)   & 0xff); dest++;           \
*dest = ((*src >> 8)   & 0xff); dest++;           \
*dest = ((*src >> 16)  & 0xff); dest++; src++;
#define WRITE1_RGBA_BGR888(src, dest)             \
*dest = ((*src >> 16)  & 0xff); dest++;           \
*dest = ((*src >> 8)   & 0xff); dest++;           \
*dest = ((*src >> 0)   & 0xff); dest++; src++;

void
__imlib_RGBASetupContext(Context *ct)
{
   _dither_color_lut = ct->palette;
   _pal_type = ct->palette_type;

   if ((ct->depth == 16) || (ct->depth == 15))
     {
	_dither_r16 = (DATA16 *)ct->r_dither;
	_dither_g16 = (DATA16 *)ct->g_dither;
	_dither_b16 = (DATA16 *)ct->b_dither;
     }
   else if (ct->depth <= 8)
     {
	switch (_pal_type)
	  {
	  case 0:
	  case 1:
	  case 2:
	  case 3:
	  case 4:
	  case 5:
	  case 7:
	     _dither_r8 = (DATA8 *)ct->r_dither;
	     _dither_g8 = (DATA8 *)ct->g_dither;
	     _dither_b8 = (DATA8 *)ct->b_dither;
	     break;
	  case 6:
	     _dither_r8 = (DATA8 *)ct->r_dither;
	     break;
	  default:
	     break;
	  }
     }
   _dither_r8 = (DATA8 *)ct->r_dither;
}
   
/* Palette mode stuff */

void
__imlib_RGBA_init(void *rd, void *gd, void *bd, int depth, DATA8 palette_type)
{
   DATA16 *rd16, *gd16, *bd16;
   DATA8 *rd8, *gd8, *bd8;
   /* the famous dither matrix */
   const DATA8 _dither_44[4][4] =
     {
	  {0, 4, 1, 5},
	  {6, 2, 7, 3},
	  {1, 5, 0, 4},
	  {7, 3, 6, 2}
     };
   const DATA8 _dither_88[8][8] =
     {
	  { 0,  32, 8,  40, 2,  34, 10, 42 },
	  { 48, 16, 56, 24, 50, 18, 58, 26 },
	  { 12, 44, 4,  36, 14, 46, 6,  38 },
	  { 60, 28, 52, 20, 62, 30, 54, 22 },
	  { 3,  35, 11, 43, 1,  33, 9,  41 },
	  { 51, 19, 59, 27, 49, 17, 57, 25 },
	  { 15, 47, 7,  39, 13, 45, 5,  37 },
	  { 63, 31, 55, 23, 61, 29, 53, 21 }
     };
   int i, x, y;

   if (!dither_a_init)
     {
	for (y = 0; y < 8; y++)
	  {
	     for (x = 0; x < 8; x++)
	       {
		  for (i = 0; i < 256; i++)
		    {
		       int pi;

		       pi = (i * (256 - 128)) / 255;
		       if ((_dither_88[x][y] < ((pi & 0x7f) >> 1)) && (pi < (256 - 128)))
			  _dither_a1[(x << 11) | (y << 8) | i] = (((pi + 128) >> 7) & 0x01);
		       else
			  _dither_a1[(x << 11) | (y << 8) | i] = ((pi >> 7) & 0x01);
		    }
	       }
	  }
	dither_a_init = 1;
     }
   switch (depth)
     {
     case 16:
	rd16 = (DATA16 *)rd;
	gd16 = (DATA16 *)gd;
	bd16 = (DATA16 *)bd;
	for (y = 0; y < 4; y++)
	  {
	     for (x = 0; x < 4; x++)
	       {
		  for (i = 0; i < 256; i++)
		    {
		       if ((_dither_44[x][y] < (i & 0x7)) && (i < (256 - 8)))
			  rd16[(x << 10) | (y << 8) | i] = ((i + 8) & 0xf8) << 8;
		       else
			  rd16[(x << 10) | (y << 8) | i] = (i & 0xf8) << 8;
		       
		       if ((_dither_44[x][y] < ((i & 0x3) << 1)) && (i < (256 - 4)))
			  gd16[(x << 10) | (y << 8) | i] = (((i + 4) & 0xfc) << 8) >> 5;
		       else
			  gd16[(x << 10) | (y << 8) | i] = ((i & 0xfc) << 8) >> 5;
		       
		       if ((_dither_44[x][y] < (i & 0x7)) && (i < (256 - 8)))
			  bd16[(x << 10) | (y << 8) | i] = (((i + 8) & 0xf8) << 16) >> 19;
		       else
			  bd16[(x << 10) | (y << 8) | i] = ((i & 0xf8) << 16) >> 19;
		    }
	       }
	  }
	break;
     case 15:
	rd16 = (DATA16 *)rd;
	gd16 = (DATA16 *)gd;
	bd16 = (DATA16 *)bd;
	for (y = 0; y < 4; y++)
	  {
	     for (x = 0; x < 4; x++)
	       {
		  for (i = 0; i < 256; i++)
		    {	      
		       if ((_dither_44[x][y] < (i & 0x7)) && (i < (256 - 8)))
			  rd16[(x << 10) | (y << 8) | i] = (((i + 8) & 0xf8) << 8) >> 1;
		       else
			  rd16[(x << 10) | (y << 8) | i] = ((i & 0xf8) << 8) >> 1;
		       
		       if ((_dither_44[x][y] < (i & 0x7)) && (i < (256 - 8)))
			  gd16[(x << 10) | (y << 8) | i] = (((i + 8) & 0xf8) << 8) >> 6;
		       else
			  gd16[(x << 10) | (y << 8) | i] = ((i & 0xf8) << 8) >> 6;
		       
		       if ((_dither_44[x][y] < (i & 0x7)) && (i < (256 - 8)))
			  bd16[(x << 10) | (y << 8) | i] = (((i + 8) & 0xf8) << 16) >> 19;
		       else
			  bd16[(x << 10) | (y << 8) | i] = ((i & 0xf8) << 16) >> 19;
		    }
	       }
	  }
	break;
     default:
	rd8 = (DATA8 *)rd;
	gd8 = (DATA8 *)gd;
	bd8 = (DATA8 *)bd;
	switch (palette_type)
	  {
	  case 0:
	     for (y = 0; y < 8; y++)
	       {
		  for (x = 0; x < 8; x++)
		    {
		       for (i = 0; i < 256; i++)
			 {
			   int pi;

			   pi = (i * (256 - 32)) / 255;
			   if ((_dither_88[x][y] < ((pi & 0x1f) << 1)) && (pi < (256 - 32)))
			     rd8[(x << 11) | (y << 8) | i] = ((pi + 32) & 0xe0);
			   else
			     rd8[(x << 11) | (y << 8) | i] = (pi & 0xe0);
			   
			   pi = (i * (256 - 32)) / 255;
			   if ((_dither_88[x][y] < ((pi & 0x1f) << 1)) && (pi < (256 - 32)))
			     gd8[(x << 11) | (y << 8) | i] = (((pi + 32) >> 3)& 0x1c);
			   else
			     gd8[(x << 11) | (y << 8) | i] = ((pi >> 3) & 0x1c);
			    
			   pi = (i * (256 - 64)) / 255;
			   if ((_dither_88[x][y] < (pi & 0x3f))        && (pi < (256 - 64)))
			     bd8[(x << 11) | (y << 8) | i] = (((pi + 64) >> 6)& 0x03);
			   else
			     bd8[(x << 11) | (y << 8) | i] = ((pi >> 6) & 0x03);
			 }
		    }
	       } 
	     break;
	  case 7: /* 666 8 bit 216 color rgb cube */
	     if (!_dither_666r)
	       {
		  _dither_666r = malloc(256 * sizeof(DATA8));
		  _dither_666g = malloc(256 * sizeof(DATA8));
		  _dither_666b = malloc(256 * sizeof(DATA8));
	       }
	     for (y = 0; y < 256; y++)
	       {
		  _dither_666r[y] = (DATA8)(((y * 6) >> 8) * 6 * 6);
		  _dither_666g[y] = (DATA8)(((y * 6) >> 8) * 6);
		  _dither_666b[y] = (DATA8)(((y * 6) >> 8));
	       }
	     for (y = 0; y < 8; y++)
	       {
		  for (x = 0; x < 8; x++)
		    {
		       for (i = 0; i < 256; i++)
			 {
			   double pi;

			   pi = 64.0 * (((double)i - (_dither_666b[i] * (256.0 / 6.0))) / (256.0 / 6.0));
			   if ((_dither_88[x][y] < (DATA8)pi) && ((double)i < (256 - (256.0 / 6.0))))
			      {
				 rd8[(x << 11) | (y << 8) | i] = (_dither_666b[i] + 1) * 6 * 6;
				 gd8[(x << 11) | (y << 8) | i] = (_dither_666b[i] + 1) * 6;
				 bd8[(x << 11) | (y << 8) | i] = (_dither_666b[i] + 1);
			      }
			   else
			      {
				 rd8[(x << 11) | (y << 8) | i] = (_dither_666b[i]) * 6 * 6;
				 gd8[(x << 11) | (y << 8) | i] = (_dither_666b[i]) * 6;
				 bd8[(x << 11) | (y << 8) | i] = (_dither_666b[i]);
			      }
			 }
		    }
	       } 
	     break;
	  case 1:
	     for (y = 0; y < 8; y++)
	       {
		  for (x = 0; x < 8; x++)
		    {
		       for (i = 0; i < 256; i++)
			 {
			    int pi;

			    pi = (i * (256 - 64)) / 255;
			    if ((_dither_88[x][y] < (pi & 0x3f))        && (pi < (256 - 64)))
			       rd8[(x << 11) | (y << 8) | i] = (((pi + 64) >> 1) & 0x60);
			    else
			       rd8[(x << 11) | (y << 8) | i] = ((pi >> 1) & 0x60);
			    
			    pi = (i * (256 - 32)) / 255;
			    if ((_dither_88[x][y] < ((pi & 0x1f) << 1)) && (pi < (256 - 32)))
			       gd8[(x << 11) | (y << 8) | i] = (((pi + 32) >> 3)& 0x1c);
			    else
			       gd8[(x << 11) | (y << 8) | i] = ((pi >> 3) & 0x1c);
			    
			    pi = (i * (256 - 64)) / 255;
			    if ((_dither_88[x][y] < (pi & 0x3f))        && (pi < (256 - 64)))
			       bd8[(x << 11) | (y << 8) | i] = (((pi + 64) >> 6)& 0x03);
			    else
			       bd8[(x << 11) | (y << 8) | i] = ((pi >> 6) & 0x03);
			 }
		    }
	       }
	     break;
	  case 2:
	     for (y = 0; y < 8; y++)
	       {
		  for (x = 0; x < 8; x++)
		    {
		       for (i = 0; i < 256; i++)
			 {
			    int pi;

			    pi = (i * (256 - 64)) / 255;
			    if ((_dither_88[x][y] < (pi & 0x3f))         && (pi < (256 - 64)))
			       rd8[(x << 11) | (y << 8) | i] = (((pi + 64) >> 2) & 0x30);
			    else
			       rd8[(x << 11) | (y << 8) | i] = ((pi >> 2) & 0x30);
			    
			    pi = (i * (256 - 64)) / 255;
			    if ((_dither_88[x][y] < (pi & 0x3f))        && (pi < (256 - 64)))
			       gd8[(x << 11) | (y << 8) | i] = (((pi + 64) >> 4)& 0x0c);
			    else
			       gd8[(x << 11) | (y << 8) | i] = ((pi >> 4) & 0x0c);
			    
			    pi = (i * (256 - 64)) / 255;
			    if ((_dither_88[x][y] < (pi & 0x3f))        && (pi < (256 - 64)))
			       bd8[(x << 11) | (y << 8) | i] = (((pi + 64) >> 6)& 0x03);
			    else
			       bd8[(x << 11) | (y << 8) | i] = ((pi >> 6) & 0x03);
			 }
		    }
	       }
	     break;
	  case 3:
	     for (y = 0; y < 8; y++)
	       {
		  for (x = 0; x < 8; x++)
		    {
		       for (i = 0; i < 256; i++)
			 {
			    int pi;

			    pi = (i * (256 - 64)) / 255;
			    if ((_dither_88[x][y] < (pi & 0x3f)) && (pi < (256 - 64)))
			       rd8[(x << 11) | (y << 8) | i] = (((pi + 64) >> 3) & 0x18);
			    else
			       rd8[(x << 11) | (y << 8) | i] = ((pi >> 3) & 0x18);
			    
			    pi = (i * (256 - 64)) / 255;
			    if ((_dither_88[x][y] < (pi & 0x3f)) && (pi < (256 - 64)))
			       gd8[(x << 11) | (y << 8) | i] = (((pi + 64) >> 5) & 0x06);
			    else
			       gd8[(x << 11) | (y << 8) | i] = ((pi >> 5) & 0x06);
			    
			    pi = (i * (256 - 128)) / 255;
			    if ((_dither_88[x][y] < ((pi & 0x7f) >> 1)) && (pi < (256 - 128)))
			       bd8[(x << 11) | (y << 8) | i] = (((pi + 128) >> 7) & 0x01);
			    else
			       bd8[(x << 11) | (y << 8) | i] = ((pi >> 7) & 0x01);
			 }
		    }
	       }
	     break;
	  case 4:
	     for (y = 0; y < 8; y++)
	       {
		  for (x = 0; x < 8; x++)
		    {
		       for (i = 0; i < 256; i++)
			 {
			    int pi;

			    pi = (i * (256 - 128)) / 255;
			    if ((_dither_88[x][y] < ((pi & 0x7f) >> 1)) && (pi < (256 - 128)))
			       rd8[(x << 11) | (y << 8) | i] = (((pi + 128) >> 4) & 0x08);
			    else
			       rd8[(x << 11) | (y << 8) | i] = ((pi >> 4) & 0x08);
			    
			    pi = (i * (256 - 64)) / 255;
			    if ((_dither_88[x][y] < (pi & 0x3f)) && (pi < (256 - 64)))
			       gd8[(x << 11) | (y << 8) | i] = (((pi + 64) >> 5) & 0x06);
			    else
			       gd8[(x << 11) | (y << 8) | i] = ((pi >> 5) & 0x06);
			    
			    pi = (i * (256 - 128)) / 255;
			    if ((_dither_88[x][y] < ((pi & 0x7f) >> 1)) && (pi < (256 - 128)))
			       bd8[(x << 11) | (y << 8) | i] = (((pi + 128) >> 7) & 0x01);
			    else
			       bd8[(x << 11) | (y << 8) | i] = ((pi >> 7) & 0x01);
			 }
		    }
	       }
	     break;
	  case 5:
	     for (y = 0; y < 8; y++)
	       {
		  for (x = 0; x < 8; x++)
		    {
		       for (i = 0; i < 256; i++)
			 {
			    int pi;

			    pi = (i * (256 - 128)) / 255;
			    if ((_dither_88[x][y] < ((pi & 0x7f) >> 1)) && (pi < (256 - 128)))
			       rd8[(x << 11) | (y << 8) | i] = (((pi + 128) >> 5) & 0x04);
			    else
			       rd8[(x << 11) | (y << 8) | i] = ((pi >> 5) & 0x04);
			    
			    pi = (i * (256 - 128)) / 255;
			    if ((_dither_88[x][y] < ((pi & 0x7f) >> 1)) && (pi < (256 - 128)))
			       gd8[(x << 11) | (y << 8) | i] = (((pi + 128) >> 6) & 0x02);
			    else
			       gd8[(x << 11) | (y << 8) | i] = ((pi >> 6) & 0x02);
			    
			    pi = (i * (256 - 128)) / 255;
			    if ((_dither_88[x][y] < ((pi & 0x7f) >> 1)) && (pi < (256 - 128)))
			       bd8[(x << 11) | (y << 8) | i] = (((pi + 128) >> 7) & 0x01);
			    else
			       bd8[(x << 11) | (y << 8) | i] = ((pi >> 7) & 0x01);
			 }
		    }
	       }
	     break;
	  case 6:
	     for (y = 0; y < 8; y++)
	       {
		  for (x = 0; x < 8; x++)
		    {
		       for (i = 0; i < 256; i++)
			 {
			    int pi;

			    pi = (i * (256 - 128)) / 255;
			    if ((_dither_88[x][y] < ((pi & 0x7f) >> 1)) && (pi < (256 - 128)))
			       rd8[(x << 11) | (y << 8) | i] = (((pi + 128) >> 7) & 0x01);
			    else
			       rd8[(x << 11) | (y << 8) | i] = ((pi >> 7) & 0x01);
			 }
		    }
	       }
	     break;
	  default:
	     break;
	  }
	break;
     }
}
	
static void
__imlib_RGBA_to_RGB565_fast(DATA32 *src , int src_jump, 
		    DATA8 *dst, int dow,
		    int width, int height, int dx, int dy)
{
  int x, y, w, h;
  DATA16 *dest = (DATA16 *)dst;
  int dest_jump = (dow / sizeof(DATA16)) - width;
  
  w = width;
  h = height;

  if (IS_ALIGNED_32((int)dest))
    {
      if (IS_MULTIPLE_2(width))
	{
	  for (y = 0; y < h; y++)
	    {
	      for (x = 0; x < w; x+=2)
		WRITE2_RGBA_RGB565(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else
	{
	  w--;
	  for (y = 0; y < h; y++)
	    {
	      for (x = 0; x < w; x+=2)
		WRITE2_RGBA_RGB565(src, dest);
	      WRITE1_RGBA_RGB565(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
    }
  else 
    {
      if (IS_MULTIPLE_2(width))
	{
	  w-=2;
	  for (y = 0; y < h; y++)
	    {
	      WRITE1_RGBA_RGB565(src, dest);
	      for (x = 0; x < w; x+=2)
		WRITE2_RGBA_RGB565(src, dest);
	      WRITE1_RGBA_RGB565(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else
	{
	  w--;
	  for (y = 0; y < h; y++)
	    {
	      WRITE1_RGBA_RGB565(src, dest);
	      for (x = 0; x < w; x+=2)
		WRITE2_RGBA_RGB565(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
    }
  return;
  dx = 0;
  dy = 0;
}

static void
__imlib_RGBA_to_RGB565_dither(DATA32 *src , int src_jump, 
		      DATA8 *dst, int dow,
		      int width, int height, int dx, int dy)
{
  int x, y, w, h;
  DATA16 *dest = (DATA16 *)dst;
  int dest_jump = (dow / sizeof(DATA16)) - width;
  
  w = width + dx;
  h = height + dy;
  
  if (IS_ALIGNED_32((int)dest))
    {
      if (IS_MULTIPLE_2(width))
	{
	  for (y = dy; y < h; y++)
	    {
	      for (x = dx; x < w; x+=2)
		WRITE2_RGBA_RGB565_DITHER(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else
	{
	  w--;
	  for (y = dy; y < h; y++)
	    {
	      for (x = dx; x < w; x+=2)
		WRITE2_RGBA_RGB565_DITHER(src, dest);
	      WRITE1_RGBA_RGB565_DITHER(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
    }
  else 
    {
      if (IS_MULTIPLE_2(width))
	{
	  w-=2;
	  for (y = dy; y < h; y++)
	    {
	      x = dx - 1;
	      WRITE1_RGBA_RGB565_DITHER(src, dest);
	      for (x = dx; x < w; x+=2)
		WRITE2_RGBA_RGB565_DITHER(src, dest);
	      WRITE1_RGBA_RGB565_DITHER(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else
	{
	  w--;
	  for (y = 0; y < h; y++)
	    {
	      x = dx - 1;
	      WRITE1_RGBA_RGB565_DITHER(src, dest);
	      for (x = dx; x < w; x+=2)
		WRITE2_RGBA_RGB565_DITHER(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
    }
  return;
  dx = 0;
  dy = 0;
}

static void
__imlib_RGBA_to_BGR565_fast(DATA32 *src , int src_jump, 
			    DATA8 *dst, int dow,
			    int width, int height, int dx, int dy)
{
  int x, y, w, h;
  DATA16 *dest = (DATA16 *)dst;
  int dest_jump = (dow / sizeof(DATA16)) - width;
  
  w = width;
  h = height;

  if (IS_ALIGNED_32((int)dest))
    {
      if (IS_MULTIPLE_2(width))
	{
	  for (y = 0; y < h; y++)
	    {
	      for (x = 0; x < w; x+=2)
		WRITE2_RGBA_BGR565(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else
	{
	  w--;
	  for (y = 0; y < h; y++)
	    {
	      for (x = 0; x < w; x+=2)
		WRITE2_RGBA_BGR565(src, dest);
	      WRITE1_RGBA_BGR565(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
    }
  else 
    {
      if (IS_MULTIPLE_2(width))
	{
	  w-=2;
	  for (y = 0; y < h; y++)
	    {
	      WRITE1_RGBA_BGR565(src, dest);
	      for (x = 0; x < w; x+=2)
		WRITE2_RGBA_BGR565(src, dest);
	      WRITE1_RGBA_BGR565(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else
	{
	  w--;
	  for (y = 0; y < h; y++)
	    {
	      WRITE1_RGBA_BGR565(src, dest);
	      for (x = 0; x < w; x+=2)
		WRITE2_RGBA_BGR565(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
    }
  return;
  dx = 0;
  dy = 0;
}

static void
__imlib_RGBA_to_BGR565_dither(DATA32 *src , int src_jump, 
		      DATA8 *dst, int dow,
		      int width, int height, int dx, int dy)
{
  int x, y, w, h;
  DATA16 *dest = (DATA16 *)dst;
  int dest_jump = (dow / sizeof(DATA16)) - width;
  
  w = width + dx;
  h = height + dy;
  
  if (IS_ALIGNED_32((int)dest))
    {
      if (IS_MULTIPLE_2(width))
	{
	  for (y = dy; y < h; y++)
	    {
	      for (x = dx; x < w; x+=2)
		WRITE2_RGBA_BGR565_DITHER(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else
	{
	  w--;
	  for (y = dy; y < h; y++)
	    {
	      for (x = dx; x < w; x+=2)
		WRITE2_RGBA_BGR565_DITHER(src, dest);
	      WRITE1_RGBA_BGR565_DITHER(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
    }
  else 
    {
      if (IS_MULTIPLE_2(width))
	{
	  w-=2;
	  for (y = dy; y < h; y++)
	    {
	      x = dx - 1;
	      WRITE1_RGBA_BGR565_DITHER(src, dest);
	      for (x = dx; x < w; x+=2)
		WRITE2_RGBA_BGR565_DITHER(src, dest);
	      WRITE1_RGBA_BGR565_DITHER(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else
	{
	  w--;
	  for (y = 0; y < h; y++)
	    {
	      x = dx - 1;
	      WRITE1_RGBA_BGR565_DITHER(src, dest);
	      for (x = dx; x < w; x+=2)
		WRITE2_RGBA_BGR565_DITHER(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
    }
  return;
  dx = 0;
  dy = 0;
}

static void
__imlib_RGBA_to_RGB555_fast(DATA32 *src , int src_jump, 
		    DATA8 *dst, int dow,
		    int width, int height, int dx, int dy)
{
  int x, y, w, h;
  DATA16 *dest = (DATA16 *)dst;
  int dest_jump = (dow / sizeof(DATA16)) - width;
  
  w = width;
  h = height;
  
  if (IS_ALIGNED_32((int)dest))
    {
      if (IS_MULTIPLE_2(width))
	{
	  for (y = 0; y < h; y++)
	    {
	      for (x = 0; x < w; x+=2)
		WRITE2_RGBA_RGB555(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else
	{
	  w--;
	  for (y = 0; y < h; y++)
	    {
	      for (x = 0; x < w; x+=2)
		WRITE2_RGBA_RGB555(src, dest);
	      WRITE1_RGBA_RGB555(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
    }
  else 
    {
      if (IS_MULTIPLE_2(width))
	{
	  w-=2;
	  for (y = 0; y < h; y++)
	    {
	      WRITE1_RGBA_RGB555(src, dest);
	      for (x = 0; x < w; x+=2)
		WRITE2_RGBA_RGB555(src, dest);
	      WRITE1_RGBA_RGB555(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else
	{
	  w--;
	  for (y = 0; y < h; y++)
	    {
	      WRITE1_RGBA_RGB555(src, dest);
	      for (x = 0; x < w; x+=2)
		WRITE2_RGBA_RGB555(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
    }
  return;
  dx = 0;
  dy = 0;
}

static void
__imlib_RGBA_to_RGB555_dither(DATA32 *src , int src_jump, 
		      DATA8 *dst, int dow,
		      int width, int height, int dx, int dy)
{
  int x, y, w, h;
  DATA16 *dest = (DATA16 *)dst;
  int dest_jump = (dow / sizeof(DATA16)) - width;
  
  w = width + dx;
  h = height + dy;
  
  if (IS_ALIGNED_32((int)dest))
    {
      if (IS_MULTIPLE_2(width))
	{
	  for (y = dy; y < h; y++)
	    {
	      for (x = dx; x < w; x+=2)
		WRITE2_RGBA_RGB555_DITHER(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else
	{
	  w--;
	  for (y = dy; y < h; y++)
	    {
	      for (x = dx; x < w; x+=2)
		WRITE2_RGBA_RGB555_DITHER(src, dest);
	      WRITE1_RGBA_RGB555_DITHER(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
    }
  else 
    {
      if (IS_MULTIPLE_2(width))
	{
	  w-=2;
	  for (y = dy; y < h; y++)
	    {
	      x = dx - 1;
	      WRITE1_RGBA_RGB555_DITHER(src, dest);
	      for (x = dx; x < w; x+=2)
		WRITE2_RGBA_RGB555_DITHER(src, dest);
	      WRITE1_RGBA_RGB555_DITHER(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else
	{
	  w--;
	  for (y = dy; y < h; y++)
	    {
	      x = dx - 1;
	      WRITE1_RGBA_RGB555_DITHER(src, dest);
	      for (x = dx; x < w; x+=2)
		WRITE2_RGBA_RGB555_DITHER(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
    }
  return;
  dx = 0;
  dy = 0;
}

static void
__imlib_RGBA_to_BGR555_fast(DATA32 *src , int src_jump, 
		    DATA8 *dst, int dow,
		    int width, int height, int dx, int dy)
{
  int x, y, w, h;
  DATA16 *dest = (DATA16 *)dst;
  int dest_jump = (dow / sizeof(DATA16)) - width;
  
  w = width;
  h = height;
  
  if (IS_ALIGNED_32((int)dest))
    {
      if (IS_MULTIPLE_2(width))
	{
	  for (y = 0; y < h; y++)
	    {
	      for (x = 0; x < w; x+=2)
		WRITE2_RGBA_BGR555(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else
	{
	  w--;
	  for (y = 0; y < h; y++)
	    {
	      for (x = 0; x < w; x+=2)
		WRITE2_RGBA_BGR555(src, dest);
	      WRITE1_RGBA_BGR555(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
    }
  else 
    {
      if (IS_MULTIPLE_2(width))
	{
	  w-=2;
	  for (y = 0; y < h; y++)
	    {
	      WRITE1_RGBA_BGR555(src, dest);
	      for (x = 0; x < w; x+=2)
		WRITE2_RGBA_BGR555(src, dest);
	      WRITE1_RGBA_BGR555(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else
	{
	  w--;
	  for (y = 0; y < h; y++)
	    {
	      WRITE1_RGBA_BGR555(src, dest);
	      for (x = 0; x < w; x+=2)
		WRITE2_RGBA_BGR555(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
    }
  return;
  dx = 0;
  dy = 0;
}

static void
__imlib_RGBA_to_BGR555_dither(DATA32 *src , int src_jump, 
		      DATA8 *dst, int dow,
		      int width, int height, int dx, int dy)
{
  int x, y, w, h;
  DATA16 *dest = (DATA16 *)dst;
  int dest_jump = (dow / sizeof(DATA16)) - width;
  
  w = width + dx;
  h = height + dy;
  
  if (IS_ALIGNED_32((int)dest))
    {
      if (IS_MULTIPLE_2(width))
	{
	  for (y = dy; y < h; y++)
	    {
	      for (x = dx; x < w; x+=2)
		WRITE2_RGBA_BGR555_DITHER(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else
	{
	  w--;
	  for (y = dy; y < h; y++)
	    {
	      for (x = dx; x < w; x+=2)
		WRITE2_RGBA_BGR555_DITHER(src, dest);
	      WRITE1_RGBA_BGR555_DITHER(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
    }
  else 
    {
      if (IS_MULTIPLE_2(width))
	{
	  w-=2;
	  for (y = dy; y < h; y++)
	    {
	      x = dx - 1;
	      WRITE1_RGBA_BGR555_DITHER(src, dest);
	      for (x = dx; x < w; x+=2)
		WRITE2_RGBA_BGR555_DITHER(src, dest);
	      WRITE1_RGBA_BGR555_DITHER(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else
	{
	  w--;
	  for (y = dy; y < h; y++)
	    {
	      x = dx - 1;
	      WRITE1_RGBA_BGR555_DITHER(src, dest);
	      for (x = dx; x < w; x+=2)
		WRITE2_RGBA_BGR555_DITHER(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
    }
  return;
  dx = 0;
  dy = 0;
}

static void
__imlib_RGBA_to_RGB332_fast(DATA32 *src , int src_jump, 
		    DATA8 *dest, int dow,
		    int width, int height, int dx, int dy)
{
  int x, y, w, h;
  int dest_jump = dow - width;
  
  w = width;
  h = height;

  if (IS_ALIGNED_32((int)dest))
    {
      if (IS_MULTIPLE_4(width))
	{
	  for (y = 0; y < h; y++)
	    {
	      for (x = 0; x < w; x+=4)
		WRITE4_RGBA_RGB332(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else if (IS_MULTIPLE_2(width))
	{
	  w-=2;
	  for (y = 0; y < h; y++)
	    {
	      for (x = 0; x < w; x+=4)
		WRITE4_RGBA_RGB332(src, dest);
	      WRITE2_RGBA_RGB332(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else
	{
	  w-=3;
	  for (y = 0; y < h; y++)
	    {
	      for (x = 0; x < w; x+=4)
		WRITE4_RGBA_RGB332(src, dest);
	      for (; x < width; x++)
		{
		  WRITE1_RGBA_RGB332(src, dest);
		}
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
    }
  else 
    {
      for (y = 0; y < h; y++)
	{
	  for (x = 0; ((x < w) && (!(IS_ALIGNED_32((int)dest)))); x++)
	    {
	      WRITE1_RGBA_RGB332(src, dest);
	    }
	  if (x < w)
	    {
	      if (IS_MULTIPLE_4((width - x)))
		{
		  for (; x < w; x+=4)
		    WRITE4_RGBA_RGB332(src, dest);
		  src += src_jump;
		  dest += dest_jump;
		}
	      else if (IS_MULTIPLE_2((width - x)))
		{
		  w = width - 2 - x;
		  for (; x < w; x+=4)
		    WRITE4_RGBA_RGB332(src, dest);
		  WRITE2_RGBA_RGB332(src, dest);
		  src += src_jump;
		  dest += dest_jump;
		}
	      else
		{
		  w = width - 3 - x;
		  for (; x < w; x+=4)
		    WRITE4_RGBA_RGB332(src, dest);
		  for (; x < width; x++)
		    {
		      WRITE1_RGBA_RGB332(src, dest);
		    }
		  src += src_jump;
		  dest += dest_jump;
		}
	    }
	}
    }
  return;
  dx = 0;
  dy = 0;
}

static void
__imlib_RGBA_to_RGB332_dither(DATA32 *src , int src_jump, 
		      DATA8 *dest, int dow,
		      int width, int height, int dx, int dy)
{
  int x, y, w, h;
  int dest_jump = dow - width;

  w = width + dx;
  h = height + dy;

  if (IS_ALIGNED_32((int)dest))
    {
      if (IS_MULTIPLE_4(width))
	{
	  for (y = dy; y < h; y++)
	    {
	      for (x = dx; x < w; x+=4)
		WRITE4_RGBA_RGB332_DITHER(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else if (IS_MULTIPLE_2(width))
	{
	  w-=2;
	  for (y = dy; y < h; y++)
	    {
	      for (x = dx; x < w; x+=4)
		WRITE4_RGBA_RGB332_DITHER(src, dest);
	      WRITE2_RGBA_RGB332_DITHER(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else
	{
	  w-=3;
	  for (y = dy; y < h; y++)
	    {
	      for (x = dx; x < w; x+=4)
		WRITE4_RGBA_RGB332_DITHER(src, dest);
	      for (; x < (width + dx); x++)
		{
		  WRITE1_RGBA_RGB332_DITHER(src, dest);
		}
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
    }
  else 
    {
      for (y = dy; y < h; y++)
	{
	  w = width + dx;
	  for (x = dx; ((x < w) && (!(IS_ALIGNED_32((int)dest)))); x++)
	    {
	      WRITE1_RGBA_RGB332_DITHER(src, dest);
	    }
	  if (x < w)
	    {
	      w = (width + dx) - (3 + x);
	      for (; x < w; x+=4)
		WRITE4_RGBA_RGB332_DITHER(src, dest);
	      for (; x < (width + dx); x++)
		{
		  WRITE1_RGBA_RGB332_DITHER(src, dest);
		}
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
    }
  return;
  dx = 0;
  dy = 0;
}

static void
__imlib_RGBA_to_RGB666_fast(DATA32 *src , int src_jump, 
		    DATA8 *dest, int dow,
		    int width, int height, int dx, int dy)
{
  int x, y, w, h;
  int dest_jump = dow - width;
  
  w = width;
  h = height;

  if (IS_ALIGNED_32((int)dest))
    {
      if (IS_MULTIPLE_4(width))
	{
	  for (y = 0; y < h; y++)
	    {
	      for (x = 0; x < w; x+=4)
		WRITE4_RGBA_RGB666(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else if (IS_MULTIPLE_2(width))
	{
	  w-=2;
	  for (y = 0; y < h; y++)
	    {
	      for (x = 0; x < w; x+=4)
		WRITE4_RGBA_RGB666(src, dest);
	      WRITE2_RGBA_RGB666(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else
	{
	  w-=3;
	  for (y = 0; y < h; y++)
	    {
	      for (x = 0; x < w; x+=4)
		WRITE4_RGBA_RGB666(src, dest);
	      for (; x < width; x++)
		{
		  WRITE1_RGBA_RGB666(src, dest);
		}
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
    }
  else 
    {
      for (y = 0; y < h; y++)
	{
	  for (x = 0; ((x < w) && (!(IS_ALIGNED_32((int)dest)))); x++)
	    {
	      WRITE1_RGBA_RGB666(src, dest);
	    }
	  if (x < w)
	    {
	      if (IS_MULTIPLE_4((width - x)))
		{
		  for (; x < w; x+=4)
		    WRITE4_RGBA_RGB666(src, dest);
		  src += src_jump;
		  dest += dest_jump;
		}
	      else if (IS_MULTIPLE_2((width - x)))
		{
		  w = width - 2 - x;
		  for (; x < w; x+=4)
		    WRITE4_RGBA_RGB666(src, dest);
		  WRITE2_RGBA_RGB666(src, dest);
		  src += src_jump;
		  dest += dest_jump;
		}
	      else
		{
		  w = width - 3 - x;
		  for (; x < w; x+=4)
		    WRITE4_RGBA_RGB666(src, dest);
		  for (; x < width; x++)
		    {
		      WRITE1_RGBA_RGB666(src, dest);
		    }
		  src += src_jump;
		  dest += dest_jump;
		}
	    }
	}
    }
  return;
  dx = 0;
  dy = 0;
}

static void
__imlib_RGBA_to_RGB666_dither(DATA32 *src , int src_jump, 
		      DATA8 *dest, int dow,
		      int width, int height, int dx, int dy)
{
  int x, y, w, h;
  int dest_jump = dow - width;

  w = width + dx;
  h = height + dy;

  if (IS_ALIGNED_32((int)dest))
    {
      if (IS_MULTIPLE_4(width))
	{
	  for (y = dy; y < h; y++)
	    {
	      for (x = dx; x < w; x+=4)
		WRITE4_RGBA_RGB666_DITHER(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else if (IS_MULTIPLE_2(width))
	{
	  w-=2;
	  for (y = dy; y < h; y++)
	    {
	      for (x = dx; x < w; x+=4)
		WRITE4_RGBA_RGB666_DITHER(src, dest);
	      WRITE2_RGBA_RGB666_DITHER(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else
	{
	  w-=3;
	  for (y = dy; y < h; y++)
	    {
	      for (x = dx; x < w; x+=4)
		WRITE4_RGBA_RGB666_DITHER(src, dest);
	      for (; x < (width + dx); x++)
		{
		  WRITE1_RGBA_RGB666_DITHER(src, dest);
		}
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
    }
  else 
    {
      for (y = dy; y < h; y++)
	{
	  w = width + dx;
	  for (x = dx; ((x < w) && (!(IS_ALIGNED_32((int)dest)))); x++)
	    {
	      WRITE1_RGBA_RGB666_DITHER(src, dest);
	    }
	  if (x < w)
	    {
	      w = (width + dx) - (3 + x);
	      for (; x < w; x+=4)
		WRITE4_RGBA_RGB666_DITHER(src, dest);
	      for (; x < (width + dx); x++)
		{
		  WRITE1_RGBA_RGB666_DITHER(src, dest);
		}
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
    }
  return;
  dx = 0;
  dy = 0;
}

static void
__imlib_RGBA_to_RGB232_fast(DATA32 *src , int src_jump, 
		    DATA8 *dest, int dow,
		    int width, int height, int dx, int dy)
{
  int x, y, w, h;
  int dest_jump = dow - width;
  
  w = width;
  h = height;

  if (IS_ALIGNED_32((int)dest))
    {
      if (IS_MULTIPLE_4(width))
	{
	  for (y = 0; y < h; y++)
	    {
	      for (x = 0; x < w; x+=4)
		WRITE4_RGBA_RGB232(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else if (IS_MULTIPLE_2(width))
	{
	  w-=2;
	  for (y = 0; y < h; y++)
	    {
	      for (x = 0; x < w; x+=4)
		WRITE4_RGBA_RGB232(src, dest);
	      WRITE2_RGBA_RGB232(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else
	{
	  w-=3;
	  for (y = 0; y < h; y++)
	    {
	      for (x = 0; x < w; x+=4)
		WRITE4_RGBA_RGB232(src, dest);
	      for (; x < width; x++)
		{
		  WRITE1_RGBA_RGB232(src, dest);
		}
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
    }
  else 
    {
      for (y = 0; y < h; y++)
	{
	  for (x = 0; ((x < w) && (!(IS_ALIGNED_32((int)dest)))); x++)
	    {
	      WRITE1_RGBA_RGB232(src, dest);
	    }
	  if (x < w)
	    {
	      if (IS_MULTIPLE_4((width - x)))
		{
		  for (; x < w; x+=4)
		    WRITE4_RGBA_RGB232(src, dest);
		  src += src_jump;
		  dest += dest_jump;
		}
	      else if (IS_MULTIPLE_2((width - x)))
		{
		  w = width - 2 - x;
		  for (; x < w; x+=4)
		    WRITE4_RGBA_RGB232(src, dest);
		  WRITE2_RGBA_RGB232(src, dest);
		  src += src_jump;
		  dest += dest_jump;
		}
	      else
		{
		  w = width - 3 - x;
		  for (; x < w; x+=4)
		    WRITE4_RGBA_RGB232(src, dest);
		  for (; x < width; x++)
		    {
		      WRITE1_RGBA_RGB232(src, dest);
		    }
		  src += src_jump;
		  dest += dest_jump;
		}
	    }
	}
    }
  return;
  dx = 0;
  dy = 0;
}

static void
__imlib_RGBA_to_RGB232_dither(DATA32 *src , int src_jump, 
		      DATA8 *dest, int dow,
		      int width, int height, int dx, int dy)
{
  int x, y, w, h;
  int dest_jump = dow - width;

  w = width + dx;
  h = height + dy;

  if (IS_ALIGNED_32((int)dest))
    {
      if (IS_MULTIPLE_4(width))
	{
	  for (y = dy; y < h; y++)
	    {
	      for (x = dx; x < w; x+=4)
		WRITE4_RGBA_RGB232_DITHER(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else if (IS_MULTIPLE_2(width))
	{
	  w-=2;
	  for (y = dy; y < h; y++)
	    {
	      for (x = dx; x < w; x+=4)
		WRITE4_RGBA_RGB232_DITHER(src, dest);
	      WRITE2_RGBA_RGB232_DITHER(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else
	{
	  w-=3;
	  for (y = dy; y < h; y++)
	    {
	      for (x = dx; x < w; x+=4)
		WRITE4_RGBA_RGB232_DITHER(src, dest);
	      for (; x < (width + dx); x++)
		{
		  WRITE1_RGBA_RGB232_DITHER(src, dest);
		}
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
    }
  else 
    {
      for (y = dy; y < h; y++)
	{
	  w = width + dx;
	  for (x = dx; ((x < w) && (!(IS_ALIGNED_32((int)dest)))); x++)
	    {
	      WRITE1_RGBA_RGB232_DITHER(src, dest);
	    }
	  if (x < w)
	    {
	      w = (width + dx) - (3 + x);
	      for (; x < w; x+=4)
		WRITE4_RGBA_RGB232_DITHER(src, dest);
	      for (; x < (width + dx); x++)
		{
		  WRITE1_RGBA_RGB232_DITHER(src, dest);
		}
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
    }
  return;
  dx = 0;
  dy = 0;
}

static void
__imlib_RGBA_to_RGB222_fast(DATA32 *src , int src_jump, 
		    DATA8 *dest, int dow,
		    int width, int height, int dx, int dy)
{
  int x, y, w, h;
  int dest_jump = dow - width;
  
  w = width;
  h = height;

  if (IS_ALIGNED_32((int)dest))
    {
      if (IS_MULTIPLE_4(width))
	{
	  for (y = 0; y < h; y++)
	    {
	      for (x = 0; x < w; x+=4)
		WRITE4_RGBA_RGB222(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else if (IS_MULTIPLE_2(width))
	{
	  w-=2;
	  for (y = 0; y < h; y++)
	    {
	      for (x = 0; x < w; x+=4)
		WRITE4_RGBA_RGB222(src, dest);
	      WRITE2_RGBA_RGB222(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else
	{
	  w-=3;
	  for (y = 0; y < h; y++)
	    {
	      for (x = 0; x < w; x+=4)
		WRITE4_RGBA_RGB222(src, dest);
	      for (; x < width; x++)
		{
		  WRITE1_RGBA_RGB222(src, dest);
		}
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
    }
  else 
    {
      for (y = 0; y < h; y++)
	{
	  for (x = 0; ((x < w) && (!(IS_ALIGNED_32((int)dest)))); x++)
	    {
	      WRITE1_RGBA_RGB222(src, dest);
	    }
	  if (x < w)
	    {
	      if (IS_MULTIPLE_4((width - x)))
		{
		  for (; x < w; x+=4)
		    WRITE4_RGBA_RGB222(src, dest);
		  src += src_jump;
		  dest += dest_jump;
		}
	      else if (IS_MULTIPLE_2((width - x)))
		{
		  w = width - 2 - x;
		  for (; x < w; x+=4)
		    WRITE4_RGBA_RGB222(src, dest);
		  WRITE2_RGBA_RGB222(src, dest);
		  src += src_jump;
		  dest += dest_jump;
		}
	      else
		{
		  w = width - 3 - x;
		  for (; x < w; x+=4)
		    WRITE4_RGBA_RGB222(src, dest);
		  for (; x < width; x++)
		    {
		      WRITE1_RGBA_RGB222(src, dest);
		    }
		  src += src_jump;
		  dest += dest_jump;
		}
	    }
	}
    }
  return;
  dx = 0;
  dy = 0;
}

static void
__imlib_RGBA_to_RGB222_dither(DATA32 *src , int src_jump, 
		      DATA8 *dest, int dow,
		      int width, int height, int dx, int dy)
{
  int x, y, w, h;
  int dest_jump = dow - width;

  w = width + dx;
  h = height + dy;

  if (IS_ALIGNED_32((int)dest))
    {
      if (IS_MULTIPLE_4(width))
	{
	  for (y = dy; y < h; y++)
	    {
	      for (x = dx; x < w; x+=4)
		WRITE4_RGBA_RGB222_DITHER(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else if (IS_MULTIPLE_2(width))
	{
	  w-=2;
	  for (y = dy; y < h; y++)
	    {
	      for (x = dx; x < w; x+=4)
		WRITE4_RGBA_RGB222_DITHER(src, dest);
	      WRITE2_RGBA_RGB222_DITHER(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else
	{
	  w-=3;
	  for (y = dy; y < h; y++)
	    {
	      for (x = dx; x < w; x+=4)
		WRITE4_RGBA_RGB222_DITHER(src, dest);
	      for (; x < (width + dx); x++)
		{
		  WRITE1_RGBA_RGB222_DITHER(src, dest);
		}
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
    }
  else 
    {
      for (y = dy; y < h; y++)
	{
	  w = width + dx;
	  for (x = dx; ((x < w) && (!(IS_ALIGNED_32((int)dest)))); x++)
	    {
	      WRITE1_RGBA_RGB222_DITHER(src, dest);
	    }
	  if (x < w)
	    {
	      w = (width + dx) - (3 + x);
	      for (; x < w; x+=4)
		WRITE4_RGBA_RGB222_DITHER(src, dest);
	      for (; x < (width + dx); x++)
		{
		  WRITE1_RGBA_RGB222_DITHER(src, dest);
		}
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
    }
  return;
  dx = 0;
  dy = 0;
}

static void
__imlib_RGBA_to_RGB221_fast(DATA32 *src , int src_jump, 
		    DATA8 *dest, int dow,
		    int width, int height, int dx, int dy)
{
  int x, y, w, h;
  int dest_jump = dow - width;
  
  w = width;
  h = height;

  if (IS_ALIGNED_32((int)dest))
    {
      if (IS_MULTIPLE_4(width))
	{
	  for (y = 0; y < h; y++)
	    {
	      for (x = 0; x < w; x+=4)
		WRITE4_RGBA_RGB221(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else if (IS_MULTIPLE_2(width))
	{
	  w-=2;
	  for (y = 0; y < h; y++)
	    {
	      for (x = 0; x < w; x+=4)
		WRITE4_RGBA_RGB221(src, dest);
	      WRITE2_RGBA_RGB221(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else
	{
	  w-=3;
	  for (y = 0; y < h; y++)
	    {
	      for (x = 0; x < w; x+=4)
		WRITE4_RGBA_RGB221(src, dest);
	      for (; x < width; x++)
		{
		  WRITE1_RGBA_RGB221(src, dest);
		}
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
    }
  else 
    {
      for (y = 0; y < h; y++)
	{
	  for (x = 0; ((x < w) && (!(IS_ALIGNED_32((int)dest)))); x++)
	    {
	      WRITE1_RGBA_RGB221(src, dest);
	    }
	  if (x < w)
	    {
	      if (IS_MULTIPLE_4((width - x)))
		{
		  for (; x < w; x+=4)
		    WRITE4_RGBA_RGB221(src, dest);
		  src += src_jump;
		  dest += dest_jump;
		}
	      else if (IS_MULTIPLE_2((width - x)))
		{
		  w = width - 2 - x;
		  for (; x < w; x+=4)
		    WRITE4_RGBA_RGB221(src, dest);
		  WRITE2_RGBA_RGB221(src, dest);
		  src += src_jump;
		  dest += dest_jump;
		}
	      else
		{
		  w = width - 3 - x;
		  for (; x < w; x+=4)
		    WRITE4_RGBA_RGB221(src, dest);
		  for (; x < width; x++)
		    {
		      WRITE1_RGBA_RGB221(src, dest);
		    }
		  src += src_jump;
		  dest += dest_jump;
		}
	    }
	}
    }
  return;
  dx = 0;
  dy = 0;
}

static void
__imlib_RGBA_to_RGB221_dither(DATA32 *src , int src_jump, 
		      DATA8 *dest, int dow,
		      int width, int height, int dx, int dy)
{
  int x, y, w, h;
  int dest_jump = dow - width;

  w = width + dx;
  h = height + dy;

  if (IS_ALIGNED_32((int)dest))
    {
      if (IS_MULTIPLE_4(width))
	{
	  for (y = dy; y < h; y++)
	    {
	      for (x = dx; x < w; x+=4)
		WRITE4_RGBA_RGB221_DITHER(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else if (IS_MULTIPLE_2(width))
	{
	  w-=2;
	  for (y = dy; y < h; y++)
	    {
	      for (x = dx; x < w; x+=4)
		WRITE4_RGBA_RGB221_DITHER(src, dest);
	      WRITE2_RGBA_RGB221_DITHER(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else
	{
	  w-=3;
	  for (y = dy; y < h; y++)
	    {
	      for (x = dx; x < w; x+=4)
		WRITE4_RGBA_RGB221_DITHER(src, dest);
	      for (; x < (width + dx); x++)
		{
		  WRITE1_RGBA_RGB221_DITHER(src, dest);
		}
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
    }
  else 
    {
      for (y = dy; y < h; y++)
	{
	  w = width + dx;
	  for (x = dx; ((x < w) && (!(IS_ALIGNED_32((int)dest)))); x++)
	    {
	      WRITE1_RGBA_RGB221_DITHER(src, dest);
	    }
	  if (x < w)
	    {
	      w = (width + dx) - (3 + x);
	      for (; x < w; x+=4)
		WRITE4_RGBA_RGB221_DITHER(src, dest);
	      for (; x < (width + dx); x++)
		{
		  WRITE1_RGBA_RGB221_DITHER(src, dest);
		}
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
    }
  return;
  dx = 0;
  dy = 0;
}
  
static void
__imlib_RGBA_to_RGB121_fast(DATA32 *src , int src_jump, 
		    DATA8 *dest, int dow,
		    int width, int height, int dx, int dy)
{
  int x, y, w, h;
  int dest_jump = dow - width;
  
  w = width;
  h = height;

  if (IS_ALIGNED_32((int)dest))
    {
      if (IS_MULTIPLE_4(width))
	{
	  for (y = 0; y < h; y++)
	    {
	      for (x = 0; x < w; x+=4)
		WRITE4_RGBA_RGB121(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else if (IS_MULTIPLE_2(width))
	{
	  w-=2;
	  for (y = 0; y < h; y++)
	    {
	      for (x = 0; x < w; x+=4)
		WRITE4_RGBA_RGB121(src, dest);
	      WRITE2_RGBA_RGB121(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else
	{
	  w-=3;
	  for (y = 0; y < h; y++)
	    {
	      for (x = 0; x < w; x+=4)
		WRITE4_RGBA_RGB121(src, dest);
	      for (; x < width; x++)
		{
		  WRITE1_RGBA_RGB121(src, dest);
		}
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
    }
  else 
    {
      for (y = 0; y < h; y++)
	{
	  for (x = 0; ((x < w) && (!(IS_ALIGNED_32((int)dest)))); x++)
	    {
	      WRITE1_RGBA_RGB121(src, dest);
	    }
	  if (x < w)
	    {
	      if (IS_MULTIPLE_4((width - x)))
		{
		  for (; x < w; x+=4)
		    WRITE4_RGBA_RGB121(src, dest);
		  src += src_jump;
		  dest += dest_jump;
		}
	      else if (IS_MULTIPLE_2((width - x)))
		{
		  w = width - 2 - x;
		  for (; x < w; x+=4)
		    WRITE4_RGBA_RGB121(src, dest);
		  WRITE2_RGBA_RGB121(src, dest);
		  src += src_jump;
		  dest += dest_jump;
		}
	      else
		{
		  w = width - 3 - x;
		  for (; x < w; x+=4)
		    WRITE4_RGBA_RGB121(src, dest);
		  for (; x < width; x++)
		    {
		      WRITE1_RGBA_RGB121(src, dest);
		    }
		  src += src_jump;
		  dest += dest_jump;
		}
	    }
	}
    }
  return;
  dx = 0;
  dy = 0;
}

static void
__imlib_RGBA_to_RGB121_dither(DATA32 *src , int src_jump, 
		      DATA8 *dest, int dow,
		      int width, int height, int dx, int dy)
{
  int x, y, w, h;
  int dest_jump = dow - width;

  w = width + dx;
  h = height + dy;

  if (IS_ALIGNED_32((int)dest))
    {
      if (IS_MULTIPLE_4(width))
	{
	  for (y = dy; y < h; y++)
	    {
	      for (x = dx; x < w; x+=4)
		WRITE4_RGBA_RGB121_DITHER(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else if (IS_MULTIPLE_2(width))
	{
	  w-=2;
	  for (y = dy; y < h; y++)
	    {
	      for (x = dx; x < w; x+=4)
		WRITE4_RGBA_RGB121_DITHER(src, dest);
	      WRITE2_RGBA_RGB121_DITHER(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else
	{
	  w-=3;
	  for (y = dy; y < h; y++)
	    {
	      for (x = dx; x < w; x+=4)
		WRITE4_RGBA_RGB121_DITHER(src, dest);
	      for (; x < (width + dx); x++)
		{
		  WRITE1_RGBA_RGB121_DITHER(src, dest);
		}
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
    }
  else 
    {
      for (y = dy; y < h; y++)
	{
	  w = width + dx;
	  for (x = dx; ((x < w) && (!(IS_ALIGNED_32((int)dest)))); x++)
	    {
	      WRITE1_RGBA_RGB121_DITHER(src, dest);
	    }
	  if (x < w)
	    {
	      w = (width + dx) - (3 + x);
	      for (; x < w; x+=4)
		WRITE4_RGBA_RGB121_DITHER(src, dest);
	      for (; x < (width + dx); x++)
		{
		  WRITE1_RGBA_RGB121_DITHER(src, dest);
		}
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
    }
  return;
  dx = 0;
  dy = 0;
}
  
static void
__imlib_RGBA_to_RGB111_fast(DATA32 *src , int src_jump, 
		    DATA8 *dest, int dow,
		    int width, int height, int dx, int dy)
{
  int x, y, w, h;
  int dest_jump = dow - width;
  
  w = width;
  h = height;

  if (IS_ALIGNED_32((int)dest))
    {
      if (IS_MULTIPLE_4(width))
	{
	  for (y = 0; y < h; y++)
	    {
	      for (x = 0; x < w; x+=4)
		WRITE4_RGBA_RGB111(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else if (IS_MULTIPLE_2(width))
	{
	  w-=2;
	  for (y = 0; y < h; y++)
	    {
	      for (x = 0; x < w; x+=4)
		WRITE4_RGBA_RGB111(src, dest);
	      WRITE2_RGBA_RGB111(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else
	{
	  w-=3;
	  for (y = 0; y < h; y++)
	    {
	      for (x = 0; x < w; x+=4)
		WRITE4_RGBA_RGB111(src, dest);
	      for (; x < width; x++)
		{
		  WRITE1_RGBA_RGB111(src, dest);
		}
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
    }
  else 
    {
      for (y = 0; y < h; y++)
	{
	  for (x = 0; ((x < w) && (!(IS_ALIGNED_32((int)dest)))); x++)
	    {
	      WRITE1_RGBA_RGB111(src, dest);
	    }
	  if (x < w)
	    {
	      if (IS_MULTIPLE_4((width - x)))
		{
		  for (; x < w; x+=4)
		    WRITE4_RGBA_RGB111(src, dest);
		  src += src_jump;
		  dest += dest_jump;
		}
	      else if (IS_MULTIPLE_2((width - x)))
		{
		  w = width - 2 - x;
		  for (; x < w; x+=4)
		    WRITE4_RGBA_RGB111(src, dest);
		  WRITE2_RGBA_RGB111(src, dest);
		  src += src_jump;
		  dest += dest_jump;
		}
	      else
		{
		  w = width - 3 - x;
		  for (; x < w; x+=4)
		    WRITE4_RGBA_RGB111(src, dest);
		  for (; x < width; x++)
		    {
		      WRITE1_RGBA_RGB111(src, dest);
		    }
		  src += src_jump;
		  dest += dest_jump;
		}
	    }
	}
    }
  return;
  dx = 0;
  dy = 0;
}

static void
__imlib_RGBA_to_RGB111_dither(DATA32 *src , int src_jump, 
		      DATA8 *dest, int dow,
		      int width, int height, int dx, int dy)
{
  int x, y, w, h;
  int dest_jump = dow - width;

  w = width + dx;
  h = height + dy;

  if (IS_ALIGNED_32((int)dest))
    {
      if (IS_MULTIPLE_4(width))
	{
	  for (y = dy; y < h; y++)
	    {
	      for (x = dx; x < w; x+=4)
		WRITE4_RGBA_RGB111_DITHER(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else if (IS_MULTIPLE_2(width))
	{
	  w-=2;
	  for (y = dy; y < h; y++)
	    {
	      for (x = dx; x < w; x+=4)
		WRITE4_RGBA_RGB111_DITHER(src, dest);
	      WRITE2_RGBA_RGB111_DITHER(src, dest);
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
      else
	{
	  w-=3;
	  for (y = dy; y < h; y++)
	    {
	      for (x = dx; x < w; x+=4)
		WRITE4_RGBA_RGB111_DITHER(src, dest);
	      for (; x < (width + dx); x++)
		{
		  WRITE1_RGBA_RGB111_DITHER(src, dest);
		}
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
    }
  else 
    {
      for (y = dy; y < h; y++)
	{
	  w = width + dx;
	  for (x = dx; ((x < w) && (!(IS_ALIGNED_32((int)dest)))); x++)
	    {
	      WRITE1_RGBA_RGB111_DITHER(src, dest);
	    }
	  if (x < w)
	    {
	      w = (width + dx) - (3 + x);
	      for (; x < w; x+=4)
		WRITE4_RGBA_RGB111_DITHER(src, dest);
	      for (; x < (width + dx); x++)
		{
		  WRITE1_RGBA_RGB111_DITHER(src, dest);
		}
	      src += src_jump;
	      dest += dest_jump;
	    }
	}
    }
  return;
  dx = 0;
  dy = 0;
}
  
static void
__imlib_RGBA_to_RGB1_fast(DATA32 *src , int src_jump, 
		    DATA8 *dest, int dow,
		    int width, int height, int dx, int dy)
{
  int x, y, w, h;
  int dest_jump = dow - width;
  
  w = width;
  h = height;

  for (y = 0; y < h; y++)
    {
      for (x = 0; x < w; x++)
	{
	  WRITE1_RGBA_RGB1(src, dest);
	}
      src += src_jump;
      dest += dest_jump;
    }
  return;
  dx = 0;
  dy = 0;
}

static void
__imlib_RGBA_to_RGB1_dither(DATA32 *src , int src_jump, 
		      DATA8 *dest, int dow,
		      int width, int height, int dx, int dy)
{
  int x, y, w, h;
  int dest_jump = dow - width;

  w = width + dx;
  h = height + dy;

  for (y = dy; y < h; y++)
    {
      for (x = dx; x < w; x++)
	{
	  WRITE1_RGBA_RGB1_DITHER(src, dest);
	}
      src += src_jump;
      dest += dest_jump;
    }
  return;
  dx = 0;
  dy = 0;
}

static void
__imlib_RGBA_to_A1_fast(DATA32 *src , int src_jump, 
		DATA8 *dest, int dow,
		int width, int height, int dx, int dy)
{
   int x, y, w, h;
   int dest_jump = dow - (width >> 3);
   
   w = width;
   h = height;

   for (y = 0; y < h; y++)
     {
	for (x = 0; x < w; x++)
	  {
	     WRITE1_RGBA_A1(src, dest);
	  }
	src += src_jump;
	dest += dest_jump;
     }
   return;
   dx = 0;
   dy = 0;
}

static void
__imlib_RGBA_to_A1_dither(DATA32 *src , int src_jump, 
		  DATA8 *dest, int dow,
		  int width, int height, int dx, int dy)
{
   int x, y, w, h;
   int dest_jump = dow - (width >> 3);
   
   w = width + dx;
   h = height + dy;

   for (y = dy; y < h; y++)
     {
	for (x = dx; x < w; x++)
	  {
	     WRITE1_RGBA_A1_DITHER(src, dest);
	  }
	src += src_jump;
	dest += dest_jump;
     }
   return;
   dx = 0;
   dy = 0;
}
  
static void
__imlib_RGBA_to_RGB8888_fast(DATA32 *src , int src_jump, 
		    DATA8 *dst, int dow,
		    int width, int height, int dx, int dy)
{
   int y, w, h;
   DATA32 *dest = (DATA32 *)dst;
   int dest_jump = (dow / sizeof(DATA32)) - width;
   
   w = width;
   h = height;

   if ((src_jump > 0) || (dest_jump > 0))
     {
	for (y = h; y > 0; y--)
	  {
	     memcpy(dest, src, w * sizeof(DATA32));
	     src  += src_jump  + w;
	     dest += dest_jump + w;
	  }
     }
   else
      memcpy(dest, src, h * w * sizeof(DATA32));
   return;
   dx = 0;
   dy = 0;
}

static void
__imlib_RGBA_to_BGR8888_fast(DATA32 *src , int src_jump, 
		    DATA8 *dst, int dow,
		    int width, int height, int dx, int dy)
{
   int x, y, w, h;
   DATA32 *dest = (DATA32 *)dst;
   int dest_jump = (dow / sizeof(DATA32)) - width;
   
   w = width;
   h = height;

   for (y = 0; y < h; y++)
     {
	for (x = 0; x < w; x++)
	  {
	     WRITE1_RGBA_BGR8888(src, dest);
	  }
	src += src_jump;
	dest += dest_jump;
     }
   return;
   dx = 0;
   dy = 0;
}

static void
__imlib_RGBA_to_RGB888_fast(DATA32 *src , int src_jump, 
		    DATA8  *dest, int dow,
		    int width, int height, int dx, int dy)
{
  int x, y, w, h;
  int dest_jump = dow - (width * 3);
  
  w = width;
  h = height;
  
  for (y = 0; y < h; y++)
    {
      for (x = 0; x < w; x++)
	{
	  WRITE1_RGBA_RGB888(src, dest);
	}
      src += src_jump;
      dest += dest_jump;
    }
  return;
  dx = 0;
  dy = 0;
}

static void
__imlib_RGBA_to_BGR888_fast(DATA32 *src , int src_jump, 
		    DATA8  *dest, int dow,
		    int width, int height, int dx, int dy)
{
  int x, y, w, h;
  int dest_jump = dow - (width * 3);
  
  w = width;
  h = height;
  
  for (y = 0; y < h; y++)
    {
      for (x = 0; x < w; x++)
	{
	  WRITE1_RGBA_BGR888(src, dest);
	}
      src += src_jump;
      dest += dest_jump;
    }
  return;
  dx = 0;
  dy = 0;
}

static void
__imlib_RGBA_to_Nothing(DATA32 *src , int src_jump, 
			DATA8  *dest, int dow,
			int width, int height, int dx, int dy)
{
   /*\ Nothing: Dummy function \*/
}

ImlibRGBAFunction
__imlib_GetRGBAFunction(int depth, 
			unsigned long rm, unsigned long gm, unsigned long bm,
			char hiq, DATA8 palette_type)
{
   if (depth == 16)
     {
	if (hiq)
	  {
	     if ((rm == 0xf800) && (gm == 0x7e0) && (bm == 0x1f))
		return __imlib_RGBA_to_RGB565_dither;
	     if ((rm == 0x7c00) && (gm == 0x3e0) && (bm == 0x1f))
		return __imlib_RGBA_to_RGB555_dither;
	     if ((bm == 0xf800) && (gm == 0x7e0) && (rm == 0x1f))
		return __imlib_RGBA_to_BGR565_dither;
	     if ((bm == 0x7c00) && (gm == 0x3e0) && (rm == 0x1f))
		return __imlib_RGBA_to_BGR555_dither;
	  }
	else
	  {
#ifdef DO_MMX_ASM
	     if (__imlib_get_cpuid() && CPUID_MMX)
	       {
		  if ((rm == 0xf800) && (gm == 0x7e0) && (bm == 0x1f))
		     return __imlib_mmx_rgb565_fast;
		  if ((rm == 0x7c00) && (gm == 0x3e0) && (bm == 0x1f))
		     return __imlib_mmx_rgb555_fast;
		  if ((bm == 0xf800) && (gm == 0x7e0) && (rm == 0x1f))
		     return __imlib_mmx_bgr565_fast;
		  if ((bm == 0x7c00) && (gm == 0x3e0) && (rm == 0x1f))
		     return __imlib_mmx_bgr555_fast;
	       }
	     else
#endif
	       {
		  if ((rm == 0xf800) && (gm == 0x7e0) && (bm == 0x1f))
		     return __imlib_RGBA_to_RGB565_fast;
		  if ((rm == 0x7c00) && (gm == 0x3e0) && (bm == 0x1f))
		     return __imlib_RGBA_to_RGB555_fast;
		  if ((bm == 0xf800) && (gm == 0x7e0) && (rm == 0x1f))
		     return __imlib_RGBA_to_BGR565_fast;
		  if ((bm == 0x7c00) && (gm == 0x3e0) && (rm == 0x1f))
		     return __imlib_RGBA_to_BGR555_fast;
	       }
	  }
	return NULL;
     }
   else if (depth == 32)
     {
	if ((rm == 0xff0000) && (gm == 0xff00) && (bm == 0xff))
	   return __imlib_RGBA_to_RGB8888_fast;
	return NULL;
     }
   else if (depth == 24)
     {
	if ((rm == 0xff0000) && (gm == 0xff00) && (bm == 0xff))
	   return __imlib_RGBA_to_RGB888_fast;
	return NULL;
     }
   else if (depth == 8)
     {
	if (hiq)
	  {
	     if (palette_type == 0)
		return __imlib_RGBA_to_RGB332_dither;
	     if (palette_type == 1)
		return __imlib_RGBA_to_RGB232_dither;
	     if (palette_type == 2)
		return __imlib_RGBA_to_RGB222_dither;
	     if (palette_type == 3)
		return __imlib_RGBA_to_RGB221_dither;
	     if (palette_type == 4)
		return __imlib_RGBA_to_RGB121_dither;
	     if (palette_type == 5)
		return __imlib_RGBA_to_RGB111_dither;
	     if (palette_type == 6)
		return __imlib_RGBA_to_RGB1_dither;
	     if (palette_type == 7)
		return __imlib_RGBA_to_RGB666_dither;
	  }
	else
	  {
	     if (palette_type == 0)
		return __imlib_RGBA_to_RGB332_fast;
	     if (palette_type == 1)
		return __imlib_RGBA_to_RGB232_fast;
	     if (palette_type == 2)
		return __imlib_RGBA_to_RGB222_fast;
	     if (palette_type == 3)
		return __imlib_RGBA_to_RGB221_fast;
	     if (palette_type == 4)
		return __imlib_RGBA_to_RGB121_fast;
	     if (palette_type == 5)
		return __imlib_RGBA_to_RGB111_fast;
	     if (palette_type == 6)
		return __imlib_RGBA_to_RGB1_fast;
	     if (palette_type == 7)
		return __imlib_RGBA_to_RGB666_fast;
	  }
     }
   return NULL;
}

ImlibRGBAFunction
__imlib_GetMaskFunction(char hiq)
{
   return hiq ? &__imlib_RGBA_to_A1_dither : &__imlib_RGBA_to_A1_fast;
}


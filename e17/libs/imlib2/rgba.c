#include "common.h"
#include "rgba.h"

#define IS_ALIGNED_64(val) (!((val) & 0x7))
#define IS_ALIGNED_32(val) (!((val) & 0x3))
#define IS_ALIGNED_16(val) (!((val) & 0x1))

#define IS_MULTIPLE_2(val) (!((val) & 0x1))
#define IS_MULTIPLE_4(val) (!((val) & 0x3))

/* for PPC / Motorola / SPARC, not x86, ALPHA */
/*#define __BIG_ENDIAN__*/
/* for data in ABGR memory model */

/* NOTES: */
/* x86: RGBA in byte order = ABGR when read as an int (in register/int) */

/* internal static functions */
static void *_load_PNG (int *ww, int *hh, FILE *f);

/* lookup table to see what color index to use */
extern DATA8 _dither_color_lut[256];

/* using DATA32 - major speedup for aligned memory reads */

/* 48Kb of static data (lookup tabel for dithering) */
static DATA16 _dither_r565_44[4][4][256];
static DATA16 _dither_g565_44[4][4][256];
static DATA16 _dither_b565_44[4][4][256];
/* another 48Kb of static data (lookup table for dithering) */
static DATA16 _dither_r555_44[4][4][256];
static DATA16 _dither_g555_44[4][4][256];
static DATA16 _dither_b555_44[4][4][256];
/* another 24Kb of static data (lookup table for dithering) */
static DATA8 _dither_r332_88[8][8][256];
static DATA8 _dither_g332_88[8][8][256];
static DATA8 _dither_b332_88[8][8][256];
/* another 24Kb of static data (lookup table for dithering) */
static DATA8 _dither_r232_88[8][8][256];
static DATA8 _dither_g232_88[8][8][256];
static DATA8 _dither_b232_88[8][8][256];
/* another 24Kb of static data (lookup table for dithering) */
static DATA8 _dither_r222_88[8][8][256];
static DATA8 _dither_g222_88[8][8][256];
static DATA8 _dither_b222_88[8][8][256];
/* another 24Kb of static data (lookup table for dithering) */
static DATA8 _dither_r221_88[8][8][256];
static DATA8 _dither_g221_88[8][8][256];
static DATA8 _dither_b221_88[8][8][256];
/* another 24Kb of static data (lookup table for dithering) */
static DATA8 _dither_r121_88[8][8][256];
static DATA8 _dither_g121_88[8][8][256];
static DATA8 _dither_b121_88[8][8][256];
/* another 24Kb of static data (lookup table for dithering) */
static DATA8 _dither_r111_88[8][8][256];
static DATA8 _dither_g111_88[8][8][256];
static DATA8 _dither_b111_88[8][8][256];
/* another 8Kb of static data (lookup table for dithering) */
static DATA8 _dither_1_88[8][8][256];

/*****************************************************************************/
/* Actual rendering routines                                                 */
/* RGBA -> RGB565                                                            */
/*****************************************************************************/

/*****************************************************************************/
/* MACROS for plain RGBA -> RGB565 conversion */
#define WRITE1_RGBA_RGB565(src, dest)        \
*dest = ((*src << 8) & 0xf800) |           \
        ((*src >> 5) & 0x7e0) |            \
        ((*src >> 19) & 0x1f); dest++; src++
#ifdef __BIG_ENDIAN__
#define WRITE2_RGBA_RGB565(src, dest)                   \
{                                                       \
 *((DATA32 *)dest) = ((src[1] << 8) & 0xf800) |         \
                     ((src[1] >> 5) & 0x7e0) |          \
                     ((src[1] >> 19) & 0x1f) |          \
                     ((src[0] << 24) & 0xf8000000) |    \
                     ((src[0] << 11) & 0x7e00000) |     \
                     ((src[0] >> 3) & 0x1f0000);        \
                     dest += 2; src += 2;               \
}
#else
#define WRITE2_RGBA_RGB565(src, dest)                    \
{                                                        \
 *((DATA32 *)dest) = ((src[0] << 8) & 0xf800) |          \
                     ((src[0] >> 5) & 0x7e0) |           \
                     ((src[0] >> 19) & 0x1f) |           \
                     ((src[1] << 24) & 0xf8000000) |     \
                     ((src[1] << 11) & 0x7e00000) |      \
                     ((src[1] >> 3) & 0x1f0000);         \
                     dest += 2; src += 2;                \
}
#endif
/*****************************************************************************/
/* MACROS for dithered RGBA -> RGB565 conversion */
#define DITHER_RGBA_565_LUT_R(num) \
(_dither_r565_44[(x + num) & 0x3][y & 0x3][(src[num] >> 0 ) & 0xff])
#define DITHER_RGBA_565_LUT_G(num) \
(_dither_g565_44[(x + num) & 0x3][y & 0x3][(src[num] >> 8)  & 0xff])
#define DITHER_RGBA_565_LUT_B(num) \
(_dither_b565_44[(x + num) & 0x3][y & 0x3][(src[num] >> 16) & 0xff])

#define WRITE1_RGBA_RGB565_DITHER(src, dest)                  \
*dest = (DITHER_RGBA_565_LUT_R(0)) |                          \
        (DITHER_RGBA_565_LUT_G(0)) |                          \
        (DITHER_RGBA_565_LUT_B(0)); dest++; src++
#ifdef __BIG_ENDIAN__
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
/* Actual rendering routines                                                 */
/* RGBA -> RGB555                                                            */
/*****************************************************************************/

/*****************************************************************************/
/* MACROS for plain RGBA -> RGB555 conversion */
#define WRITE1_RGBA_RGB555(src, dest)        \
*dest = ((*src << 7) & 0x7c00) |           \
        ((*src >> 6) & 0x3e0) |            \
        ((*src >> 19) & 0x1f); dest++; src++
#ifdef __BIG_ENDIAN__
#define WRITE2_RGBA_RGB555(src, dest)                   \
{                                                       \
 *((DATA32 *)dest) = ((src[1] << 7) & 0x7c00) |         \
                     ((src[1] >> 6) & 0x3e0) |          \
                     ((src[1] >> 19) & 0x1f) |          \
                     ((src[0] << 23) & 0x7c000000) |    \
                     ((src[0] << 10) & 0x3e00000) |     \
                     ((src[0] >> 3) & 0x1f0000);        \
                     dest += 2; src += 2;               \
}
#else
#define WRITE2_RGBA_RGB555(src, dest)                    \
{                                                        \
 *((DATA32 *)dest) = ((src[0] << 7) & 0x7c00) |          \
                     ((src[0] >> 6) & 0x3e0) |           \
                     ((src[0] >> 19) & 0x1f) |           \
                     ((src[1] << 23) & 0x7c000000) |     \
                     ((src[1] << 10) & 0x3e00000) |      \
                     ((src[1] >> 3) & 0x1f0000);         \
                     dest += 2; src += 2;                \
}
#endif
/*****************************************************************************/
/* MACROS for dithered RGBA -> RGB555 conversion */
#define DITHER_RGBA_555_LUT_R(num) \
(_dither_r555_44[(x + num) & 0x3][y & 0x3][(src[num] >> 0 ) & 0xff])
#define DITHER_RGBA_555_LUT_G(num) \
(_dither_g555_44[(x + num) & 0x3][y & 0x3][(src[num] >> 8)  & 0xff])
#define DITHER_RGBA_555_LUT_B(num) \
(_dither_b555_44[(x + num) & 0x3][y & 0x3][(src[num] >> 16) & 0xff])

#define WRITE1_RGBA_RGB555_DITHER(src, dest)                  \
*dest = (DITHER_RGBA_555_LUT_R(0)) |                          \
        (DITHER_RGBA_555_LUT_G(0)) |                          \
        (DITHER_RGBA_555_LUT_B(0)); dest++; src++
#ifdef __BIG_ENDIAN__
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
/* Actual rendering routines                                                 */
/* RGBA -> RGB332                                                            */
/*****************************************************************************/

/*****************************************************************************/
/* MACROS for plain RGBA -> RGB332 conversion */
#define WRITE1_RGBA_RGB332(src, dest)        \
*dest = _dither_color_lut[((*src >> 22) & 0x03) |                          \
                          ((*src >> 11) & 0x1c) |                          \
                          ((*src)       & 0xe0)]; dest++; src++
#ifdef __BIG_ENDIAN__
#define WRITE2_RGBA_RGB332(src, dest)                                      \
{                                                                          \
 *((DATA16 *)dest) = (_dither_color_lut[((src[1] >> 22) & 0x03) |          \
                                        ((src[1] >> 11) & 0x1c) |          \
                                        ((src[1])       & 0xe0)]) |        \
                     (_dither_color_lut[((src[0] >> 22) & 0x03) |          \
                                        ((src[0] >> 11) & 0x1c) |          \
                                        ((src[0]        & 0xe0)] << 8);    \
                     dest += 2; src += 2;                                  \
}
#define WRITE4_RGBA_RGB332(src, dest)                                      \
{                                                                          \
 *((DATA32 *)dest) = (_dither_color_lut[((src[3] >> 22) & 0x03) |          \
                                        ((src[3] >> 11) & 0x1c) |          \
                                        ((src[3])       & 0xe0)]) |        \
                     (_dither_color_lut[((src[2] >> 22) & 0x03) |          \
                                        ((src[2] >> 11) & 0x1c) |          \
                                        ((src[2])       & 0xe0)] << 8) |   \
                     (_dither_color_lut[((src[1] >> 22) & 0x03) |          \
                                        ((src[1] >> 11) & 0x1c) |          \
                                        ((src[1])       & 0xe0)] << 16) |  \
                     (_dither_color_lut[((src[0] >> 22) & 0x03) |          \
                                        ((src[0] >> 11) & 0x1c) |          \
                                        ((src[0])       & 0xe0)] << 24);   \
                     dest += 4; src += 4;                                  \
}
#else
#define WRITE2_RGBA_RGB332(src, dest)                                      \
{                                                                          \
 *((DATA16 *)dest) = (_dither_color_lut[((src[0] >> 22) & 0x03) |          \
                                        ((src[0] >> 11) & 0x1c) |          \
                                        ((src[0])       & 0xe0)]) |        \
                     (_dither_color_lut[((src[1] >> 22) & 0x03) |          \
                                        ((src[1] >> 11) & 0x1c) |          \
                                        ((src[1])       & 0xe0)] << 8);    \
                     dest += 2; src += 2;                                  \
}
#define WRITE4_RGBA_RGB332(src, dest)                                      \
{                                                                          \
 *((DATA32 *)dest) = (_dither_color_lut[((src[0] >> 22) & 0x03) |          \
                                        ((src[0] >> 11) & 0x1c) |          \
                                        ((src[0])       & 0xe0)]) |        \
                     (_dither_color_lut[((src[1] >> 22) & 0x03) |          \
                                        ((src[1] >> 11) & 0x1c) |          \
                                        ((src[1])       & 0xe0)] << 8) |   \
                     (_dither_color_lut[((src[2] >> 22) & 0x03) |          \
                                        ((src[2] >> 11) & 0x1c) |          \
                                        ((src[2])       & 0xe0)] << 16) |  \
                     (_dither_color_lut[((src[3] >> 22) & 0x03) |          \
                                        ((src[3] >> 11) & 0x1c) |          \
                                        ((src[3])       & 0xe0)] << 24);   \
                     dest += 4; src += 4;                                  \
}
#endif
/*****************************************************************************/
/* MACROS for dithered RGBA -> RGB332 conversion */
#define DITHER_RGBA_332_LUT_R(num) \
(_dither_r332_88[(x + num) & 0x7][y & 0x7][(src[num] >> 0 ) & 0xff])
#define DITHER_RGBA_332_LUT_G(num) \
(_dither_g332_88[(x + num) & 0x7][y & 0x7][(src[num] >> 8)  & 0xff])
#define DITHER_RGBA_332_LUT_B(num) \
(_dither_b332_88[(x + num) & 0x7][y & 0x7][(src[num] >> 16) & 0xff])

#define WRITE1_RGBA_RGB332_DITHER(src, dest)                                   \
*dest = _dither_color_lut[(DITHER_RGBA_332_LUT_R(0)) |                         \
                          (DITHER_RGBA_332_LUT_G(0)) |                         \
                          (DITHER_RGBA_332_LUT_B(0))]; dest++; src++
#ifdef __BIG_ENDIAN__
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
/* RGBA -> RGB232                                                            */
/*****************************************************************************/

/*****************************************************************************/
/* MACROS for plain RGBA -> RGB232 conversion */
#define RGB232_BSHIFT >> 22
#define RGB232_GSHIFT >> 11
#define RGB232_RSHIFT >> 1
#define RGB232_BMASK & 0x03
#define RGB232_GMASK & 0x1c
#define RGB232_RMASK & 0x60

#define WRITE1_RGBA_RGB232(src, dest)        \
*dest = _dither_color_lut[((*src RGB232_BSHIFT) RGB232_BMASK) |                          \
                          ((*src RGB232_GSHIFT) RGB232_GMASK) |                          \
                          ((*src RGB232_RSHIFT)  RGB232_RMASK)]; dest++; src++
#ifdef __BIG_ENDIAN__
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
(_dither_r232_88[(x + num) & 0x7][y & 0x7][(src[num] >> 0 ) & 0xff])
#define DITHER_RGBA_232_LUT_G(num) \
(_dither_g232_88[(x + num) & 0x7][y & 0x7][(src[num] >> 8)  & 0xff])
#define DITHER_RGBA_232_LUT_B(num) \
(_dither_b232_88[(x + num) & 0x7][y & 0x7][(src[num] >> 16) & 0xff])

#define WRITE1_RGBA_RGB232_DITHER(src, dest)                                   \
*dest = _dither_color_lut[(DITHER_RGBA_232_LUT_R(0)) |                         \
                          (DITHER_RGBA_232_LUT_G(0)) |                         \
                          (DITHER_RGBA_232_LUT_B(0))]; dest++; src++
#ifdef __BIG_ENDIAN__
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
#define RGB222_BSHIFT >> 22
#define RGB222_GSHIFT >> 12
#define RGB222_RSHIFT >> 2
#define RGB222_BMASK & 0x03
#define RGB222_GMASK & 0x0c
#define RGB222_RMASK & 0x30

#define WRITE1_RGBA_RGB222(src, dest)        \
*dest = _dither_color_lut[((*src RGB222_BSHIFT) RGB222_BMASK) |                          \
                          ((*src RGB222_GSHIFT) RGB222_GMASK) |                          \
                          ((*src RGB222_RSHIFT)  RGB222_RMASK)]; dest++; src++
#ifdef __BIG_ENDIAN__
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
(_dither_r222_88[(x + num) & 0x7][y & 0x7][(src[num] >> 0 ) & 0xff])
#define DITHER_RGBA_222_LUT_G(num) \
(_dither_g222_88[(x + num) & 0x7][y & 0x7][(src[num] >> 8)  & 0xff])
#define DITHER_RGBA_222_LUT_B(num) \
(_dither_b222_88[(x + num) & 0x7][y & 0x7][(src[num] >> 16) & 0xff])

#define WRITE1_RGBA_RGB222_DITHER(src, dest)                                   \
*dest = _dither_color_lut[(DITHER_RGBA_222_LUT_R(0)) |                         \
                          (DITHER_RGBA_222_LUT_G(0)) |                         \
                          (DITHER_RGBA_222_LUT_B(0))]; dest++; src++
#ifdef __BIG_ENDIAN__
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
#define RGB221_BSHIFT >> 23
#define RGB221_GSHIFT >> 13
#define RGB221_RSHIFT >> 3
#define RGB221_BMASK & 0x01
#define RGB221_GMASK & 0x06
#define RGB221_RMASK & 0x18

#define WRITE1_RGBA_RGB221(src, dest)        \
*dest = _dither_color_lut[((*src RGB221_BSHIFT) RGB221_BMASK) |                          \
                          ((*src RGB221_GSHIFT) RGB221_GMASK) |                          \
                          ((*src RGB221_RSHIFT)  RGB221_RMASK)]; dest++; src++
#ifdef __BIG_ENDIAN__
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
(_dither_r221_88[(x + num) & 0x7][y & 0x7][(src[num] >> 0 ) & 0xff])
#define DITHER_RGBA_221_LUT_G(num) \
(_dither_g221_88[(x + num) & 0x7][y & 0x7][(src[num] >> 8)  & 0xff])
#define DITHER_RGBA_221_LUT_B(num) \
(_dither_b221_88[(x + num) & 0x7][y & 0x7][(src[num] >> 16) & 0xff])

#define WRITE1_RGBA_RGB221_DITHER(src, dest)                                   \
*dest = _dither_color_lut[(DITHER_RGBA_221_LUT_R(0)) |                         \
                          (DITHER_RGBA_221_LUT_G(0)) |                         \
                          (DITHER_RGBA_221_LUT_B(0))]; dest++; src++
#ifdef __BIG_ENDIAN__
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
#define RGB121_BSHIFT >> 23
#define RGB121_GSHIFT >> 13
#define RGB121_RSHIFT >> 4
#define RGB121_BMASK & 0x01
#define RGB121_GMASK & 0x06
#define RGB121_RMASK & 0x08

#define WRITE1_RGBA_RGB121(src, dest)        \
*dest = _dither_color_lut[((*src RGB121_BSHIFT) RGB121_BMASK) |                          \
                          ((*src RGB121_GSHIFT) RGB121_GMASK) |                          \
                          ((*src RGB121_RSHIFT)  RGB121_RMASK)]; dest++; src++
#ifdef __BIG_ENDIAN__
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
(_dither_r121_88[(x + num) & 0x7][y & 0x7][(src[num] >> 0 ) & 0xff])
#define DITHER_RGBA_121_LUT_G(num) \
(_dither_g121_88[(x + num) & 0x7][y & 0x7][(src[num] >> 8)  & 0xff])
#define DITHER_RGBA_121_LUT_B(num) \
(_dither_b121_88[(x + num) & 0x7][y & 0x7][(src[num] >> 16) & 0xff])

#define WRITE1_RGBA_RGB121_DITHER(src, dest)                                   \
*dest = _dither_color_lut[(DITHER_RGBA_121_LUT_R(0)) |                         \
                          (DITHER_RGBA_121_LUT_G(0)) |                         \
                          (DITHER_RGBA_121_LUT_B(0))]; dest++; src++
#ifdef __BIG_ENDIAN__
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
#define RGB111_BSHIFT >> 23
#define RGB111_GSHIFT >> 14
#define RGB111_RSHIFT >> 5
#define RGB111_BMASK & 0x01
#define RGB111_GMASK & 0x02
#define RGB111_RMASK & 0x30

#define WRITE1_RGBA_RGB111(src, dest)        \
*dest = _dither_color_lut[((*src RGB111_BSHIFT) RGB111_BMASK) |                          \
                          ((*src RGB111_GSHIFT) RGB111_GMASK) |                          \
                          ((*src RGB111_RSHIFT)  RGB111_RMASK)]; dest++; src++
#ifdef __BIG_ENDIAN__
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
(_dither_r111_88[(x + num) & 0x7][y & 0x7][(src[num] >> 0 ) & 0xff])
#define DITHER_RGBA_111_LUT_G(num) \
(_dither_g111_88[(x + num) & 0x7][y & 0x7][(src[num] >> 8)  & 0xff])
#define DITHER_RGBA_111_LUT_B(num) \
(_dither_b111_88[(x + num) & 0x7][y & 0x7][(src[num] >> 16) & 0xff])

#define WRITE1_RGBA_RGB111_DITHER(src, dest)                                   \
*dest = _dither_color_lut[(DITHER_RGBA_111_LUT_R(0)) |                         \
                          (DITHER_RGBA_111_LUT_G(0)) |                         \
                          (DITHER_RGBA_111_LUT_B(0))]; dest++; src++
#ifdef __BIG_ENDIAN__
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
(_dither_1_88[(x + num) & 0x7][y & 0x7][((((*src >> 0) & 0xff) +      \
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

#define WRITE1_RGBA_A1(src, dest)                   \
*dest |= ((*src & 0x80000000) >> (31 - (x & 0x7))); \
if ((x & 0x7) == 0x7) dest++;                       \
src++

/*****************************************************************************/
/* MACROS for dithered RGBA -> A1 conversion */
#define DITHER_RGBA_A1_LUT(num) \
(_dither_1_88[(x + num) & 0x7][y & 0x7][(src[num] >> 24)])

#define WRITE1_RGBA_A1_DITHER(src, dest)            \
*dest |= (DITHER_RGBA_A1_LUT(0)) << (x & 0x7);      \
if ((x & 0x7) == 0x7) dest++;                       \
src++;

/*****************************************************************************/
/* Actual rendering routines                                                 */
/* RGBA -> RGB8888                                                           */
/*****************************************************************************/

/*****************************************************************************/
/* MACROS for plain RGBA -> RGB8888 conversion */
#define WRITE1_RGBA_RGB8888(src, dest)             \
*dest = ((*src >> 16)  & 0x0000ff) |              \
        ((*src << 0)   & 0x00ff00) |              \
        ((*src << 16)  & 0xff0000); dest++; src++;

/*****************************************************************************/
/* Actual rendering routines                                                 */
/* RGBA -> RGB888                                                            */
/*****************************************************************************/

/*****************************************************************************/
/* MACROS for plain RGBA -> RGB888 conversion */
#define WRITE1_RGBA_RGB888(src, dest)             \
*dest = ((*src >> 16)  & 0xff); dest++;       \
*dest = ((*src >> 8)   & 0xff); dest++;       \
*dest = ((*src >> 0)   & 0xff); dest++; src++;

/* Palette mode stuff */

void 
__imlib_RGBA_init(void)
{
  /* the famous dither matrix */
  DATA8 _dither_44[4][4] =
    {
	{0, 4, 1, 5},
	{6, 2, 7, 3},
	{1, 5, 0, 4},
	{7, 3, 6, 2}
    };
  DATA8 _dither_88[8][8] =
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
  
  for (y = 0; y < 4; y++)
    {
      for (x = 0; x < 4; x++)
	{
	  for (i = 0; i < 256; i++)
	    {
	      if ((_dither_44[x][y] < (i & 0x7)) && (i < (256 - 8)))
		_dither_r565_44[x][y][i] = ((i + 8) & 0xf8) << 8;
	      else
		_dither_r565_44[x][y][i] = (i & 0xf8) << 8;
	      
	      if ((_dither_44[x][y] < ((i & 0x3) << 1)) && (i < (256 - 4)))
		_dither_g565_44[x][y][i] = (((i + 4) & 0xfc) << 8) >> 5;
	      else
		_dither_g565_44[x][y][i] = ((i & 0xfc) << 8) >> 5;
	      
	      if ((_dither_44[x][y] < (i & 0x7)) && (i < (256 - 8)))
		_dither_b565_44[x][y][i] = (((i + 8) & 0xf8) << 16) >> 19;
	      else
		_dither_b565_44[x][y][i] = ((i & 0xf8) << 16) >> 19;
	      
	      if ((_dither_44[x][y] < (i & 0x7)) && (i < (256 - 8)))
		_dither_r555_44[x][y][i] = (((i + 8) & 0xf8) << 8) >> 1;
	      else
		_dither_r555_44[x][y][i] = ((i & 0xf8) << 8) >> 1;
	      
	      if ((_dither_44[x][y] < (i & 0x7)) && (i < (256 - 8)))
		_dither_g555_44[x][y][i] = (((i + 8) & 0xf8) << 8) >> 6;
	      else
		_dither_g555_44[x][y][i] = ((i & 0xf8) << 8) >> 6;
	      
	      if ((_dither_44[x][y] < (i & 0x7)) && (i < (256 - 8)))
		_dither_b555_44[x][y][i] = (((i + 8) & 0xf8) << 16) >> 19;
	      else
		_dither_b555_44[x][y][i] = ((i & 0xf8) << 16) >> 19;
	    }
	}
    }
  for (y = 0; y < 8; y++)
    {
      for (x = 0; x < 8; x++)
	{
	  for (i = 0; i < 256; i++)
	    {
	      if ((_dither_88[x][y] < ((i & 0x1f) << 1)) && (i < (256 - 32)))
		_dither_r332_88[x][y][i] = ((i + 32) & 0xe0);
	      else
		_dither_r332_88[x][y][i] = (i & 0xe0);
	      
	      if ((_dither_88[x][y] < ((i & 0x1f) << 1)) && (i < (256 - 32)))
		_dither_g332_88[x][y][i] = (((i + 32) >> 3)& 0x1c);
	      else
		_dither_g332_88[x][y][i] = ((i >> 3) & 0x1c);
	      
	      if ((_dither_88[x][y] < (i & 0x3f))        && (i < (256 - 64)))
		_dither_b332_88[x][y][i] = (((i + 64) >> 6)& 0x03);
	      else
		_dither_b332_88[x][y][i] = ((i >> 6) & 0x03);
	    }
	}
    } 
  for (y = 0; y < 8; y++)
    {
      for (x = 0; x < 8; x++)
	{
	  for (i = 0; i < 256; i++)
	    {
	      if ((_dither_88[x][y] < (i & 0x3f))        && (i < (256 - 64)))
		_dither_r232_88[x][y][i] = (((i + 64) >> 1) & 0x60);
	      else
		_dither_r232_88[x][y][i] = ((i >> 1) & 0x60);
	      
	      if ((_dither_88[x][y] < ((i & 0x1f) << 1)) && (i < (256 - 32)))
		_dither_g232_88[x][y][i] = (((i + 32) >> 3)& 0x1c);
	      else
		_dither_g232_88[x][y][i] = ((i >> 3) & 0x1c);
	      
	      if ((_dither_88[x][y] < (i & 0x3f))        && (i < (256 - 64)))
		_dither_b232_88[x][y][i] = (((i + 64) >> 6)& 0x03);
	      else
		_dither_b232_88[x][y][i] = ((i >> 6) & 0x03);
	    }
	}
    }
  for (y = 0; y < 8; y++)
    {
      for (x = 0; x < 8; x++)
	{
	  for (i = 0; i < 256; i++)
	    {
	      if ((_dither_88[x][y] < (i & 0x3f))         && (i < (256 - 64)))
		_dither_r222_88[x][y][i] = (((i + 64) >> 2) & 0x30);
	      else
		_dither_r222_88[x][y][i] = ((i >> 2) & 0x30);
	      
	      if ((_dither_88[x][y] < (i & 0x3f))        && (i < (256 - 64)))
		_dither_g222_88[x][y][i] = (((i + 64) >> 4)& 0x0c);
	      else
		_dither_g222_88[x][y][i] = ((i >> 4) & 0x0c);
	      
	      if ((_dither_88[x][y] < (i & 0x3f))        && (i < (256 - 64)))
		_dither_b222_88[x][y][i] = (((i + 64) >> 6)& 0x03);
	      else
		_dither_b222_88[x][y][i] = ((i >> 6) & 0x03);
	    }
	}
    }
  for (y = 0; y < 8; y++)
    {
      for (x = 0; x < 8; x++)
	{
	  for (i = 0; i < 256; i++)
	    {
	      if ((_dither_88[x][y] < (i & 0x3f)) && (i < (256 - 64)))
		_dither_r221_88[x][y][i] = (((i + 64) >> 3) & 0x18);
	      else
		_dither_r221_88[x][y][i] = ((i >> 3) & 0x18);
	      
	      if ((_dither_88[x][y] < (i & 0x3f)) && (i < (256 - 64)))
		_dither_g221_88[x][y][i] = (((i + 64) >> 5) & 0x06);
	      else
		_dither_g221_88[x][y][i] = ((i >> 5) & 0x06);
	      
	      if ((_dither_88[x][y] < ((i & 0x7f) >> 1)) && (i < (256 - 128)))
		_dither_b221_88[x][y][i] = (((i + 128) >> 7) & 0x01);
	      else
		_dither_b221_88[x][y][i] = ((i >> 7) & 0x01);
	    }
	}
    }
  for (y = 0; y < 8; y++)
    {
      for (x = 0; x < 8; x++)
	{
	  for (i = 0; i < 256; i++)
	    {
	      if ((_dither_88[x][y] < ((i & 0x7f) >> 1)) && (i < (256 - 128)))
		_dither_r121_88[x][y][i] = (((i + 128) >> 4) & 0x08);
	      else
		_dither_r121_88[x][y][i] = ((i >> 4) & 0x08);
	      
	      if ((_dither_88[x][y] < (i & 0x3f)) && (i < (256 - 64)))
		_dither_g121_88[x][y][i] = (((i + 64) >> 5) & 0x06);
	      else
		_dither_g121_88[x][y][i] = ((i >> 5) & 0x06);
	      
	      if ((_dither_88[x][y] < ((i & 0x7f) >> 1)) && (i < (256 - 128)))
		_dither_b121_88[x][y][i] = (((i + 128) >> 7) & 0x01);
	      else
		_dither_b121_88[x][y][i] = ((i >> 7) & 0x01);
	    }
	}
    }
  for (y = 0; y < 8; y++)
    {
      for (x = 0; x < 8; x++)
	{
	  for (i = 0; i < 256; i++)
	    {
	      if ((_dither_88[x][y] < ((i & 0x7f) >> 1)) && (i < (256 - 128)))
		_dither_r111_88[x][y][i] = (((i + 128) >> 5) & 0x04);
	      else
		_dither_r111_88[x][y][i] = ((i >> 5) & 0x04);
	      
	      if ((_dither_88[x][y] < ((i & 0x7f) >> 1)) && (i < (256 - 128)))
		_dither_g111_88[x][y][i] = (((i + 128) >> 6) & 0x02);
	      else
		_dither_g111_88[x][y][i] = ((i >> 6) & 0x02);
	      
	      if ((_dither_88[x][y] < ((i & 0x7f) >> 1)) && (i < (256 - 128)))
		_dither_b111_88[x][y][i] = (((i + 128) >> 7) & 0x01);
	      else
		_dither_b111_88[x][y][i] = ((i >> 7) & 0x01);
	    }
	}
    }
  for (y = 0; y < 8; y++)
    {
      for (x = 0; x < 8; x++)
	{
	  for (i = 0; i < 256; i++)
	    {
	      if ((_dither_88[x][y] < ((i & 0x7f) >> 1)) && (i < (256 - 128)))
		_dither_1_88[x][y][i] = (((i + 128) >> 7) & 0x01);
	      else
		_dither_1_88[x][y][i] = ((i >> 7) & 0x01);
	    }
	}
    }
}

void
__imlib_RGBA_to_RGB565_fast(DATA32 *src , int src_jump, 
		    DATA16 *dest, int dest_jump,
		    int width, int height, int dx, int dy)
{
  int x, y, w, h;
  
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

void
__imlib_RGBA_to_RGB565_dither(DATA32 *src , int src_jump, 
		      DATA16 *dest, int dest_jump,
		      int width, int height, int dx, int dy)
{
  int x, y, w, h;
  
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

void
__imlib_RGBA_to_RGB555_fast(DATA32 *src , int src_jump, 
		    DATA16 *dest, int dest_jump,
		    int width, int height, int dx, int dy)
{
  int x, y, w, h;
  
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

void
__imlib_RGBA_to_RGB555_dither(DATA32 *src , int src_jump, 
		      DATA16 *dest, int dest_jump,
		      int width, int height, int dx, int dy)
{
  int x, y, w, h;
  
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

void
__imlib_RGBA_to_RGB332_fast(DATA32 *src , int src_jump, 
		    DATA8 *dest, int dest_jump,
		    int width, int height, int dx, int dy)
{
  int x, y, w, h;
  
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

void
__imlib_RGBA_to_RGB332_dither(DATA32 *src , int src_jump, 
		      DATA8 *dest, int dest_jump,
		      int width, int height, int dx, int dy)
{
  int x, y, w, h;

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

void
__imlib_RGBA_to_RGB232_fast(DATA32 *src , int src_jump, 
		    DATA8 *dest, int dest_jump,
		    int width, int height, int dx, int dy)
{
  int x, y, w, h;
  
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

void
__imlib_RGBA_to_RGB232_dither(DATA32 *src , int src_jump, 
		      DATA8 *dest, int dest_jump,
		      int width, int height, int dx, int dy)
{
  int x, y, w, h;

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

void
__imlib_RGBA_to_RGB222_fast(DATA32 *src , int src_jump, 
		    DATA8 *dest, int dest_jump,
		    int width, int height, int dx, int dy)
{
  int x, y, w, h;
  
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

void
__imlib_RGBA_to_RGB222_dither(DATA32 *src , int src_jump, 
		      DATA8 *dest, int dest_jump,
		      int width, int height, int dx, int dy)
{
  int x, y, w, h;

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

void
__imlib_RGBA_to_RGB221_fast(DATA32 *src , int src_jump, 
		    DATA8 *dest, int dest_jump,
		    int width, int height, int dx, int dy)
{
  int x, y, w, h;
  
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

void
__imlib_RGBA_to_RGB221_dither(DATA32 *src , int src_jump, 
		      DATA8 *dest, int dest_jump,
		      int width, int height, int dx, int dy)
{
  int x, y, w, h;

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
  
void
__imlib_RGBA_to_RGB121_fast(DATA32 *src , int src_jump, 
		    DATA8 *dest, int dest_jump,
		    int width, int height, int dx, int dy)
{
  int x, y, w, h;
  
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

void
__imlib_RGBA_to_RGB121_dither(DATA32 *src , int src_jump, 
		      DATA8 *dest, int dest_jump,
		      int width, int height, int dx, int dy)
{
  int x, y, w, h;

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
  
void
__imlib_RGBA_to_RGB111_fast(DATA32 *src , int src_jump, 
		    DATA8 *dest, int dest_jump,
		    int width, int height, int dx, int dy)
{
  int x, y, w, h;
  
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

void
__imlib_RGBA_to_RGB111_dither(DATA32 *src , int src_jump, 
		      DATA8 *dest, int dest_jump,
		      int width, int height, int dx, int dy)
{
  int x, y, w, h;

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
  
void
__imlib_RGBA_to_RGB1_fast(DATA32 *src , int src_jump, 
		    DATA8 *dest, int dest_jump,
		    int width, int height, int dx, int dy)
{
  int x, y, w, h;
  
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

void
__imlib_RGBA_to_RGB1_dither(DATA32 *src , int src_jump, 
		      DATA8 *dest, int dest_jump,
		      int width, int height, int dx, int dy)
{
  int x, y, w, h;

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

void
__imlib_RGBA_to_A1_fast(DATA32 *src , int src_jump, 
		DATA8 *dest, int dest_jump,
		int width, int height, int dx, int dy)
{
  int x, y, w, h;
  
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

void
__imlib_RGBA_to_A1_dither(DATA32 *src , int src_jump, 
		  DATA8 *dest, int dest_jump,
		  int width, int height, int dx, int dy)
{
  int x, y, w, h;
  
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
  
void
__imlib_RGBA_to_RGB8888_fast(DATA32 *src , int src_jump, 
		    DATA32 *dest, int dest_jump,
		    int width, int height, int dx, int dy)
{
  int x, y, w, h;
  
  w = width;
  h = height;
  
  for (y = 0; y < h; y++)
    {
      for (x = 0; x < w; x++)
	{
	  WRITE1_RGBA_RGB8888(src, dest);
	}
      src += src_jump;
      dest += dest_jump;
    }
  return;
  dx = 0;
  dy = 0;
}

void
__imlib_RGBA_to_RGB888_fast(DATA32 *src , int src_jump, 
		    DATA8  *dest, int dest_jump,
		    int width, int height, int dx, int dy)
{
  int x, y, w, h;
  
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
  

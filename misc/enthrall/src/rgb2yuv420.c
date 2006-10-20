/* Avisynth v3.0 alpha.
 * Copyright 2005 David Pierre - Ben Rudiak-Gould et al.
 * http://www.avisynth.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA, or visit
 * http://www.gnu.org/copyleft/gpl.html .
 */

#include <stdint.h>

/*
 * Colour conversion from
 * http://www.poynton.com/notes/colour_and_gamma/ColorFAQ.html#RTFToC30
 *
 * YCbCr in Rec. 601 format
 * RGB values are in the range [0..255]
 *
 * [ Y  ]   [  16 ]    1    [  65.738    129.057    25.064  ]   [ R ]
 * [ Cb ] = [ 128 ] + --- * [ -37.945    -74.494    112.439 ] * [ G ]
 * [ Cr ]   [ 128 ]   256   [ 112.439    -94.154    -18.285 ]   [ B ]
 */
void
rgb2yuv420 (uint32_t *data_rgb, int width, int height,
            uint8_t *data_y, uint8_t *data_u, uint8_t *data_v,
            int y_stride, int uv_stride)
{
	int x, y, row_stride = width * 4;
	uint8_t *rgb, *Y = data_y, *U = data_u, *V = data_v;

	uint8_t u00, u01, u10, u11;
	uint8_t v00, v01, v10, v11;

	int32_t RtoYCoeff = (int32_t) (65.738  * 256 + 0.5);
	int32_t GtoYCoeff = (int32_t) (129.057 * 256 + 0.5);
	int32_t BtoYCoeff = (int32_t) (25.064  * 256 + 0.5);

	int32_t RtoUCoeff = (int32_t) (-37.945 * 256 + 0.5);
	int32_t GtoUCoeff = (int32_t) (-74.494 * 256 + 0.5);
	int32_t BtoUCoeff = (int32_t) (112.439 * 256 + 0.5);

	int32_t RtoVCoeff = (int32_t) (112.439 * 256 + 0.5);
	int32_t GtoVCoeff = (int32_t) (-94.154 * 256 + 0.5);
	int32_t BtoVCoeff = (int32_t) (-18.285 * 256 + 0.5);

	int y_edge = y_stride - width, uv_edge = uv_stride - (width / 2);

	/* Y plane */
	rgb = (uint8_t *) data_rgb;

	for (y = height; y-- > 0; ) {
		for (x = width; x-- > 0; ) {
			/* No need to saturate between 16 and 235 */
			*Y = 16 + ((32768 +
			            BtoYCoeff * *(rgb) +
			            GtoYCoeff * *(rgb + 1) +
			            RtoYCoeff * *(rgb + 2)) >> 16);
			Y++;
			rgb += 4;
		}

		Y += y_edge;
	}

	/* U and V planes */
	rgb = (uint8_t *) data_rgb;

	for (y = height / 2; y-- > 0; ) {
		for (x = width / 2; x-- > 0; ) {
			/* No need to saturate between 16 and 240 */
			u00 = 128 + ((32768 +
			            BtoUCoeff * *(rgb) +
			            GtoUCoeff * *(rgb + 1) +
			            RtoUCoeff * *(rgb + 2)) >> 16);
			u01 = 128 + ((32768 +
			            BtoUCoeff * *(rgb + 4) +
			            GtoUCoeff * *(rgb + 5) +
			            RtoUCoeff * *(rgb + 6)) >> 16);
			u10 = 128 + ((32768 +
			            BtoUCoeff * *(rgb + row_stride) +
			            GtoUCoeff * *(rgb + row_stride + 1) +
			            RtoUCoeff * *(rgb + row_stride + 2)) >> 16);
			u11 = 128 + ((32768 +
			            BtoUCoeff * *(rgb + row_stride + 4) +
			            GtoUCoeff * *(rgb + row_stride + 5) +
			            RtoUCoeff * *(rgb + row_stride + 6)) >> 16);
			*U++ = (2 + u00 + u01 + u10 + u11) >> 2;

			v00 = 128 + ((32768 +
			            BtoVCoeff * *(rgb) +
			            GtoVCoeff * *(rgb + 1) +
			            RtoVCoeff * *(rgb + 2)) >> 16);
			v01 = 128 + ((32768 +
			            BtoVCoeff * *(rgb + 4) +
			            GtoVCoeff * *(rgb + 5) +
			            RtoVCoeff * *(rgb + 6)) >> 16);
			v10 = 128 + ((32768 +
			            BtoVCoeff * *(rgb + row_stride) +
			            GtoVCoeff * *(rgb + row_stride + 1) +
			            RtoVCoeff * *(rgb + row_stride + 2)) >> 16);
			v11 = 128 + ((32768 +
			            BtoVCoeff * *(rgb + row_stride + 4) +
			            GtoVCoeff * *(rgb + row_stride + 5) +
			            RtoVCoeff * *(rgb + row_stride + 6)) >> 16);
			*V++ = (2 + v00 + v01 + v10 + v11) >> 2;

			rgb += 8;
		}

		rgb += row_stride;
		U += uv_edge;
		V += uv_edge;
	}
}

/*
 * Copyright (c) 2005 Vincent Torri (vtorri at univ-evry fr)
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
            uint8_t *data_y, uint8_t *data_u, uint8_t *data_v)
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
	}
}

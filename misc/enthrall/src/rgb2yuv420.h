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

#ifndef __RGB2YUV420_H
#define __RGB2YUV420_H

void rgb2yuv420 (uint32_t *data_rgb, int width, int height,
                 uint8_t *data_y, uint8_t *data_u, uint8_t *data_v,
                 int y_stride, int uv_stride);

#endif

/*
 * Copyright (C) 2007 Vincent Torri (vtorri at univ-evry dot fr)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */

void *enthrall_ffmpeg_new ();

bool enthrall_ffmpeg_init (void       *engine,
			   const char *filename,
			   int         bitrate,
			   int        *width,
			   int        *height,
			   int        *offset_x,
			   int        *offset_y);

void enthrall_ffmpeg_encode_frame (void     *engine,
				   uint32_t *data);

void enthrall_ffmpeg_finish (void *engine);

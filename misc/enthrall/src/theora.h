/*
 * Copyright (C) 2006 Tilman Sauerbeck (tilman at code-monkey de)
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
#include <stdio.h>
#include <stdint.h>
#include <theora/theora.h>

typedef struct {
	FILE *file;
	ogg_stream_state to;
	theora_state td;
	yuv_buffer yuv;
} EnthrallTheora;

bool enthrall_theora_init (EnthrallTheora *et, const char *filename,
                           int quality, int width, int height,
                           uint8_t **y, uint8_t **u, uint8_t **v);

void enthrall_theora_encode_frame (EnthrallTheora *et, bool final_frame);
void enthrall_theora_finish (EnthrallTheora *et);

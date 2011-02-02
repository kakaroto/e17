/*
 * Copyright 2011 Mike Blumenkrantz <mike@zentific.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef EDJE_PARSER_MATH_H
#define EDJE_PARSER_MATH_H

#include <Eina.h>
#include "edje.h"
Eina_Bool edje_parser_strtol(const char *text,
                             int        *i);
Eina_Bool edje_parser_strtod(const char *text,
                             double     *d);
Eina_Bool edje_parser_strtobool(const char *text,
                                int        *i);

#endif

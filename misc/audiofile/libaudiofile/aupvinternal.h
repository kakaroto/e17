/*
	Audio File Library
	Copyright (C) 1998, Michael Pruett <michael@68k.org>

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Library General Public
	License as published by the Free Software Foundation; either
	version 2 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Library General Public License for more details.

	You should have received a copy of the GNU Library General Public
	License along with this library; if not, write to the 
	Free Software Foundation, Inc., 59 Temple Place - Suite 330, 
	Boston, MA  02111-1307  USA.
*/

/*
	aupvinternal.h

	This file contains the private data structures for the parameter
	value list data types.
*/

#ifndef AUPVINTERNAL_H
#define AUPVINTERNAL_H

struct _AUpvitem
{
	int		type;
	int		parameter;

	union
	{
		long	l;
		double	d;
		void	*v;
	}
	value;
};

struct _AUpvlist
{
	size_t				count;
	struct _AUpvitem	*items;
};

#endif

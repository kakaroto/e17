/*
	Audio File Library

	Copyright 1998, Michael Pruett <michael@68k.org>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License as
	published by the Free Software Foundation; either version 2 of
	the License, or (at your option) any later version.

	This program is distributed in the hope that it will be
	useful, but WITHOUT ANY WARRANTY; without even the implied
	warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
	PURPOSE.  See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public
	License along with this program; if not, write to the Free
	Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
	MA 02111-1307, USA.
*/

#ifdef __USE_SGI_HEADERS__
#include <dmedia/audiofile.h>
#include <dmedia/audioutil.h>
#else
#include <audiofile.h>
#include <aupvlist.h>
#endif

#include <stdio.h>

main (int ac, char **av)
{
	AUpvlist	list, formatlist;
	long		*flist;
	long		*larray;
	char		*cvalue;
	long		lvalue;
	int			i, formatcount;
	int			param;

	formatlist = afQuery(AF_QUERYTYPE_FILEFMT, AF_QUERY_IDS, 0, 0, 0);
	formatcount = afQueryLong(AF_QUERYTYPE_FILEFMT, AF_QUERY_ID_COUNT, 0, 0, 0);

	printf("formatcount = %d\n", formatcount);
	AUpvgetval(formatlist, 0, &flist);
	AUpvfree(formatlist);

	for (i=0; i<formatcount; i++)
	{
		int		count;
		long	format;
		char	*formatstring;

		format = flist[i];
		printf("format = %d\n", format);
		formatstring = afQueryPointer(AF_QUERYTYPE_FILEFMT, AF_QUERY_NAME,
			format, 0, 0);
		printf("format = %s\n", formatstring);

		lvalue = afQueryLong(AF_QUERYTYPE_INST, AF_QUERY_SUPPORTED,
			format, 0, 0);
		printf("instrument query: supported: %d\n", lvalue);

		lvalue = afQueryLong(AF_QUERYTYPE_INST, AF_QUERY_ID_COUNT,
			format, 0, 0);
		printf("instrument query: id count: %d\n", lvalue);

		lvalue = afQueryLong(AF_QUERYTYPE_INSTPARAM, AF_QUERY_SUPPORTED,
			format, 0, 0);
		printf("instrument parameter query: supported: %d\n", lvalue);

		lvalue = afQueryLong(AF_QUERYTYPE_INSTPARAM, AF_QUERY_ID_COUNT,
			format, 0, 0);
		printf("instrument parameter query: id count: %d\n", lvalue);

		larray = afQueryPointer(AF_QUERYTYPE_INSTPARAM, AF_QUERY_IDS,
			format, 0, 0);

		if (larray != NULL)
		{
			int	i;
			for (i=0; i<lvalue; i++)
			{
				printf("instrument parameter query: id: %d\n", larray[i]);
				printf("	type of parameter: %d\n",
					afQueryLong(AF_QUERYTYPE_INSTPARAM, AF_QUERY_TYPE,
						format, larray[i], 0));
				printf("	name of parameter: %s\n",
					afQueryPointer(AF_QUERYTYPE_INSTPARAM, AF_QUERY_NAME,
						format, larray[i], 0));
			}
			free(larray);
		}
	}
	free(flist);
}

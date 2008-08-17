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
#include <dmedia/dm_audioutil.h>
#else
#include <audiofile.h>
#include <aupvlist.h>
#endif

#include <stdio.h>
#include <stdlib.h>

int main (int ac, char **av)
{
	AUpvlist	list, formatlist;
	long		*flist;
	long		*larray;
	char		*cvalue;
	long		lvalue;
	int			i, formatcount;

	formatlist = afQuery(AF_QUERYTYPE_FILEFMT, AF_QUERY_IDS, 0, 0, 0);
	formatcount = afQueryLong(AF_QUERYTYPE_FILEFMT, AF_QUERY_ID_COUNT, 0, 0, 0);

	printf("formatcount = %d\n", formatcount);
	AUpvgetval(formatlist, 0, &flist);
	AUpvgetparam(formatlist, 0, &i);
	printf("formatlist parameter 0: %d\n", i);

	for (i=0; i<formatcount; i++)
	{
		long	format;

		format = flist[i];
		printf("format = %ld\n", format);

		list = afQuery(AF_QUERYTYPE_FILEFMT, AF_QUERY_NAME, format, 0, 0);
		AUpvgetval(list, 0, &cvalue);
		printf("AUpvgetval: %s\n", cvalue);
		AUpvfree(list);

		list = afQuery(AF_QUERYTYPE_FILEFMT, AF_QUERY_LABEL, format, 0, 0);
		AUpvgetval(list, 0, &cvalue);
		printf("AUpvgetval: %s\n", cvalue);
		AUpvfree(list);

		list = afQuery(AF_QUERYTYPE_FILEFMT, AF_QUERY_DESC, format, 0, 0);
		AUpvgetval(list, 0, &cvalue);
		printf("AUpvgetval: %s\n", cvalue);
		AUpvfree(list);

		list = afQuery(AF_QUERYTYPE_FILEFMT, AF_QUERY_IMPLEMENTED, format, 0, 0);
		AUpvgetval(list, 0, &lvalue);
		printf("AUpvgetval: implemented: %ld\n", lvalue);
		AUpvfree(list);

		list = afQuery(AF_QUERYTYPE_FILEFMT, AF_QUERY_COMPRESSION_TYPES,
			AF_QUERY_VALUE_COUNT, format, 0);
		AUpvgetval(list, 0, &lvalue);
		printf("AUpvgetval: compression types: %ld\n", lvalue);
		AUpvfree(list);

		list = afQuery(AF_QUERYTYPE_FILEFMT, AF_QUERY_COMPRESSION_TYPES,
			AF_QUERY_VALUES, format, 0);
		AUpvgetval(list, 0, &larray);
		if (larray != NULL)
		{
			int	i;
			for (i=0; i<lvalue; i++)
				printf("larray[%d]: %ld\n", i, larray[i]);
		}
		free(larray);
		AUpvfree(list);

		list = afQuery(AF_QUERYTYPE_FILEFMT, AF_QUERY_SAMPLE_FORMATS,
			AF_QUERY_DEFAULT, format, 0);
		AUpvgetval(list, 0, &lvalue);
		printf("AUpvgetval: sample format: %ld\n", lvalue);
		AUpvfree(list);

		list = afQuery(AF_QUERYTYPE_FILEFMT, AF_QUERY_SAMPLE_SIZES,
			AF_QUERY_DEFAULT, format, 0);
		AUpvgetval(list, 0, &lvalue);
		printf("AUpvgetval: sample size: %ld\n", lvalue);
		AUpvfree(list);
	}

	list = afQuery(AF_QUERYTYPE_FILEFMT, AF_QUERY_ID_COUNT, 0, 0, 0);
	AUpvgetval(list, 0, &lvalue);
	printf("AUpvgetval: id count: %ld\n", lvalue);
	AUpvfree(list);
	free(flist);
	return 0;
}

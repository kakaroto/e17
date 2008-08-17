#include <dmedia/audiofile.h>
#include <dmedia/audioutil.h>
#include <stdio.h>

main (int argc, char **argv)
{
	AUpvlist	list;
	long		lvalue;
	char		*formatstring;

	printf("fuck!\n");
	lvalue = afQueryLong(AF_QUERYTYPE_LOOP, AF_QUERY_SUPPORTED, AF_FILE_AIFF, 0, 0);
	printf("fuck!\n");
	printf("ass: %d\n", lvalue);
	formatstring = afQueryPointer(AF_QUERYTYPE_FILEFMT, AF_QUERY_NAME, 1, 0, 0);
}

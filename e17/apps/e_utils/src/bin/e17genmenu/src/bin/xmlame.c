#include "xmlame.h"

/** xmlame.c Extensively Mocked Language Approximately Mangled for Enlightenment. 
 *
 * This is NOT a real XML parser.  There were a few ways we could go when it came
 * to parsing the freedesktop.org (fdo) XML menu files.  Whatever way we went, we 
 * needed some sort of XML parser if we wanted to fully support fdo menu files.  
 * Nothing we can do about that, fdo set the standard and they choose XML to do it.
 *
 * After a discussion with raster, three things led to the decision to do it this 
 * way.  It is likely that this will get included as a core part of the E17 window 
 * manager (E17) coz E17 needs this functionality.  E17 is in a dependency freeze
 * and there is no XML parser in it's current dependencies.  The fdo XML menu files
 * look to be simple enough to parse that this sort of fake, brain dead, XML parser
 * may get away with it.  Much testing on lots of systems is highly recommended.
 */

Dumb_List *
xmlame_new(char *buffer)
{
   Dumb_List *list;

   list = dumb_list_new(buffer);
   return list;
}

void
xmlame_fill(char *file)
{
}

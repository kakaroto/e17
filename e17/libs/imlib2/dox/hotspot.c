#include "dox.h"

DoxHotspot *root;

void  init_hotspot()
{
   root = malloc( sizeof( DoxHotspot ) );
   root->name = strdup( "root" );
   root->x = 0;
   root->y = 0;
   root->w = 0;
   root->h = 0;
   root->next = NULL;
   printf( "Hotspots initialised.\n" );
}

void  add_hotspot( char *target, int x, int y, int w, int h )
{
   DoxHotspot *ptr;
   for( ptr = root; ptr->next != NULL; ptr = ptr->next )
     ;
   ptr->next = malloc( sizeof( DoxHotspot ) );
   ptr = ptr->next;
   ptr->name = strdup( "root" );
   ptr->x = 0;
   ptr->y = 0;
   ptr->w = 0;
   ptr->h = 0;
   ptr->next = NULL;
   printf( "Hotspot added\n" );
}

char *check_hotspot( int x, int y )
{  
   DoxHotspot *ptr;
   for( ptr = root; ptr != NULL; ptr = ptr->next )
   {
      if( (x >= ptr->x) && (x <= (ptr->x + ptr->w)) &&
	  (y >= ptr->y) && (y <= (ptr->y + ptr->h)) )
      {
	 printf( "Found Hotspot\n" );
	 return ptr->name;
      }
   }
   return NULL;
}

void  tidy_hotspots()
{
   
}

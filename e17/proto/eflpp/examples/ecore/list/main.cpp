#include <eflpp_ecore.h>
#include <eflpp_ecore_data.h>

#include <iostream>
using namespace std;
using namespace efl;

int main( int argc, const char **argv )
{
    EcoreList <int> numberList;

    int n[6] = {10,11,12,13,14,15};

    numberList.append( &n[0] );
    numberList.append( &n[1] );
    numberList.append( &n[2] );
    numberList.append( &n[3] );
    numberList.append( &n[4] );
    numberList.append( &n[5] );

    int *i = numberList.gotoFirst ();

    for (; i != NULL; i = numberList.next ())
    {
      printf ("i = %d\n", *i);
    }

    return 0;
}

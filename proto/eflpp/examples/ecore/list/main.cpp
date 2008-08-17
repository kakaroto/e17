#include <eflpp_ecore.h>
#include <eflpp_ecore_data.h>

#include <iostream>
using namespace std;
using namespace efl;

int main( int argc, const char **argv )
{
    // int* test
    EcoreList <int> numberList;
  
    int n[6] = {10,11,12,13,14,15};
  
    numberList.append( &n[0] );
    numberList.append( &n[1] );
    numberList.append( &n[2] );
    numberList.append( &n[3] );
    numberList.append( &n[4] );
    numberList.append( &n[5] );

    int *i = numberList.gotoFirst ();
    i = numberList.next ();

    for (; i != NULL; i = numberList.next ())
    {
      printf ("i = %d\n", *i);
    }
  
    // char* test
    EcoreList <char> strList;
    
    char *str[3] = {"First", "Second", "Third"};
  
    strList.append( str[0] );
    strList.append( str[1] );
    strList.append( str[2] );
    
    char *s = strList.gotoFirst ();
    s = strList.next ();

    for (; s != NULL; s = strList.next ())
    {
      printf ("s = %s\n", s);
    }

    return 0;
}

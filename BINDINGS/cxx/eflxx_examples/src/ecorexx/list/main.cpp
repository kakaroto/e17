#include <ecorexx/Ecorexx.h>

#include <iostream>
using namespace std;
using namespace Eflxx;

int main( int argc, const char **argv )
{
  // int* test
  Ecorexx::List <int> numberList;

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
  Ecorexx::List <char> strList;

  char first[] = "First";
  char second[] = "Second";
  char third[] = "Third";

  strList.append( first );
  strList.append( second );
  strList.append( third );

  char *s = strList.gotoFirst ();
  s = strList.next ();

  for (; s != NULL; s = strList.next ())
  {
    printf ("s = %s\n", s);
  }

  return 0;
}

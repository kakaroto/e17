#include <einaxx/Einaxx.h>

#include <iostream>

using namespace std;
using namespace Einaxx;

int main (int argc, char **argv)
{
  Application::init ();
  
  List<int*> l;
  int i1 = 1;
  int i2 = 2;
  int i3 = 3;
  int i4 = 4;
  int i5 = 5;
  
  l.append (&i1);
  l.append (&i2);
  l.append (&i3);
  l.append (&i4);
  l.append (&i5);

  List<int*>::Iterator it = l.createIterator ();
  
  int *ptr;
  
  it.next (&ptr);
  
  cout << "i1: " << *ptr << endl;
  cout << "i2: " << *(l.at (2)) << endl;

  Application::shutdown ();

  return 0;
}

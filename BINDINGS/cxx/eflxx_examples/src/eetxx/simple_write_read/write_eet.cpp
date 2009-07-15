#include <eetxx/Eetxx.h>

#include <iostream>
#include <algorithm>
#include <cstring>

using namespace std;
using namespace efl;

struct A
{
  int x;
  double d;
  char s[10];
};


int main(int argc, char **argv)
{
  EetDocument::init ();

  EetDocument doc("writing_test.eet", EET_FILE_MODE_WRITE);
  EetList my_list (doc);

  int x = 2;
  cout << "Set x" << endl;
  my_list["test_int"] = x;
  cout << "---------" << endl;

  A a;
  a.x = 5;
  a.d = 12.2;
  memset(a.s, '\0', 10);
  memcpy(a.s, "john", 10);

  cout << "Set A" << endl;
  my_list["A"] = make_data (a, true);
  cout << "---------" << endl;

  cout << "Use intermediate object" << endl;
  const char *s = "first string";
  DataInformation<const char *> d = make_data(s, false, strlen (s)+1);
  my_list["my_string"] = d;
  cout << "---------" << endl;


  cout << "Use directly helper function" << endl;
  const char *s2 = "second string";
  my_list["my_string2"] =  make_data (s2, false, strlen (s)+1);
  cout << "--------" << endl;

  cout << "Set B" << endl;
  DataInformation<int> t2 (2, true);
  my_list["B"] = t2;
  cout << "--------" << endl;


  //eet_close(ef);
  doc.close ();
  eet_shutdown();

  return 0;
}

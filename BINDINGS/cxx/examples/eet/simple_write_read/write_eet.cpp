#include <eflpp_eet.h>

#include <iostream>
#include <algorithm>
#include <cstring>

struct A
{
  int x;
  double d;
  char s[10];
};


int main(int argc, char **argv)
{
  using namespace efl;
  eet_init();

  efl::eet_document doc("writing_test.eet", EET_FILE_MODE_WRITE);
  efl::eetlist my_list (doc);

  int x = 2;
  std::cout << "Set x" << std::endl;
  my_list["test_int"] = x;
  std::cout << "---------" << std::endl;

  A a;
  a.x = 5;
  a.d = 12.2;
  memset(a.s, '\0', 10);
  memcpy(a.s, "john", 10);

  std::cout << "Set A" << std::endl;
  my_list["A"] = efl::make_data (a, true);
  std::cout << "---------" << std::endl;

  std::cout << "Use intermediate object" << std::endl;
  const char *s = "first string";  
  data_information<const char *> d = make_data(s, false, strlen (s)+1);
  my_list["my_string"] = d;
  std::cout << "---------" << std::endl;

  
  std::cout << "Use directly helper function" << std::endl;
  const char *s2 = "second string";  
  my_list["my_string2"] =  make_data (s2, false, strlen (s)+1);
  std::cout << "--------" << std::endl;

  std::cout << "Set B" << std::endl;
  efl::data_information<int> t2 (2, true);
  my_list["B"] = t2;
  std::cout << "--------" << std::endl;


  //eet_close(ef);
  doc.close ();
  eet_shutdown();

  return 0;
}

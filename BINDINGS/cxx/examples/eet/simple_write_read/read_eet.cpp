#include <eflpp_eet.h>

#include <iostream>
#include <algorithm>

struct A
{
  int x;
  double d;
  char s[10];
};

std::ostream& operator<< (std::ostream& os, const A& a)
{
  os  << "x=" << a.x << " d=" << a.d << " s=" << a.s;
  return os;
}

std::ostream& operator<< (std::ostream& os, const efl::eet_chunk& chunk)
{
  void *data = chunk.get ();
  os  << chunk.get_key () << " ";

  if (chunk.get_key () == "A")
    os << *reinterpret_cast<A *> (data);
  else if ((chunk.get_key () == "my_string")
           || (chunk.get_key () == "my_string2"))
    os << reinterpret_cast<char *> (data);

  else
    os << *reinterpret_cast<int *> (data);

  return os;
}

struct display_eet_file_content
{
  void operator () (const efl::eet_chunk &chunk)
  {
    std::cout << chunk << std::endl;
  }
};

int main(int argc, char **argv)
{
  eet_init();

  efl::eet_document doc("writing_test.eet", EET_FILE_MODE_READ);
  efl::eetlist my_list (doc);

  
  //A *a = reinterpret_cast<A *> (my_list["A"]. get ());

  //std::cout << a->x << std::endl;
  //std::cout << a->d << std::endl;
  //std::cout << a->s << std::endl;

  //int *i = reinterpret_cast<int *> (my_list["B"]. get ());
  //std::cout << "i=" << *i << std::endl;

  std::for_each (my_list.begin (),
                 my_list.end (),
                 display_eet_file_content());

  doc.close ();
  eet_shutdown();

  return 0;
}

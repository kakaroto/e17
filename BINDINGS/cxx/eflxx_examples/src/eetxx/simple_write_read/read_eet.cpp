#include <eetxx/Eetxx.h>

#include <iostream>
#include <algorithm>

using namespace std;

struct A
{
  int x;
  double d;
  char s[10];
};

ostream& operator<< (ostream& os, const A& a)
{
  os  << "x=" << a.x << " d=" << a.d << " s=" << a.s;
  return os;
}

ostream& operator<< (ostream& os, const Eetxx::Chunk& chunk)
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
  void operator () (const Eetxx::Chunk &chunk)
  {
    cout << chunk << endl;
  }
};

int main(int argc, char **argv)
{
  Eetxx::Document::init ();

  Eetxx::Document doc("writing_test.eet", EET_FILE_MODE_READ);
  Eetxx::List my_list (doc);


  //A *a = reinterpret_cast<A *> (my_list["A"]. get ());

  //cout << a->x << endl;
  //cout << a->d << endl;
  //cout << a->s << endl;

  //int *i = reinterpret_cast<int *> (my_list["B"]. get ());
  //cout << "i=" << *i << endl;

  for_each (my_list.begin (),
            my_list.end (),
            display_eet_file_content());

  doc.close ();
  eet_shutdown();

  return 0;
}

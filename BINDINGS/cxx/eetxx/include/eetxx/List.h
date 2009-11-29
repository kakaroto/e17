#ifndef EET_LIST_H
#define EET_LIST_H

/* includes */
#include "Chunk.h"
#include "ChunkIterator.h"

namespace eet {

/* forward declarations */
class Document;

class List
{
public:
  List (Document& doc, const std::string &filter = "*" );

  ~List ();

  ChunkIterator begin ();

  ChunkIterator end ();

  const Chunk operator [] (const std::string &key_name) const;

  Chunk operator [] (const std::string &key_name);

private:
  Document &_doc;
  char     **_list;
  int        _nb_elems;
};

} // namespace eet

#endif // EET_LIST_H

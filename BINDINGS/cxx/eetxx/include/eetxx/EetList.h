#ifndef EET_LIST_H
#define EET_LIST_H

/* includes */
#include "EetChunk.h"
#include "ChunkIterator.h"

namespace efl {

/* forward declarations */
class EetDocument;

class EetList
{
public:
  EetList (EetDocument& doc, const std::string &filter = "*" );

  ~EetList ();

  ChunkIterator begin ();

  ChunkIterator end ();

  const EetChunk operator [] (const std::string &key_name) const;

  EetChunk operator [] (const std::string &key_name);

private:
  EetDocument &_doc;
  char     **_list;
  int        _nb_elems;
};

} // namespace efl

#endif // EET_LIST_H

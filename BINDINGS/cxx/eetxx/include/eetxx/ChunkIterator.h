#ifndef CHUNK_ITERATOR_H
#define CHUNK_ITERATOR_H

/* EFLxx includes */
#include "EetChunk.h"

namespace efl {

/* forward declarations */
class EetDocument;

class ChunkIterator :
      public std::iterator<std::forward_iterator_tag, char *, void>
{
public:
  ChunkIterator (char **chunk, EetDocument &doc);

  EetChunk operator * () const throw ();

  bool operator == (const ChunkIterator &i);

  bool operator != (const ChunkIterator &i);

  ChunkIterator &operator ++ () throw ();

  ChunkIterator operator ++ (int) throw ();

private:
  ChunkIterator ();
  char     **the_chunk;
  EetDocument  &_doc;
};

} // end namespace efl

#endif // CHUNK_ITERATOR_H

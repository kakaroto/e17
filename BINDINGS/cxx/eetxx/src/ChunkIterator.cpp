#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/eetxx/ChunkIterator.h"
#include "../include/eetxx/EetDocument.h"

namespace efl {

ChunkIterator::ChunkIterator (char **chunk, EetDocument &doc)
    : the_chunk (chunk),
    _doc(doc)
{}

EetChunk ChunkIterator::operator * () const throw ()
{
  return EetChunk (_doc.get (), *the_chunk);
}

bool ChunkIterator::operator == (const ChunkIterator &i)
{
  return the_chunk == i.the_chunk;
}

bool ChunkIterator::operator != (const ChunkIterator &i)
{
  return the_chunk != i.the_chunk;
}

ChunkIterator &ChunkIterator::operator ++ () throw ()
{
  the_chunk = &the_chunk[1];
  return *this;
}

ChunkIterator ChunkIterator::operator ++ (int) throw ()
{
  ChunkIterator tem (the_chunk, _doc);
  ++ *this;
  return tem;
}

} // end namespace efl

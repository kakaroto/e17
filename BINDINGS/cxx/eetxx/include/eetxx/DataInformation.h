#ifndef EET_DATA_INFORMATION_H
#define EET_DATA_INFORMATION_H

namespace Eetxx {

template <typename _Ty>
class DataInformation
{
public:
  DataInformation (_Ty data,
                   bool compress = false,
                   unsigned int size = sizeof (_Ty) )
      : _data (data),
      _compress (compress),
      _size (size)
  {}

  _Ty _data;
  bool _compress;
  unsigned int _size;
};

/* Helper function to avoid explicit template instantiation by the user */
template<typename _Ty>
DataInformation<_Ty>
make_data (_Ty data, bool compress = false, unsigned int size = sizeof (_Ty))
{
  return DataInformation<_Ty> (data, compress, size);
}

template<typename _Ty>
DataInformation<_Ty *>
make_data (_Ty *data, bool compress = false, unsigned int size = sizeof (_Ty))
{
  return DataInformation<_Ty *> (data, compress, size);
}

} // namespace Eetxx

#endif // EET_DATA_INFORMATION_H

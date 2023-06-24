#ifndef SERIALIZE_H
#define SERIALIZE_H

#include "serialize_binary.h"

enum class format_t
{
  binary
};

template <typename T>
int write (T& val, const std::string &path, format_t format = format_t::binary)
{
  if (format == format_t::binary)
    return write_binary (val, path);

  return 0;
}

template <typename T>
int read (T& val, const std::string &path, format_t format = format_t::binary)
{  
  if (format == format_t::binary)
    return read_binary (val, path);

  return -1;
}

#endif // SERIALIZE_H

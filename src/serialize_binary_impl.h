#ifndef SERIALIZE_BINARY_IMPL_H
#define SERIALIZE_BINARY_IMPL_H

#include "serialize_binary_type_traits.h"
#include <iostream>
#include <fstream>
#include <string>

//-------------------------------------------------------------------------
/// supported struct
///
//template<typename T>
//requires requires (T val, parameters_list pl) { val.build_parameter_list (pl); }
//bool write_binary_impl (T &/*val*/, std::ostream &/*os*/, int /*depth*/)
//{
////  parameters_list pl;
////  val.build_parameter_list (pl);
////  return pl.write (os);
//}

//template<typename T>
//requires requires (T val, parameters_list pl) { val.build_parameter_list (pl); }
//bool read_binary_impl (T &/*val*/, std::istream &/*is*/, int /*depth*/)
//{
////  parameters_list pl;
////  val.build_parameter_list (pl);
////  return pl.read (is);
//}

//template<typename T>
//requires requires (T val, parameters_list pl) { val.build_parameter_list (pl); }
//bool bytes_count_impl (const T &val, std::ostream &os)
//{
////  parameters_list pl;
////  val.build_parameter_list (pl);
////  return pl.write (os);
//}

//-------------------------------------------------------------------------
/// boolean
///
template<typename T, typename std::enable_if<is_boolean<T>::value, int>::type = 0>
bool write_binary_impl (const T &val, std::ostream &os)
{
  size_t bytes_to_write = bytes_count_impl (val);
  char ch = (val) ? 1 : 0;
  return (bool) os.write (&ch, bytes_to_write);
}

template<typename T, typename std::enable_if<is_boolean<T>::value, int>::type = 0>
bool read_binary_impl (T &val, std::istream &is)
{
  size_t bytes_to_write = bytes_count_impl (val);
  char ch;
  bool r = (bool) is.read (&ch, bytes_to_write);
  val = ch == 1;
  return r;
}

template<typename T, typename std::enable_if<is_boolean<T>::value, int>::type = 0>
size_t bytes_count_impl (const T &/*val*/)
{
  return sizeof (char);
}

//-------------------------------------------------------------------------
/// simple
///

template<typename T, typename std::enable_if<is_simple<T>::value, int>::type = 0>
size_t bytes_count_impl (const T &/*val*/)
{
  return sizeof (T);
}

template<typename T, typename std::enable_if<is_simple<T>::value, int>::type = 0>
bool write_binary_impl (const T &val, std::ostream &os)
{
  size_t bytes_to_write = bytes_count_impl (val);
  const char *ptr = reinterpret_cast<const char *> (&val);

  return (bool) os.write (ptr, bytes_to_write);
}

template<typename T, typename std::enable_if<is_simple<T>::value, int>::type = 0>
bool read_binary_impl (T &val, std::istream &is)
{
  size_t bytes_to_write = bytes_count_impl (val);
  char *ptr = reinterpret_cast<char *> (&val);

  return (bool) is.read (ptr, bytes_to_write);
}

//-------------------------------------------------------------------------
/// string
///

template<typename T, typename std::enable_if<is_string<T>::value, int>::type = 0>
size_t bytes_count_impl (const T &val)
{
  size_t size = val.size ();
  return bytes_count_impl (size) + size;
}

template<typename T, typename std::enable_if<is_string<T>::value, int>::type = 0>
bool write_binary_impl (const T &val, std::ostream &os)
{
  size_t bytes_to_write = val.size ();
  const char *ptr = val.c_str ();

  return write_binary_impl (bytes_to_write, os) && os.write (ptr, bytes_to_write);
}

template<typename T, typename std::enable_if<is_string<T>::value, int>::type = 0>
bool read_binary_impl (T &val, std::istream &is)
{
  size_t bytes_to_load = val.size ();

  if (!read_binary_impl (bytes_to_load, is))
      return false;

  val.resize (bytes_to_load);
  if (!bytes_to_load)
      return true;

  return (bool) is.read (reinterpret_cast<char *> (&val[0]), bytes_to_load);
}

//-------------------------------------------------------------------------
/// Enum
///

template<typename T>
requires requires (T val) { enum_to_string (val); }
size_t bytes_count_impl (const T &val)
{
  std::string string = enum_to_string (val);
  return bytes_count_impl (string);
}

template<typename T>
requires requires (T val) { enum_to_string (val); }
bool write_binary_impl (const T &val, std::ostream &os)
{
  std::string string = enum_to_string (val);
  return write_binary_impl (string, os);
}

template<typename T>
requires requires (T val) { enum_to_string (val); }
bool read_binary_impl (T &val, std::istream &is)
{
  std::string string;
  read_binary_impl (string, is);

  for (int i = 0; i < (int) T::COUNT; i++)
    {
      if (string != enum_to_string ((T) i))
        continue;

      val = (T) i;
      return true;
    }

  return false;
}

//-------------------------------------------------------------------------
/// vector
///

template<typename T, typename std::enable_if<is_vector<T>::value, int>::type = 0>
size_t bytes_count_impl (const T &val)
{
  size_t res = 0;
  size_t size = val.size ();

  res += bytes_count_impl (size);

  if (size == 0)
    return res;

  if (is_simple<typename T::value_type>::value)
    {
      res += sizeof (typename T::value_type) * size;
    }
  else
    {
      for (size_t i = 0; i < size; i++)
        res += bytes_count_impl (val[i]);
    }

  return res;
}

template<typename T, typename std::enable_if<is_vector<T>::value, int>::type = 0>
bool write_binary_impl (const T &val, std::ostream &os)
{
  size_t size = val.size ();
  if (!write_binary_impl (size, os))
    return false;

  if (size == 0)
    return true;

  if (is_simple<typename T::value_type>::value)
    {
      size_t bytes_to_write = sizeof (typename T::value_type) * size;
      const char *ptr = reinterpret_cast<const char *> (&val[0]);

      return (bool) os.write (ptr, bytes_to_write);
    }
  else
    {
      for (size_t i = 0; i < size; i++)
        {
          if (!write_binary_impl (val[i], os))
            return false;
        }
    }

  return true;
}

template<typename T, typename std::enable_if<is_vector<T>::value, int>::type = 0>
bool read_binary_impl (T &val, std::istream &is)
{
    size_t size;
    if (!read_binary_impl (size, is))
        return false;

    val.resize (size);
    if (size == 0)
        return true;

    if (is_simple<typename T::value_type>::value)
    {
        size_t bytes_to_read = sizeof (typename T::value_type) * size;
        char *ptr = reinterpret_cast<char *> (&val[0]);

        return (bool) is.read (ptr, bytes_to_read);
    }
    else
    {
        for (size_t i = 0; i < size; i++)
        {
            if (!read_binary_impl (val[i], is))
                return false;
        }
    }

    return true;
}

//-------------------------------------------------------------------------

template <typename T>
int write_binary (const T& val, const std::string &path)
{
  std::ofstream outfile;

  outfile.open (path, std::ofstream::binary);

  if (!outfile)
    return -1;

  int r = write_binary_impl (val, outfile);

  outfile.close ();
  return r;
}

template <typename T>
bool read_binary (T& val, const std::string &path)
{
  std::ifstream infile;

  infile.open (path);

  if (!infile)
    return -1;

  int r = read_binary_impl (val, infile);

  infile.close ();
  return r;
}


#endif // SERIALIZE_BINARY_IMPL_H

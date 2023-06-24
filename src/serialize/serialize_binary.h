#ifndef SERIALIZE_BINARY_H
#define SERIALIZE_BINARY_H

#include "serialize_binary_type_traits.h"
#include "serialize_list.h"
#include <iostream>
#include <fstream>
#include <string>

//-------------------------------------------------------------------------
/// boolean
///

template<typename T, typename std::enable_if<is_boolean<T>::value, int>::type = 0>
size_t __bytes_count (const T &/*val*/)
{
  return sizeof (char);
}

template<typename T, typename std::enable_if<is_boolean<T>::value, int>::type = 0>
bool __write_binary (const T &val, std::ostream &os)
{
  size_t bytes_to_write = __bytes_count (val);
  char ch = (val) ? 1 : 0;
  return (bool) os.write (&ch, bytes_to_write);
}

template<typename T, typename std::enable_if<is_boolean<T>::value, int>::type = 0>
bool __read_binary (T &val, std::istream &is)
{
  size_t bytes_to_write = __bytes_count (val);
  char ch;
  bool r = (bool) is.read (&ch, bytes_to_write);
  val = ch == 1;
  return r;
}

//-------------------------------------------------------------------------
/// simple
///

template<typename T, typename std::enable_if<is_simple<T>::value, int>::type = 0>
size_t __bytes_count (const T &/*val*/)
{
  return sizeof (T);
}

template<typename T, typename std::enable_if<is_simple<T>::value, int>::type = 0>
bool __write_binary (const T &val, std::ostream &os)
{
  size_t bytes_to_write = __bytes_count (val);
  const char *ptr = reinterpret_cast<const char *> (&val);

  return (bool) os.write (ptr, bytes_to_write);
}

template<typename T, typename std::enable_if<is_simple<T>::value, int>::type = 0>
bool __read_binary (T &val, std::istream &is)
{
  size_t bytes_to_write = __bytes_count (val);
  char *ptr = reinterpret_cast<char *> (&val);

  return (bool) is.read (ptr, bytes_to_write);
}

//-------------------------------------------------------------------------
/// string
///

template<typename T, typename std::enable_if<is_string<T>::value, int>::type = 0>
size_t __bytes_count (const T &val)
{
  size_t size = val.size ();
  return __bytes_count (size) + size;
}

template<typename T, typename std::enable_if<is_string<T>::value, int>::type = 0>
bool __write_binary (const T &val, std::ostream &os)
{
  size_t bytes_to_write = val.size ();
  const char *ptr = val.c_str ();

  return __write_binary (bytes_to_write, os) && os.write (ptr, bytes_to_write);
}

template<typename T, typename std::enable_if<is_string<T>::value, int>::type = 0>
bool __read_binary (T &val, std::istream &is)
{
  size_t bytes_to_load = val.size ();

  if (!__read_binary (bytes_to_load, is))
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
size_t __bytes_count (const T &val)
{
  std::string string = enum_to_string (val);
  return __bytes_count (string);
}

template<typename T>
requires requires (T val) { enum_to_string (val); }
bool __write_binary (const T &val, std::ostream &os)
{
  std::string string = enum_to_string (val);
  return __write_binary (string, os);
}

template<typename T>
requires requires (T val) { enum_to_string (val); }
bool __read_binary (T &val, std::istream &is)
{
  std::string string;
  __read_binary (string, is);

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
size_t __bytes_count (T &val)
{
  size_t res = 0;
  size_t size = val.size ();

  res += __bytes_count (size);

  if (size == 0)
    return res;

  if (is_simple<typename T::value_type>::value)
    {
      res += sizeof (typename T::value_type) * size;
    }
  else
    {
      for (size_t i = 0; i < size; i++)
        res += __bytes_count (val[i]);
    }

  return res;
}

template<typename T, typename std::enable_if<is_vector<T>::value, int>::type = 0>
bool __write_binary (T &val, std::ostream &os)
{
  size_t size = val.size ();
  if (!__write_binary (size, os))
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
          if (!__write_binary (val[i], os))
            return false;
        }
    }

  return true;
}

template<typename T, typename std::enable_if<is_vector<T>::value, int>::type = 0>
bool __read_binary (T &val, std::istream &is)
{
  size_t size;
  if (!__read_binary (size, is))
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
          val[i] = {};
          if (!__read_binary (val[i], is))
            return false;
        }
    }

  return true;
}

//-------------------------------------------------------------------------
/// map
///

template<typename T, typename std::enable_if<is_map<T>::value, int>::type = 0>
size_t __bytes_count (T &val)
{
  size_t res = 0;
  size_t size = val.size ();

  res += __bytes_count (size);

  for (const auto &[k, v]: val)
    {
      res += __bytes_count (k);
      res += __bytes_count (v);
    }

  return res;
}

template<typename T, typename std::enable_if<is_map<T>::value, int>::type = 0>
bool __write_binary (T &val, std::ostream &os)
{
  size_t size = val.size ();
  if (!__write_binary (size, os))
    return false;

  for (const auto &[k, v]: val)
    {
      if (!__write_binary (k, os))
        return false;

      if (!__write_binary (v, os))
        return false;
    }

  return true;
}

template<typename T, typename std::enable_if<is_map<T>::value, int>::type = 0>
bool __read_binary (T &val, std::istream &is)
{
  size_t size;
  if (!__read_binary (size, is))
    return false;

  bool r = true;
  for (size_t i = 0; i < size; i++)
    {
      typename T::key_type k;
      typename T::mapped_type  v;

      bool has_error = __read_binary (k, is);
      if (has_error)
        r = false;

      has_error = __read_binary (v, is);
      if (has_error)
        r = false;

      val.insert ({k, v});
    }

  return r;
}

//-------------------------------------------------------------------------
/// set
///

template<typename T, typename std::enable_if<is_set<T>::value, int>::type = 0>
size_t __bytes_count (T &val)
{
  size_t res = 0;
  size_t size = val.size ();

  res += __bytes_count (size);

  for (const auto &v: val)
    res += __bytes_count (v);

  return res;
}

template<typename T, typename std::enable_if<is_set<T>::value, int>::type = 0>
bool __write_binary (T &val, std::ostream &os)
{
  size_t size = val.size ();
  if (!__write_binary (size, os))
    return false;

  for (const auto &v: val)
    {
      if (!__write_binary (v, os))
        return false;
    }

  return true;
}

template<typename T, typename std::enable_if<is_set<T>::value, int>::type = 0>
bool __read_binary (T &val, std::istream &is)
{
  size_t size;
  if (!__read_binary (size, is))
    return false;

  bool r = true;
  for (size_t i = 0; i < size; i++)
    {
      typename T::key_type v;

      bool has_error = __read_binary (v, is);
      if (has_error)
        r = false;

      val.insert (v);
    }

  return r;
}

//-------------------------------------------------------------------------
// supported struct
//

template <typename T>
int value_parameter<T>::write_binary (std::ostream &os)
{
  return __write_binary (m_val, os);
}

template <typename T>
int value_parameter<T>::read_binary (std::istream &is)
{
  int r = __read_binary (m_val, is);
  if (m_do_after_read)
    m_do_after_read ();
  return r;
}

template <typename T>
size_t value_parameter<T>::bytes_count ()
{
  return __bytes_count (m_val);
}

template<typename T>
requires requires (T val, serialize_list pl) { val.build_parameter_list (pl); }
bool __write_binary (T &val, std::ostream &os)
{
  serialize_list pl;
  val.build_parameter_list (pl);

  for (auto &[name, param]: pl.get_list ())
    {
      const std::string &name_test = name;
      __write_binary (name_test, os);
      int r = param->write_binary (os);
      if (r < 0)
        return false;
    }

  return true;
}

template<typename T>
requires requires (T val, serialize_list pl) { val.build_parameter_list (pl); }
bool __read_binary (T &val, std::istream &is)
{
  serialize_list pl;
  val.build_parameter_list (pl);

  for (auto &[name, param]: pl.get_list ())
    {
      std::string name_in_stream;
      bool r = __read_binary (name_in_stream, is);
      if (!r)
        return false;

      if (name_in_stream != name)
        {
          is.seekg (-(std::streamoff) __bytes_count (name_in_stream), std::ios_base::cur);
        }
      else
        {
          param->read_binary (is);
        }
    }

  pl.do_after_read ();

  return true;
}

template<typename T>
requires requires (T val, serialize_list pl) { val.build_parameter_list (pl); }
size_t __bytes_count (T &val)
{
  serialize_list pl;
  val.build_parameter_list (pl);

  size_t res = 0;
  for (const auto &[name, param]: pl.get_list ())
    {
      res += __bytes_count (name);
      res += param->bytes_count ();
    }

  return res;
}

//-------------------------------------------------------------------------

template <typename T>
int write_binary (T& val, const std::string &path)
{
  std::ofstream outfile;

  outfile.open (path, std::ofstream::binary);

  if (!outfile)
    return -1;

  int r = 0;
  __write_binary (val, outfile);

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

  int r = 0;
  __read_binary (val, infile);

  infile.close ();
  return r;
}

template <typename T>
size_t bytes_count (T& val)
{
  return __bytes_count (val);
}


#endif // SERIALIZE_BINARY_H

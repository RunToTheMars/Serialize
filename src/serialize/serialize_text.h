#ifndef SERIALIZE_TEXT_H
#define SERIALIZE_TEXT_H

#include "serialize_text_type_traits.h"
#include "serialize_list.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>

//-------------------------------------------------------------------------
/// boolean
///

template<typename T, typename std::enable_if<text::is_boolean<T>::value, int>::type = 0>
size_t __depth_count (const T &/*val*/)
{
  return 1;
}

template<typename T, typename std::enable_if<text::is_boolean<T>::value, int>::type = 0>
size_t __lines_count (const T &/*val*/)
{
  return 1;
}

template<typename T, typename std::enable_if<text::is_boolean<T>::value, int>::type = 0>
bool __write_text (const T &val, std::ostream &os)
{
  if (val)
    return (bool) (os << "true");
  else
    return (bool) (os << "false");

  return true;
}

template<typename T, typename std::enable_if<text::is_boolean<T>::value, int>::type = 0>
bool __read_text (T &val, std::istream &is)
{
  std::string line;
  bool r = (bool) (is >> line);
  if (!r)
    return false;

  std::transform (line.begin (), line.end (), line.begin (), [](unsigned char c) { return std::tolower (c); });

  if (line == "true" || line == "yes" || line == "1")
    {
      val = true;
      return true;
    }
  else if (line == "false" || line == "no" || line == "0")
    {
      val = false;
      return true;
    }

  return false;
}

//-------------------------------------------------------------------------
/// stream_supported
///

template<typename T, typename std::enable_if<text::is_stream_supported<T>::value, int>::type = 0>
size_t __depth_count (const T &/*val*/)
{
  return 1;
}

template<typename T, typename std::enable_if<text::is_stream_supported<T>::value, int>::type = 0>
size_t __lines_count (const T &/*val*/)
{
  return 1;
}

template<typename T, typename std::enable_if<text::is_stream_supported<T>::value, int>::type = 0>
bool __write_text (const T &val, std::ostream &os)
{
  return (bool) (os << val);
}

template<typename T, typename std::enable_if<text::is_stream_supported<T>::value, int>::type = 0>
bool __read_text (T &val, std::istream &is)
{
  return (bool) (is >> val);
}

//-------------------------------------------------------------------------
/// string
///

template<typename T, typename std::enable_if<text::is_string<T>::value, int>::type = 0>
size_t __depth_count (const T &/*val*/)
{
  return 1;
}

template<typename T, typename std::enable_if<text::is_string<T>::value, int>::type = 0>
size_t __lines_count (const T &/*val*/)
{
  return 1;
}

template<typename T, typename std::enable_if<text::is_string<T>::value, int>::type = 0>
bool __write_text (const T &val, std::ostream &os)
{
  return (bool) (os << val);
  return true;
}

template<typename T, typename std::enable_if<text::is_string<T>::value, int>::type = 0>
bool __read_text (T &val, std::istream &is)
{
  return (bool) std::getline (is, val);
}

//-------------------------------------------------------------------------
/// vector
///

template<typename T, typename std::enable_if<text::is_vector<T>::value, int>::type = 0>
size_t __depth_count (const T &val)
{
  size_t r = 0;
  for (auto &v: val)
    {
      size_t d = __depth_count (v);
      if (d > r)
        r = d;
    }

  return r + 1;
}

template<typename T, typename std::enable_if<text::is_vector<T>::value, int>::type = 0>
size_t __lines_count (const T &val)
{
  size_t r = 2;
  for (auto &v: val)
    r += __lines_count (v);

  return r;
}

template<typename T, typename std::enable_if<text::is_vector<T>::value, int>::type = 0>
bool __write_text (const T &val, std::ostream &os)
{
  bool r_start = (bool) (os << "[" << "\n");

  bool r_values = true;
  for (auto &v: val)
    {
      if (!__write_text (v, os))
        r_values = false;

      if (!text::is_vector<typename T::value_type>::value)
        {
          if (! (bool) (os << "\n"))
            r_values = false;
        }
    }

  bool r_end = (bool) (os << "]" << "\n");
  return r_start && r_values && r_end;
}

template<typename T, typename std::enable_if<text::is_vector<T>::value, int>::type = 0>
bool __read_text (T &val, std::istream &is)
{
  std::string line;
  bool r_start = (bool) (is >> line);

  if (!r_start || line != "[")
    return false;


  while (true)
  {
    line.clear ();
    if (!__read_text (line, is))
      return false;

    if (line == "]")
      return true;

    std::stringstream ss;
    ss << line;
    typename T::value_type v;

    if (!__read_text (v, ss))
      continue;

    val.push_back (v);
  }

  return false;
}

//-------------------------------------------------------------------------

template <typename T>
int write_text (T& val, const std::string &path)
{
  std::ofstream outfile;

  outfile.open (path);

  if (!outfile)
    return -1;

  int r = 0;
  __write_text (val, outfile);

  outfile.close ();
  return r;
}

template <typename T>
bool read_text (T& val, const std::string &path)
{
  std::ifstream infile;

  infile.open (path);

  if (!infile)
    return -1;

  int r = 0;
  __read_text (val, infile);

  infile.close ();
  return r;
}

template <typename T>
size_t depth_count (T& val)
{
  return __depth_count (val);
}

template <typename T>
size_t lines_count (T& val)
{
  return __lines_count (val);
}

#endif

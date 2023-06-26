#ifndef SERIALIZE_TEXT_H
#define SERIALIZE_TEXT_H

#include "serialize_text_type_traits.h"
#include "serialize_list.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>

struct __text_preffix
{
  std::string one_space = "  ";

  std::string space;
  int depth;

  __text_preffix &increment ()
  {
    space = space + one_space;
    depth = depth + 1;

    return *this;
  }

  __text_preffix &decrement ()
  {
    space = space.substr (0, space.size () - one_space.size ());
    depth --;

    return *this;
  }
};

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
bool __write_text (const T &val, std::ostream &os, __text_preffix &/*preffix*/)
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
bool __write_text (const T &val, std::ostream &os, __text_preffix &/*preffix*/)
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
bool __write_text (const T &val, std::ostream &os, __text_preffix &/*preffix*/)
{
  return (bool) (os << '\"' << val << '\"');
  return true;
}

template<typename T, typename std::enable_if<text::is_string<T>::value, int>::type = 0>
bool __read_text_without_spaces (T &val, std::istream &is)
{
  bool r = (bool) std::getline (is, val);
  if (!r)
    return false;

  int spaces = 0;
  for (spaces = 0; spaces < val.size (); spaces++)
    {
      if (val[spaces] != ' ')
        break;
    }

  val = val.substr (spaces, val.size ());
  return true;
}

template<typename T, typename std::enable_if<text::is_string<T>::value, int>::type = 0>
bool __read_text (T &val, std::istream &is)
{
  char c;

  do
  {
    bool r = (bool )is.read (&c, 1);
    if (!r)
      return false;
  } while (c != '"');

  val.clear ();

  do
  {
    bool r = (bool )is.read (&c, 1);
    if (!r)
      return false;

    if (c == '"')
      return true;

    val.push_back (c);
  } while (1);

  return true;
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
bool __write_text (const T &val, std::ostream &os, __text_preffix &preffix)
{ 
  bool r_start = (bool) (os << "[" << '\n');

  bool r_values = true;
  preffix.increment ();
  for (int i = 0; i < val.size (); i++)
    {
      os << preffix.space;
      if (!__write_text (val[i], os, preffix))
        r_values = false;

      if (i != val.size () - 1)
        if (! (bool) (os << ","))
          r_values = false;

      if (! (bool) (os << '\n'))
        r_values = false;
    }
  preffix.decrement ();

  bool r_end = (bool) (os << preffix.space << "]");
  return r_start && r_values && r_end;
}

template<typename T, typename std::enable_if<text::is_vector<T>::value, int>::type = 0>
bool __read_text (T &val, std::istream &is)
{
  char c;

  do
  {
    bool r = (bool )is.read (&c, 1);
    if (!r)
      return false;
  } while (c == ' ');

  if (c != '[')
    return false;

  while (true)
  {
    typename T::value_type v;

    if (__read_text (v, is))
      val.push_back (v);

    do
    {
      bool r = (bool )is.read (&c, 1);
      if (!r)
        return false;
    } while (c == ' ');

    if (c == ']')
      return true;
  }

  return false;
}

//template<typename T, typename std::enable_if<text::is_vector<T>::value, int>::type = 0>
//bool __read_text (T &val, std::istream &is)
//{
//  std::string line;

//  if (!__read_text_without_spaces (line, is))
//    return false;

//  if (line != "[")
//    return false;

//  while (true)
//  {
//    line.clear ();

//    int pos = is.tellg ();

//    if (!__read_text_without_spaces (line, is))
//      return false;

//    if (line == "]")
//      return true;

//    if (line == "," || line == "\n")
//      continue;

//    char c;
//    if (line == "")
//      is.read (&c, 1);
//    else
//      is.seekg (pos, is.beg);

//    typename T::value_type v;

//    pos = is.tellg ();
//    if (!__read_text (v, is))
//      continue;

//    val.push_back (v);
//  }

//  return false;
//}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/// set
///

template<typename T, typename std::enable_if<text::is_set<T>::value, int>::type = 0>
size_t __depth_count (const T &/*val*/)
{
  return 0;
}

template<typename T, typename std::enable_if<text::is_set<T>::value, int>::type = 0>
size_t __lines_count (const T &/*val*/)
{
  return 0;
}

template<typename T, typename std::enable_if<text::is_set<T>::value, int>::type = 0>
bool __write_text (const T &val, std::ostream &os, __text_preffix &preffix)
{
  bool r_start = (bool) (os << "[" << '\n');

  bool r_values = true;
  preffix.increment ();
  int size = val.size ();
  int i = 0;
  for (const typename T::value_type &v: val)
    {
      os << preffix.space;
      if (!__write_text (v, os, preffix))
        r_values = false;

      if (i != size - 1)
        if (! (bool) (os << ","))
          r_values = false;

      if (! (bool) (os << '\n'))
        r_values = false;
      i ++;
    }
  preffix.decrement ();

  bool r_end = (bool) (os << preffix.space << "]");
  return r_start && r_values && r_end;
}

template<typename T, typename std::enable_if<text::is_set<T>::value, int>::type = 0>
bool __read_text (T &val, std::istream &is)
{
  char c;

  do
  {
    bool r = (bool )is.read (&c, 1);
    if (!r)
      return false;
  } while (c == ' ');

  if (c != '[')
    return false;

  while (true)
  {
    typename T::value_type v;

    if (__read_text (v, is))
      val.insert (v);

    do
    {
      bool r = (bool )is.read (&c, 1);
      if (!r)
        return false;
    } while (c == ' ');

    if (c == ']')
      return true;
  }

  return false;
}

template <typename T>
int write_text (T& val, const std::string &path)
{
  std::ofstream outfile;

  outfile.open (path);

  if (!outfile)
    return -1;

  int r = 0;
  __text_preffix preffix;
  __write_text (val, outfile, preffix);

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

  std::string preview;
  infile.seekg (0, infile.end);
  int size = infile.tellg ();
  preview.resize (size);
  infile.seekg (0, infile.beg);
  infile.read (&preview[0], size);
  infile.seekg (0, infile.beg);

  std::stringstream ss;
  std::erase (preview, '\n');
//  std::erase (preview, ' ');

  ss << preview;

  int r = 0;
  __read_text (val, ss);

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

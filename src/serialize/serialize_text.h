#ifndef SERIALIZE_TEXT_H
#define SERIALIZE_TEXT_H

#include "serialize_text_type_traits.h"
#include "serialize_list.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>

inline bool __move_right (std::istream &is, std::string &str, size_t bytes)
{
  str.resize (bytes);
  return (bool) is.read (&str[0], bytes);
}

inline bool __move_left (std::istream &is, size_t bytes)
{
  std::istream::pos_type pos = is.tellg () ;
  pos -= std::streamoff (bytes);
  return (bool) is.seekg (pos);
}

inline bool __skip_tabs__ (std::istream &is, std::string tab)
{
  if (tab.empty ())
    return true;

  std::string buf;
  while (true)
    {
      if (!__move_right (is, buf, tab.size ()))
        return false;

      if (buf != tab)
        {
          return __move_left (is, tab.size ());
        }

    }

  return true;
}

inline bool __skip_if_next_is__ (std::istream &is, std::string str, bool &eq)
{
  eq = false;
  std::string buf;

  if (!__move_right (is, buf, str.size ()))
    return false;

  if (str == buf)
    {
      eq = true;
      return true;
    }

  return __move_left (is, str.size ());
}

//-------------------------------------------------------------------------
/// boolean
///

/// Example.txt:
///>true
///>false
///>no
///>yes
///>0
///>1

template<typename T, typename std::enable_if<text::is_boolean<T>::value, int>::type = 0>
bool __write_text (T &val, std::ostream &os, __text_streamer &/*streamer*/)
{
  if (val)
    return (bool) (os << "true");
  else
    return (bool) (os << "false");

  return true;
}

template<typename T, typename std::enable_if<text::is_boolean<T>::value, int>::type = 0>
bool __read_text (T &val, std::istream &is, __text_streamer &/*streamer*/)
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

/// Example.txt:
///>1e-5
///>c
///>15

template<typename T, typename std::enable_if<text::is_stream_supported<T>::value, int>::type = 0>
bool __write_text (T &val, std::ostream &os, __text_streamer &/*preffix*/)
{
  return (bool) (os << val);
}

template<typename T, typename std::enable_if<text::is_stream_supported<T>::value, int>::type = 0>
bool __read_text (T &val, std::istream &is, __text_streamer &/*preffix*/)
{
  return (bool) (is >> val);
}

//-------------------------------------------------------------------------
/// string
///

/// Example.txt:
///>"C++"

template<typename T, typename std::enable_if<text::is_string<T>::value, int>::type = 0>
bool __write_text (T &val, std::ostream &os, __text_streamer &/*streamer*/)
{
  return (bool) (os << '\"' << val << '\"');
  return true;
}

template<typename T, typename std::enable_if<text::is_string<T>::value, int>::type = 0>
bool __read_text (T &val, std::istream &is, __text_streamer &/*streamer*/)
{
  char c;

  if (!is.read (&c, 1))
    return false;

  if (c != '"')
    return false;

  val.clear ();

  do
  {
    if (!is.read (&c, 1))
      return false;

    if (c == '"')
      return true;

    val.push_back (c);
  } while (1);

  return true;
}

//-------------------------------------------------------------------------
/// abstract_getter
///

/// Example.txt:
///><TAB>     ...<TAB><LEFT_BRACKET>
///><TAB><TAB>...<TAB>"C++"<POSTFIX>
///><TAB><TAB>...<TAB>"is Best"<LAST_POSTFIX>
///><TAB>     ...<TAB><RIGHT_BRACKET>

template <typename T>
class abstract_getter
{
public:
  virtual ~abstract_getter () = default;

  virtual bool is_end () const = 0;
  virtual T &get_value () const = 0;
  virtual void next () = 0;
};

template <typename T, typename Iter>
class std_getter: public abstract_getter<T>
{
  Iter m_start;
  Iter m_end;

public:
  std_getter (Iter start, Iter end)
  {
    m_start = start;
    m_end = end;
  }

  bool is_end () const override
  {
    return m_start == m_end;
  }

  T &get_value () const override
  {
    return const_cast <T &> (*m_start);
  }

  void next () override
  {
    m_start++;
  }
};

template <typename T>
bool __write_getter (abstract_getter<T> &getter, std::ostream &os, __text_streamer &streamer)
{
  bool r_start = (bool) (os << streamer.cur_tab << streamer.settings.left_bracket);

  bool r_values = true;

  if (!getter.is_end ())
    {
      streamer.increment ();
      for (;;)
        {
          os << streamer.cur_tab;
          if (!__write_text (getter.get_value (), os, streamer))
            r_values = false;

          getter.next ();
          if (!getter.is_end ())
            {
              if (! (bool) (os << streamer.settings.postfix))
                r_values = false;
            }
          else
            {
              if (! (bool) (os << streamer.settings.last_postfix))
                r_values = false;

              break;
            }
        }
      streamer.decrement ();
    }

  bool r_end = (bool) (os << streamer.cur_tab << streamer.settings.right_bracket);
  return r_start && r_values && r_end;
}

template <typename T>
class abstract_setter
{
public:
  virtual ~abstract_setter () = default;

  virtual void add (T &&val) = 0;
};

template <typename T, typename C>
class std_push_back: public abstract_setter<T>
{
  C &m_container;

public:
  std_push_back (C &container): m_container (container) { }

  void add (T &&val) override
  {
    m_container.push_back (std::move (val));
  }
};

template <typename T, typename C>
class std_insert: public abstract_setter<T>
{
  C &m_container;

public:
  std_insert (C &container): m_container (container) { }

  void add (T &&val) override
  {
    m_container.insert (std::move (val));
  }
};

template<typename T>
bool __read_setter (abstract_setter<T> &val, std::istream &is, __text_streamer &preffix)
{
  if (!__skip_tabs__ (is, preffix.settings.tab))
    return false;

  {
    bool next_is_left_bracket;
    if (!__skip_if_next_is__ (is, preffix.settings.left_bracket, next_is_left_bracket))
      return false;

    if (!next_is_left_bracket)
      return false;
  }

  while (true)
  {
    T v;

    if (!__skip_tabs__ (is, preffix.settings.tab))
      return false;

    {
      bool next_is_right_bracket;
      if (!__skip_if_next_is__ (is,  preffix.settings.right_bracket, next_is_right_bracket))
        return false;

      if (next_is_right_bracket)
        return true;
    }

    if (__read_text (v, is, preffix))
      val.add (std::move (v));

    {
      bool next_is_postfix;
      if (!__skip_if_next_is__ (is, preffix.settings.postfix, next_is_postfix))
        return false;

      if (next_is_postfix)
        continue;
    }

    {
      bool next_is_last_postfix;
      if (!__skip_if_next_is__ (is, preffix.settings.last_postfix, next_is_last_postfix))
        return false;

      if (!next_is_last_postfix)
        return false;
    }
  }

  return false;
}


//-------------------------------------------------------------------------
/// vector
///

template<typename T, typename std::enable_if<text::is_vector<T>::value, int>::type = 0>
bool __write_text (T &val, std::ostream &os, __text_streamer &streamer)
{ 
  std_getter<typename T::value_type, typename T::iterator> vector_getter (val.begin (), val.end ());
  return __write_getter (vector_getter, os, streamer);
}

template<typename T, typename std::enable_if<text::is_vector<T>::value, int>::type = 0>
bool __read_text (T &val, std::istream &is, __text_streamer &preffix)
{
  std_push_back<typename T::value_type, T> vector_getter (val);
  return __read_setter (vector_getter, is, preffix);
}

//-------------------------------------------------------------------------
/// set
///

template<typename T, typename std::enable_if<text::is_set<T>::value, int>::type = 0>
bool __write_text (T &val, std::ostream &os, __text_streamer &streamer)
{
  std_getter<typename T::value_type, typename T::iterator> vector_getter (val.begin (), val.end ());
  return __write_getter (vector_getter, os, streamer);
}

template<typename T, typename std::enable_if<text::is_set<T>::value, int>::type = 0>
bool __read_text (T &val, std::istream &is, __text_streamer &preffix)
{
  std_insert<typename T::value_type, T> set_getter (val);
  return __read_setter (set_getter, is, preffix);
}

//-------------------------------------------------------------------------
// supported struct
//

template <typename T>
int value_parameter<T>::write_text (std::ostream &/*os*/, __text_streamer &/*streamer*/)
{
//  return __write_text (m_val, os, prefix);
  return 0;
}

template <typename T>
int value_parameter<T>::read_text (std::istream &/*is*/, __text_streamer &/*streamer*/)
{
//  int r = __read_text (m_val, is);
//  if (m_do_after_read)
//    m_do_after_read ();
//  return r;
    return 0;
}

//template<typename T>
//requires requires (T val, serialize_list pl) { val.build_parameter_list (pl); }
//size_t __depth_count (T &/*val*/)
//{
//  return 0;
//}

//template<typename T>
//requires requires (T val, serialize_list pl) { val.build_parameter_list (pl); }
//size_t __lines_count (T &/*val*/)
//{
//  return 0;
//}

//template<typename T>
//requires requires (T val, serialize_list pl) { val.build_parameter_list (pl); }
//bool __write_text (T &val, std::ostream &os, __text_streamer &preffix)
//{
//  serialize_list pl;
//  val.build_parameter_list (pl);

//  for (auto &[name, param]: pl.get_list ())
//    {
//      std::string &name_test = name;
//      __write_text (name_test, os, preffix);
//      os << ':';
//      int r = param->write_text (os, preffix);
//      if (r < 0)
//        return false;
//    }

//  return true;
//}

//template<typename T>
//requires requires (T val, serialize_list pl) { val.build_parameter_list (pl); }
//bool __read_text (T &val, std::istream &is)
//{
//  serialize_list pl;
//  val.build_parameter_list (pl);

//  for (auto &[name, param]: pl.get_list ())
//    {
//      std::string name_in_stream;
//      bool r = __read_text (name_in_stream, is);
//      if (!r)
//        return false;

//      char c;
//      r = (bool )is.read (&c, 1);
//      if (!r)
//        return false;

//      if (c != ':')
//        return false;

//      param->read_text (is);
//    }

//  pl.do_after_read ();

//  return true;
//}

//-------------------------------------------------------------------------

template <typename T>
int write_text (T& val, std::string &path)
{
  std::ofstream outfile;

  outfile.open (path);

  if (!outfile)
    return -1;

  int r = 0;
  __text_streamer streamer;
  __write_text (val, outfile, streamer);

  outfile.close ();
  return r;
}

template <typename T>
bool read_text (T& val, std::string &path)
{
  std::ifstream infile;

  infile.open (path);

  if (!infile)
    return -1;

  int r = 0;
  __text_streamer streamer;
  __read_text (val, infile, streamer);

  infile.close ();
  return r;
}

#endif

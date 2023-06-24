#ifndef SERIALIZE_PARAMETER_LIST_H
#define SERIALIZE_PARAMETER_LIST_H

#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include <map>
#include <memory>

class abstract_parameter
{
public:
  virtual ~abstract_parameter () = default;

  virtual int write_binary (std::ostream &os) = 0;
  virtual int read_binary (std::istream &is) = 0;
  virtual size_t bytes_count () = 0;
};

template <typename T>
class value_parameter: public abstract_parameter
{
  T &m_val;
  std::function<void ()> m_do_after_read;

public:
  value_parameter (T &val, std::function<void ()> after_read): m_val (val), m_do_after_read (after_read)
  {
  }

  int write_binary (std::ostream &os) override;
  int read_binary (std::istream &is) override;
  size_t bytes_count () override;
};

class serialize_list
{
  std::map<std::string, std::unique_ptr<abstract_parameter>> m_list;
  std::function<void ()> m_do_after_read;

public:
  template <typename T>
  void add (std::string name, T &val, std::function<void ()> after_read = {})
  {
    m_list[std::move (name)] = std::make_unique<value_parameter<T>> (val, std::move (after_read));
  }

  void set_do_after_read (std::function<void ()> after_read)
  {
    m_do_after_read = std::move (after_read);
  }

  std::map<std::string, std::unique_ptr<abstract_parameter>> &get_list () { return m_list; }
//  const std::map<std::string, std::unique_ptr<abstract_parameter>> &get_list () const { return m_list; }
  void do_after_read ()
  {
    if (m_do_after_read)
      m_do_after_read ();
  }
};

#endif // SERIALIZE_PARAMETER_LIST_H

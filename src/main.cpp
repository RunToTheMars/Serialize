#include "serialize.h"

struct example_1
{
  int m_age;
  std::string m_name;

  example_1 (): example_1 (0, "") {}
  example_1 (int age, std::string name) { m_age = age; m_name = name; }

  void build_parameter_list (serialize_list &pl)
  {
    pl.add ("age", m_age);
    pl.add ("name", m_name);
  }

  bool operator == (const example_1 &rhs) const { return m_age == rhs.m_age && m_name == rhs.m_name; }
};

template <typename T>
int make_test (T val, std::string path)
{
  int r = write (val, path);
  if (r < 0)
    return -1;

  T readed_val;
  r = read (readed_val, path);
  if (r < 0)
    return -1;

  if (readed_val != val)
    return -1;

  return 0;
}

int main ()
{
  if (make_test (2, "int.txt") < 0)
    return -1;

  if (make_test (5., "double.txt") < 0)
    return -1;

  if (make_test<std::string> ("TEST WORD", "string.txt") < 0)
    return -1;

  if (make_test<std::string> ("Проверка", "rus_string.txt") < 0)
    return -1;

  if (make_test (std::vector<int> {1, 2, 3, 4, 5}, "vec_int.txt") < 0)
    return -1;

  if (make_test (std::vector<double> {1., 2., 3., 4., 5.}, "vec_double.txt") < 0)
    return -1;

  if (make_test (std::vector<std::string> {"I", "Don't", "Love", "C++"}, "vec_string.txt") < 0)
    return -1;

  if (make_test (example_1 (25, "Stanislav"), "example_string.txt") < 0)
    return -1;

  return 0;
}

#include "serialize.h"

template <typename T>
int make_test (const T &val, std::string path)
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

  return 0;
}

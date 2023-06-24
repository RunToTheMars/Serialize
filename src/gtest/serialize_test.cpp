#include <gtest/gtest.h>
#include <filesystem>

#include "serialize.h"
#include "custom_types.h"

std::string base_dir = "gtest_serialize_test_directory";

size_t bytes_count_file (const std::string &path)
{
  std::ifstream infile;

  infile.open (path);

  if (!infile)
    return 0;

  infile.seekg(0, std::ios::end);
  std::streamsize ssize = infile.tellg ();
  size_t bytes = static_cast<size_t> (ssize);
  infile.close ();

  return bytes;
}

template <typename T1, typename T2>
void make_primitive_test (T1 to_read, T2 from_write, const std::string &filename, const std::string &current_dir)
{
  std::string path = current_dir + "/" + filename;

  write (to_read, path);
  EXPECT_EQ (bytes_count (to_read), bytes_count_file (path));

  T2 readed_val;

  read (readed_val, path);
  EXPECT_EQ (from_write, readed_val);
}

template <typename T>
void make_save_load_identity_test (T val, const std::string &filename, const std::string &current_dir)
{
  make_primitive_test (val, val, filename, current_dir);
}

TEST (serialize_test, make_save_load_identity_test)
{
  std::string test_dir = base_dir + "/" + ::testing::UnitTest::GetInstance ()->current_test_info ()->name ();
  std::filesystem::create_directories (test_dir);

  /// c++ standard types
  {
    std::string current_dir = test_dir + "/" + "cpp_standard_types";
    std::filesystem::create_directories (current_dir);

    make_save_load_identity_test (2   ,    "int.txt", current_dir);
    make_save_load_identity_test (5.  , "double.txt", current_dir);
    make_save_load_identity_test (true,   "bool.txt", current_dir);
    make_save_load_identity_test ('c' ,   "char.txt", current_dir);
  }

  /// c++ std types
  {
    std::string current_dir = test_dir + "/" + "std_types";
    std::filesystem::create_directories (current_dir);

    make_save_load_identity_test (std::string ("TEST WORD"),     "string.txt", current_dir);
    make_save_load_identity_test (std::string ("Проверка") , "rus_string.txt", current_dir);

    make_save_load_identity_test (std::vector<int>    {1, 2, 3, 4, 5}     ,    "vec_int.txt", current_dir);
    make_save_load_identity_test (std::vector<double> {1., 2., 3., 4., 5.}, "vec_double.txt", current_dir);

    make_save_load_identity_test (std::vector<std::string> {"I", "Don't", "Love", "C++"}, "vec_string.txt", current_dir);
  }

  /// composite types
  {
    std::string current_dir = test_dir + "/" + "composite_types";
    std::filesystem::create_directories (current_dir);

    make_save_load_identity_test (std::vector<int>    {1, 2, 3, 4, 5}     ,    "vec_int.txt", current_dir);
    make_save_load_identity_test (std::vector<double> {1., 2., 3., 4., 5.}, "vec_double.txt", current_dir);

    {
      std::vector<int> v1 = {1, 2, 3};
      std::vector<int> v2 = {4, 5, 6};
      std::vector<std::vector<int>> V = {v1, v2};
      make_save_load_identity_test (V, "vec_vec_int.txt", current_dir);
    }

    make_save_load_identity_test (std::vector<std::string> {"I", "Don't", "Love", "C++"}, "vec_string.txt", current_dir);

    {
      std::map<int, double> m;
      m[0] = 1.;
      m[1] = 1.5;
      make_save_load_identity_test (m, "map_int_double.txt", current_dir);
    }

    make_save_load_identity_test (std::set<char> {'s', 't', 'a'}, "set_char.txt", current_dir);
  }

  /// custom types
  {
    std::string current_dir = test_dir + "/" + "custom_types";
    std::filesystem::create_directories (current_dir);

    make_save_load_identity_test (pair {1, 2}, "pair.txt", current_dir);

    {
      vec_pair vp;
      vp.pairs.emplace_back (1, 2);
      vp.pairs.emplace_back (3, 4);
      make_save_load_identity_test (vp, "vec_pair.txt", current_dir);
    }

    {
      student s;
      s.age = 25;
      s.name = "Stanislav";
      s.is_valid = false;

      make_save_load_identity_test (s, "student.txt", current_dir);
    }
  }
}

TEST (serialize_test, mix_fields_test)
{
  std::string test_dir = base_dir + "/" + ::testing::UnitTest::GetInstance ()->current_test_info ()->name ();
  std::filesystem::create_directories (test_dir);

  std::string current_dir = test_dir;

  student_1 s1;
  s1.age = 25;
  s1.name = "Stanislav";
  s1.is_valid = false;
  s1.t = 15;

  make_primitive_test (student_2 (s1), s1, "s2.txt", current_dir);
  make_primitive_test (student_3 (s1), s1, "s3.txt", current_dir);
  make_primitive_test (student_4 (s1), s1, "s4.txt", current_dir);
}

TEST (serialize_test, add_new_fields_test)
{
  std::string test_dir = base_dir + "/" + ::testing::UnitTest::GetInstance ()->current_test_info ()->name ();
  std::filesystem::create_directories (test_dir);

  std::string current_dir = test_dir;

  student s;
  s.age = 25;
  s.name = "Stanislav";
  s.is_valid = false;

  student_new s_new;
  s_new.age = 25;
  s_new.name = "Stanislav";
  s_new.is_valid = false;
  s_new.t = 0;

  make_primitive_test (s, student_1 (s_new), "s1.txt", current_dir);
  make_primitive_test (s, student_2 (s_new), "s2.txt", current_dir);
  make_primitive_test (s, student_3 (s_new), "s3.txt", current_dir);
  make_primitive_test (s, student_4 (s_new), "s4.txt", current_dir);
}

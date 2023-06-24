#ifndef CUSTOM_TYPES_H
#define CUSTOM_TYPES_H

#include <vector>
#include <string>

class serialize_list;

struct pair
{
  int x;
  int y;

  bool operator == (const pair &rhs) const { return x == rhs.x && y == rhs.y; }

  void build_parameter_list (serialize_list &pl);
};

struct vec_pair
{
  std::vector<pair> pairs;

  bool operator == (const vec_pair &rhs) const { return pairs == rhs.pairs; }
  void build_parameter_list (serialize_list &pl);
};

struct student
{
  bool is_valid;
  std::string name;
  unsigned int age;

  bool operator == (const student &rhs) const { return is_valid == rhs.is_valid && name == rhs.name && age == rhs.age; }
  void build_parameter_list (serialize_list &pl);
};

struct student_new
{
  int t = 0;
  bool is_valid;
  std::string name;
  unsigned int age;

  bool operator == (const student_new &rhs) const { return t == rhs.t && is_valid == rhs.is_valid && name == rhs.name && age == rhs.age; }
  void build_parameter_list (serialize_list &pl);
};

struct student_1: public student_new
{
  void build_parameter_list (serialize_list &pl);
};

struct student_2: public student_new
{
  void build_parameter_list (serialize_list &pl);
};

struct student_3: public student_new
{
  void build_parameter_list (serialize_list &pl);
};

struct student_4: public student_new
{
  void build_parameter_list (serialize_list &pl);
};


#endif // CUSTOM_TYPES_H

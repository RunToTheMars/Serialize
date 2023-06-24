#include "custom_types.h"

#include "serialize.h"

void pair::build_parameter_list (serialize_list &pl)
{
  pl.add ("x", x);
  pl.add ("y", y);
}

void vec_pair::build_parameter_list (serialize_list &pl)
{
  pl.add ("pairs", pairs);
}

void student::build_parameter_list (serialize_list &pl)
{
  pl.add ("is_valid", is_valid);
  pl.add ("name"    , name);
  pl.add ("age"     , age);
}

void student_1::build_parameter_list (serialize_list &pl)
{
  pl.add ("t", t);
  pl.add ("is_valid", is_valid);
  pl.add ("name"    , name);
  pl.add ("age"     , age);
}

void student_2::build_parameter_list (serialize_list &pl)
{
  pl.add ("is_valid", is_valid);
  pl.add ("t", t);
  pl.add ("name"    , name);
  pl.add ("age"     , age);
}

void student_3::build_parameter_list (serialize_list &pl)
{
  pl.add ("is_valid", is_valid);
  pl.add ("name"    , name);
  pl.add ("t", t);
  pl.add ("age"     , age);
}

void student_4::build_parameter_list (serialize_list &pl)
{
  pl.add ("is_valid", is_valid);
  pl.add ("name"    , name);
  pl.add ("age"     , age);
  pl.add ("t", t);
}


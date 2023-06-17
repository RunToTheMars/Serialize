#ifndef SERIALIZE_BINARY_TYPE_TRAITS_H
#define SERIALIZE_BINARY_TYPE_TRAITS_H

#include <type_traits>
#include <vector>
#include <string>
#include <memory>

//-------------------------------------------------------------------------
/// bool
///
template <typename T>
struct is_boolean : std::false_type {};

template <>
struct is_boolean<bool> : std::true_type {};

//-------------------------------------------------------------------------
/// simple
///
template <typename T>
struct is_simple : std::false_type {};

template <>
struct is_simple<int> : std::true_type {};

template <>
struct is_simple<unsigned int> : std::true_type {};

template <>
struct is_simple<size_t> : std::true_type {};

template <>
struct is_simple<double> : std::true_type {};

//-------------------------------------------------------------------------
/// string
///
template <typename T>
struct is_string : std::false_type {};

template <>
struct is_string<std::string> : std::true_type {};

//-------------------------------------------------------------------------
/// vector
///
template <typename T>
struct is_vector : std::false_type {};

template <typename T>
struct is_vector<std::vector<T>> : std::true_type {};

#endif // SERIALIZE_BINARY_TYPE_TRAITS_H

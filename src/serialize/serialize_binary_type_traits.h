#ifndef SERIALIZE_BINARY_TYPE_TRAITS_H
#define SERIALIZE_BINARY_TYPE_TRAITS_H

#include <type_traits>
#include <vector>
#include <string>
#include <map>
#include <set>

namespace binary
{
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
struct is_simple<char> : std::true_type {};

template <>
struct is_simple<int> : std::true_type {};

template <>
struct is_simple<unsigned int> : std::true_type {};

template <>
struct is_simple<size_t> : std::true_type {};

template <>
struct is_simple<float> : std::true_type {};

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

//-------------------------------------------------------------------------
/// map
///
template <typename T>
struct is_map : std::false_type {};

template <typename Key, typename Val>
struct is_map<std::map<Key, Val>> : std::true_type {};

//-------------------------------------------------------------------------
/// set
///
template <typename T>
struct is_set : std::false_type {};

template <typename T>
struct is_set<std::set<T>> : std::true_type {};
}

#endif // SERIALIZE_BINARY_TYPE_TRAITS_H

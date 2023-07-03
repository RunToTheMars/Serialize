#ifndef SERIALIZE_TEXT_TYPE_TRAITS_H
#define SERIALIZE_TEXT_TYPE_TRAITS_H

#include <type_traits>
#include <vector>
#include <string>
#include <map>
#include <set>

namespace text
{
//-------------------------------------------------------------------------
/// bool
///
template <typename T>
struct is_boolean : std::false_type {};

template <>
struct is_boolean<bool> : std::true_type {};

//-------------------------------------------------------------------------
/// stream_supported
///
template <typename T>
struct is_stream_supported : std::false_type {};

template <>
struct is_stream_supported<char> : std::true_type {};

template <>
struct is_stream_supported<int> : std::true_type {};

template <>
struct is_stream_supported<unsigned int> : std::true_type {};

template <>
struct is_stream_supported<size_t> : std::true_type {};

template <>
struct is_stream_supported<float> : std::true_type {};

template <>
struct is_stream_supported<double> : std::true_type {};

//-------------------------------------------------------------------------
/// string
///
template <typename T>
struct is_string : std::false_type {};

template <>
struct is_string<std::string> : std::true_type {};

/// pair
///
template <typename T>
struct is_pair : std::false_type {};

template <typename T1, typename T2>
struct is_pair<std::pair<T1, T2>> : std::true_type {};

//-------------------------------------------------------------------------
/// vector
///
template <typename T>
struct is_vector : std::false_type {};

template <typename T>
struct is_vector<std::vector<T>> : std::true_type {};

//-------------------------------------------------------------------------
/// set
///
template <typename T>
struct is_set : std::false_type {};

template <typename T>
struct is_set<std::set<T>> : std::true_type {};

//-------------------------------------------------------------------------
/// map
///
template <typename T>
struct is_map : std::false_type {};

template <typename T, typename V>
struct is_map<std::map<T, V>> : std::true_type {};
}

#endif // SERIALIZE_TEXT_TYPE_TRAITS_H

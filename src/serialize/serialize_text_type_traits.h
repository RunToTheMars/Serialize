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
/// is_stream_supported
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
/// is_string
///
template <typename T>
struct is_string : std::false_type {};

template <>
struct is_string<std::string> : std::true_type {};

//-------------------------------------------------------------------------
/// is_vector
///
template <typename T>
struct is_vector : std::false_type {};

template <typename T>
struct is_vector<std::vector<T>> : std::true_type {};
}

#endif // SERIALIZE_TEXT_TYPE_TRAITS_H

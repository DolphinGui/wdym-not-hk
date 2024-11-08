#pragma once

#include <cstddef>

#include "core.hpp"

template <size_t len> struct str_const {
  char value[len]{};
  constexpr str_const() = default;
  template <size_t strlen>
  constexpr str_const(const char (&lit)[strlen]) noexcept {
    for (size_t i = 0; i != len; ++i) {
      value[i] = lit[i];
    }
  }
  template <size_t begin, size_t end = len>
  constexpr str_const<end - begin> substr() const noexcept
    requires(end > begin && end <= len)
  {
    str_const<end - begin> result;
    for (size_t i = 0; i != end - begin; ++i) {
      result.value[i] = value[i + begin];
    }
    return result;
  }
  constexpr char operator[](size_t i) { return value[i]; };
  constexpr static size_t length = len;
};
// omit the null character
template <size_t strlen>
str_const(const char (&lit)[strlen]) -> str_const<strlen - 1>;

constexpr auto _impl_getchar = [](size_t i, auto str) { return str[i]; };
constexpr auto _impl_getsubstr = [](auto str) {
  return str.template substr<1>();
};

template <str_const string, auto lambda> struct make_tstr_;
template <str_const string>
using to_tstr = make_tstr_<string, _impl_getchar>::type;

template <str_const<1> string>
struct make_tstr_<string, _impl_getchar>
    : returns<tuple<char_t<_impl_getchar(0, string)>>> {};

template <str_const string>
struct make_tstr_<string, _impl_getchar>
    : returns<cat<tuple<char_t<_impl_getchar(0, string)>>,
                  to_tstr<_impl_getsubstr(string)>>> {};

using c = to_tstr<"hahahaha">;

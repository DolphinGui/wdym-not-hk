#pragma once

#include "core.hpp"

template <char... cs> using tstr = tuple<char_t<cs>...>;

template <template <typename> typename, typename, typename> struct delimiWhen__;
template <template <typename> typename pred, typename in, typename out>
using delimitWhen_ = delimiWhen__<pred, in, out>::type;

template <template <typename> typename p, typename... cs>
struct delimiWhen__<p, tuple<>, tuple<cs...>>
    : returns<pair<tuple<cs...>, tuple<>>> {};

template <template <typename> typename pred, typename... in, typename... out>
struct delimiWhen__<pred, tuple<in...>, tuple<out...>>
    : returns<if_else<
          pred<head<tuple<in...>>>::value,
          pair<tuple<out...>, tail<tuple<in...>>>,
          delimitWhen_<pred, tail<tuple<in...>>,
                       cat<tuple<out...>, tuple<head<tuple<in...>>>>>>> {};

template <template <typename> typename pred, typename in>
using delimitWhen = delimitWhen_<pred, in, tstr<>>;

struct Whitespace {};
struct Numeric {};
struct Paren {};
struct Operator {};
struct Other {};

template <typename> struct classify_;
template <typename c> using classify = classify_<c>::type;

template <char c> constexpr bool isWhitespace(char_t<c>) {
  return c == ' ' or c == '\t' or c == '\n';
}
template <char c> constexpr bool isNumeric(char_t<c>) {
  for (char const digit : "0123456789") {
    if (c == digit)
      return true;
  }
  return false;
}
template <char c> constexpr bool isParen(char_t<c>) {
  return c == '(' or c == ')';
}
template <char c> constexpr bool isOp(char_t<c>) {
  for (char const op : "-+/=*^") {
    if (c == op)
      return true;
  }
  return false;
}

template <typename c>
  requires(isWhitespace(c{}))
struct classify_<c> : returns<Whitespace> {};
template <typename c>
  requires(isNumeric(c{}))
struct classify_<c> : returns<Numeric> {};
template <typename c>
  requires(isParen(c{}))
struct classify_<c> : returns<Paren> {};
template <typename c>
  requires(isOp(c{}))
struct classify_<c> : returns<Operator> {};
template <typename> struct classify_ : returns<Other> {};

template <template <typename, typename> typename, typename, typename>
struct splitWhen__;
template <template <typename, typename> typename pred, typename in,
          typename out>
using splitWhen_ = typename splitWhen__<pred, in, out>::type;

template <template <typename, typename> typename p, typename... cs>
struct splitWhen__<p, tuple<>, tuple<cs...>>
    : returns<pair<tuple<cs...>, tuple<>>> {};

template <template <typename, typename> typename pred, typename... in,
          typename... out>
struct splitWhen__<pred, tuple<in...>, tuple<out...>>
    : returns<if_else<pred<head<in...>, head<out...>>::value,
                      pair<tuple<out...>, tuple<in...>>,
                      splitWhen_<pred, tail<tuple<in...>>,
                                 cat<tuple<out...>, first<tuple<in...>>>>>> {};

template <template <typename, typename> typename pred, typename in>
using splitWhen = splitWhen_<pred, tail<in>, first<in>>;

template <typename v> struct is_space {
  constexpr static bool value =
      v::value == ' ' or v::value == '\t' or v::value == '\n';
};

template <typename a, typename b> struct split_types {
  constexpr static bool value = !std::is_same_v<classify<a>, classify<b>>;
};

template <typename str> struct tokenize__;
template <typename str> using tokenize_ = tokenize__<str>::type;
template <typename str> using tokenize = tail<tokenize_<str>>;

template <> struct tokenize__<tstr<>> : returns<tuple<tstr<>>> {};

template <typename str>
struct tokenize__
    : returns<cat<tokenize_<typename splitWhen<split_types, str>::B>,
                  tuple<typename splitWhen<split_types, str>::A>>> {};

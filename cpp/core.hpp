#pragma once

#include <tuple>
#include <type_traits>

template <typename T> using returns = std::type_identity<T>;

template <typename T, T v> struct value_t {
  constexpr static T value = v;
};

template <char c> using char_t = value_t<char, c>;

template <typename... Ts> struct tuple;

template <typename a, typename b> struct tuple<a, b> {
  using A = a;
  using B = b;
};

template <typename... tuple> struct head_t;
template <typename... tuple> using head = head_t<tuple...>::type;

template <typename t, typename... ts>
struct head_t<tuple<t, ts...>> : returns<t> {};

template <typename t, typename... ts> struct head_t<t, ts...> : returns<t> {};

template <typename... tuple> struct first_t;
template <typename... tuple> using first = first_t<tuple...>::type;

template <typename t, typename... ts>
struct first_t<tuple<t, ts...>> : returns<tuple<t>> {};

template <typename t, typename... ts> struct first_t<t, ts...> : returns<tuple<t>> {};


template <typename tuple> struct tail_t;
template <typename... tuple> using tail = tail_t<tuple...>::type;
template <typename t, typename... ts>
struct tail_t<tuple<t, ts...>> : returns<tuple<ts...>> {};

template <bool, typename True, typename False> struct if_else_impl;
template <bool b, typename True, typename False>
using if_else = if_else_impl<b, True, False>::type;

template <typename True, typename False>
struct if_else_impl<true, True, False> : returns<True> {};
template <typename True, typename False>
struct if_else_impl<false, True, False> : returns<False> {};

template <typename a, typename b> using pair = tuple<a, b>;

template <typename... Ts> struct tuple {};

template <bool a, bool b> struct or_t {
  constexpr static bool value = a or b;
};
template <bool a, bool b> constexpr static bool or_v = or_t<a, b>::value;

template <bool a, bool b> struct and_t {
  constexpr static bool value = a and b;
};
template <bool a, bool b> constexpr static bool and_v = and_t<a, b>::value;
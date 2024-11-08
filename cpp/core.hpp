#pragma once

#include <cstddef>
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

template <typename t, typename... ts>
struct first_t<t, ts...> : returns<tuple<t>> {};

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

template <typename, typename> struct cat_impl;
template <typename T1, typename T2> using cat = cat_impl<T1, T2>::type;

template <typename... T1, typename... T2>
struct cat_impl<tuple<T1...>, tuple<T2...>> : returns<tuple<T1..., T2...>> {};

template <size_t index, typename> struct get_t;

template <typename T, typename... Ts>
struct get_t<0, tuple<T, Ts...>> : returns<T> {};

template <size_t index, typename T, typename... Ts>
struct get_t<index, tuple<T, Ts...>>
    : returns<typename get_t<index - 1, tuple<Ts...>>::type> {};

template <size_t index, typename... Ts> using get = get_t<index, Ts...>::type;

template <typename... tuple> struct drop_last_t;
template <typename... tuple> using drop_last = drop_last_t<tuple...>::type;

template <typename t, typename... ts>
struct drop_last_t<tuple<t, ts...>>
    : returns<cat<tuple<t>, drop_last<tuple<ts...>>>> {};
template <typename t> struct drop_last_t<tuple<t>> : returns<tuple<>> {};

template <typename... t> struct last_t;
template <typename tuple> using last = last_t<tuple>::type;

template <typename... t>
struct last_t<tuple<t...>> : returns<get<sizeof...(t) - 1, tuple<t...>>> {};

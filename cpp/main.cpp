#include <algorithm>
#include <fmt/format.h>
#include <string>
#include <tuple>
#include <type_traits>

template <char c, char... cs> struct head {
  constexpr static char value = c;
};

template <char... cs> struct tstring;
template <> struct tstring<> {};
template <char c, char... cs> struct tstring<c, cs...> {
  using head = tstring<c>;
  using tail = tstring<cs...>;
};

template <char c> void serialize_(std::string &str, tstring<c>) {
  str.push_back(c);
}

template <char c, char... cs>
void serialize_(std::string &str, tstring<c, cs...>) {
  str.push_back(c);
  serialize_(str, tstring<cs...>{});
}

template <char... cs> std::string format_as(tstring<cs...> tstr) {
  std::string result;
  result.reserve(sizeof...(cs));
  serialize_(result, tstr);
  return result;
}

template <typename T> using returns = std::type_identity<T>;

template <typename, typename> struct cat_impl;
template <typename T1, typename T2> using cat = cat_impl<T1, T2>::type;

template <char... as, char... bs>
struct cat_impl<tstring<as...>, tstring<bs...>>
    : returns<tstring<as..., bs...>> {};
template <bool, typename True, typename False> struct if_else_impl;
template <bool b, typename True, typename False>
using if_else = if_else_impl<b, True, False>::type;

template <typename True, typename False>
struct if_else_impl<true, True, False> : returns<True> {};
template <typename True, typename False>
struct if_else_impl<false, True, False> : returns<False> {};

template <typename... Ts> struct tuple;

template <typename a, typename b> struct tuple<a, b> {
  using A = a;
  using B = b;
};
template <typename a, typename b> using pair = tuple<a, b>;

template <typename... Ts> struct tuple {};

template <typename... T1, typename... T2>
struct cat_impl<tuple<T1...>, tuple<T2...>> : returns<tuple<T1..., T2...>> {};

template <size_t index, typename> struct get_t;

template <typename T, typename... Ts>
struct get_t<0, tuple<T, Ts...>> : returns<T> {};

template <size_t index, typename T, typename... Ts>
struct get_t<index, tuple<T, Ts...>>
    : returns<typename get_t<index - 1, tuple<Ts...>>::type> {};

template <size_t index, typename... Ts> using get = get_t<index, Ts...>::type;

template <template <char> typename, typename, typename> struct delimiWhen__;
template <template <char> typename pred, typename in, typename out>
using delimitWhen_ = delimiWhen__<pred, in, out>::type;

template <template <char> typename p, char... cs>
struct delimiWhen__<p, tstring<>, tstring<cs...>>
    : returns<pair<tstring<cs...>, tstring<>>> {};

template <template <char> typename pred, char... in, char... out>
struct delimiWhen__<pred, tstring<in...>, tstring<out...>>
    : returns<if_else<
          pred<head<in...>::value>::value,
          pair<tstring<out...>, typename tstring<in...>::tail>,
          delimitWhen_<pred, typename tstring<in...>::tail,
                       cat<tstring<out...>, tstring<head<in...>::value>>>>> {};

template <template <char> typename pred, typename in>
using delimitWhen = delimitWhen_<pred, in, tstring<>>;

struct Whitespace {};
struct Numeric {};
struct Paren {};
struct Operator {};
struct Other {};

template <char> struct classify_;
template <char c> using classify = classify_<c>::type;

constexpr bool isWhitespace(char c) {
  return c == ' ' or c == '\t' or c == '\n';
}
constexpr bool isNumeric(char const c) {
  for (char const digit : "0123456789") {
    if (c == digit)
      return true;
  }
  return false;
}
constexpr bool isParen(char const c) { return c == '(' or c == ')'; }
constexpr bool isOp(char c) {
  for (char const op : "-+/=*^") {
    if (c == op)
      return true;
  }
  return false;
}

template <char c>
  requires(isWhitespace(c))
struct classify_<c> : returns<Whitespace> {};
template <char c>
  requires(isNumeric(c))
struct classify_<c> : returns<Numeric> {};
template <char c>
  requires(isParen(c))
struct classify_<c> : returns<Paren> {};
template <char c>
  requires(isOp(c))
struct classify_<c> : returns<Operator> {};
template <char> struct classify_ : returns<Other> {};

template <template <char, char> typename, typename, typename>
struct splitWhen__;
template <template <char, char> typename pred, typename in, typename out>
using splitWhen_ = splitWhen__<pred, in, out>;

template <template <char, char> typename p, char... cs>
struct splitWhen__<p, tstring<>, tstring<cs...>>
    : returns<pair<tstring<cs...>, tstring<>>> {};

template <template <char, char> typename pred, char... in, char... out>
struct splitWhen__<pred, tstring<in...>, tstring<out...>>
    : returns<if_else<
          pred<head<in...>::value, head<out...>::value>::value,
          pair<tstring<out...>, tstring<in...>>,
          typename splitWhen_<
              pred, typename tstring<in...>::tail,
              cat<tstring<out...>, tstring<head<in...>::value>>>::type>> {};

template <template <char, char> typename pred, typename in>
using splitWhen =
    typename splitWhen_<pred, typename in::tail, typename in::head>::type;

template <char c> struct is_space {
  constexpr static bool value = c == ' ';
};

template <char a, char b> struct split_types {
  constexpr static bool value = !std::is_same_v<classify<a>, classify<b>>;
};

template <typename str> struct tokenize_;
template <typename str> using tokenize = tokenize_<str>::type;

template <> struct tokenize_<tstring<>> : returns<tuple<tstring<>>> {};

template <typename str>
struct tokenize_
    : returns<cat<tuple<typename splitWhen<split_types, str>::A>,
                  tokenize<typename splitWhen<split_types, str>::B>>> {};

using hello_world =
    tstring<'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'>;
namespace scope {
using split_result = delimitWhen<is_space, hello_world>;
using hello = split_result::A;
}; // namespace scope

using m = splitWhen<split_types, hello_world>;

using helloworld = tstring<'h', 'e', 'l', 'l', ' ', ' ', 'o', ' ', 'l', 'd'>;
using g = tokenize<helloworld>;

int main() {
  auto tstr = hello_world{};

  fmt::print("{}\n", tstr);
}
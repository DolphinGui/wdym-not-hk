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

template <typename, typename> struct append;
template <char... as, char... bs>
struct append<tstring<as...>, tstring<bs...>> : returns<tstring<as..., bs...>> {
};

template <bool, typename True, typename False> struct if_else;

template <typename True, typename False>
struct if_else<true, True, False> : returns<True> {};
template <typename True, typename False>
struct if_else<false, True, False> : returns<False> {};

template <typename a, typename b> struct pair {
  using A = a;
  using B = b;
};

template <template <char> typename, typename, typename> struct delimitWhen_;

template <template <char> typename p, char... cs>
struct delimitWhen_<p, tstring<>, tstring<cs...>>
    : returns<pair<tstring<cs...>, tstring<>>> {};

template <template <char> typename pred, char... in, char... out>
struct delimitWhen_<pred, tstring<in...>, tstring<out...>>
    : returns<typename if_else<
          pred<head<in...>::value>::value,
          pair<tstring<out...>, typename tstring<in...>::tail>,
          typename delimitWhen_<
              pred, typename tstring<in...>::tail,
              typename append<tstring<out...>, tstring<head<in...>::value>>::
                  type>::type>::type> {};

template <template <char> typename pred, typename in>
using delimitWhen = typename delimitWhen_<pred, in, tstring<>>::type;

struct Whitespace {};
struct Numeric {};
struct Paren {};
struct Operator {};
struct Other {};

template <char> struct classify;

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
struct classify<c> : returns<Whitespace> {};
template <char c>
  requires(isNumeric(c))
struct classify<c> : returns<Numeric> {};
template <char c>
  requires(isParen(c))
struct classify<c> : returns<Paren> {};
template <char c>
  requires(isOp(c))
struct classify<c> : returns<Operator> {};
template <char> struct classify : returns<Other> {};

template <template <char, char> typename, typename, typename> struct splitWhen_;

template <template <char, char> typename p, char... cs>
struct splitWhen_<p, tstring<>, tstring<cs...>>
    : returns<pair<tstring<cs...>, tstring<>>> {};

template <template <char, char> typename pred, char... in, char... out>
struct splitWhen_<pred, tstring<in...>, tstring<out...>>
    : returns<typename if_else<
          pred<head<in...>::value, head<out...>::value>::value,
          pair<tstring<out...>, tstring<in...>>,
          typename splitWhen_<
              pred, typename tstring<in...>::tail,
              typename append<tstring<out...>, tstring<head<in...>::value>>::
                  type>::type>::type> {};

template <template <char, char> typename pred, typename in>
using splitWhen =
    typename splitWhen_<pred, typename in::tail, typename in::head>::type;

template <char c> struct is_space {
  constexpr static bool value = c == ' ';
};

template <char a, char b> struct split_types {
  constexpr static bool value =
      !std::is_same_v<typename classify<a>::type, typename classify<b>::type>;
};

using hello_world =
    tstring<'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'>;
using n = delimitWhen<is_space, hello_world>;
using a = n::A;

using m = splitWhen<split_types, hello_world>;

int main() {
  auto tstr = hello_world{};

  fmt::print("{}\n", tstr);
}
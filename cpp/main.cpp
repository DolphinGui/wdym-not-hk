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
  static constexpr char getHead() noexcept { return c; }
  static constexpr auto tail() noexcept { return tstring<cs...>{}; }
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

template <auto, typename, typename> struct delimitWhen_;

template <auto p, char... cs>
struct delimitWhen_<p, tstring<>, tstring<cs...>>
    : returns<pair<tstring<cs...>, tstring<>>> {};

template <auto pred, char... in, char... out>
struct delimitWhen_<pred, tstring<in...>, tstring<out...>>
    : returns<typename if_else<
          pred(head<in...>::value),
          pair<tstring<out...>, decltype(tstring<in...>::tail())>,
          typename delimitWhen_<
              pred, decltype(tstring<in...>::tail()),
              typename append<tstring<out...>, tstring<head<in...>::value>>::
                  type>::type>::type> {};

template <auto pred, typename in>
struct delimitWhen : returns<typename delimitWhen_<pred, in, tstring<>>::type> {
};

using hello_world =
    tstring<'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'>;
using n = delimitWhen<[](char c) { return c == ' '; }, hello_world>::type;
using a = n::A;

int main() {
  auto tstr = hello_world{};

  fmt::print("{}\n", tstr);
}
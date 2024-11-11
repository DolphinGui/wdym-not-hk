#include <cstdint>
#include <fmt/format.h>
#include <type_traits>

#include "core.hpp"
#include "format.hpp"
#include "string.hpp"
#include "tokenize.hpp"

using expr = to_tstr<"2 * 3 - 4">;
using nospace = tokenize<filter<is_space, expr>>;

struct Add {};
struct Sub {};
struct Div {};
struct Mul {};
struct Assign {};
struct Exp {};

template <typename str> struct Varname : returns<str> {};
template <typename num> struct Number : returns<num> {};
template <typename operation> struct Op : returns<operation> {};
template <typename expr> struct Expression : returns<expr> {};

template <typename chars> struct parse_t;
template <typename chars> using parse = parse_t<chars>::type;

template <char c>
  requires(isNumeric(char_t<c>{}))
struct parse_t<tuple<char_t<c>>> : returns<int_t<[] {
  switch (c) {
  case '0':
    return 0;
  case '1':
    return 1;
  case '2':
    return 2;
  case '3':
    return 3;
  case '4':
    return 4;
  case '5':
    return 5;
  case '6':
    return 6;
  case '7':
    return 7;
  case '8':
    return 8;
  case '9':
    return 9;
  }
}()>> {};

template <char front, char... chars>
struct parse_t<tuple<char_t<front>, char_t<chars>...>>
    : returns<int_t<parse<tuple<char_t<front>>>::value *
                        power<10, sizeof...(chars)>::value +
                    parse<tuple<char_t<chars>...>>::value>> {};

using number = parse<to_tstr<"12">>;
static_assert(number::value == 12, "number parsing has failed");

template <typename str> struct parse_expr_t;

template <typename... chars>
  requires(... && isNumeric(chars{}))
struct parse_expr_t<tuple<chars...>> {};

int main() {}

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

template <typename chars> struct parse_num_t;
template <typename chars> using parse_num = parse_num_t<chars>::type;

template <char c>
  requires(isNumeric(char_t<c>{}))
struct parse_num_t<tuple<char_t<c>>>
    : returns<int_t<
          c == '0'
              ? 0
              : (c == '1'
                     ? 1
                     : (c == '2'
                            ? 2
                            : (c == '3'
                                   ? 3
                                   : (c == '4'
                                          ? 4
                                          : (c == '5'
                                                 ? 5
                                                 : (c == '6'
                                                        ? 6
                                                        : (c == '7'
                                                               ? 7
                                                               : (c == '8'
                                                                      ? 8
                                                                      : 9))))))))>> {
};

template <intmax_t base, size_t exponent> struct power_t;
template <intmax_t base, size_t exponent>
using power = power_t<base, exponent>::type;
template <intmax_t base> struct power_t<base, 0> : returns<int_t<1>> {};
template <intmax_t base> struct power_t<base, 1> : returns<int_t<base>> {};
template <intmax_t base, size_t exponent>
struct power_t : returns<int_t<base * power<base, exponent - 1>::value>> {};

template <char front, char... chars>
struct parse_num_t<tuple<char_t<front>, char_t<chars>...>>
    : returns<int_t<parse_num<tuple<char_t<front>>>::value *
                        power<10, sizeof...(chars)>::value +
                    parse_num<tuple<char_t<chars>...>>::value>> {};

using number = parse_num<to_tstr<"12">>;
static_assert(number::value == 12, "number parsing has failed");

template <typename str> struct parse_expr_t;

template <typename... chars>
  requires(... && isNumeric(chars{}))
struct parse_expr_t<tuple<chars...>> {};

int main() {}

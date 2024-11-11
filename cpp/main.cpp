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

struct OpenParen {};
struct ClosedParen {};

template <typename str> struct Varname;
template <char... cs> struct Varname<tstr<cs...>> : returns<tstr<cs...>> {};

template <typename num> struct Number;
template <intmax_t num> struct Number<int_t<num>> : returns<int_t<num>> {};

template <typename operation> struct Op;
template <> struct Op<Add> : returns<Add> {};
template <> struct Op<Sub> : returns<Sub> {};
template <> struct Op<Div> : returns<Div> {};
template <> struct Op<Mul> : returns<Mul> {};
template <> struct Op<Assign> : returns<Assign> {};
template <> struct Op<Exp> : returns<Exp> {};

template <typename expr> struct Expression;
template <typename... es>
struct Expression<tuple<es...>> : returns<tuple<es...>> {};

template <typename chars> struct parse_tok_t;
template <typename chars> using parse_tok = parse_tok_t<chars>::type;

template <char c>
  requires(isNumeric(char_t<c>{}))
struct parse_tok_t<tuple<char_t<c>>> : returns<int_t<[] {
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
  requires(isNumeric(char_t<front>{}))
struct parse_tok_t<tuple<char_t<front>, char_t<chars>...>>
    : returns<int_t<parse_tok<tuple<char_t<front>>>::value *
                        power<10, sizeof...(chars)>::value +
                    parse_tok<tuple<char_t<chars>...>>::value>> {};

template <> struct parse_tok_t<tuple<char_t<'+'>>> : returns<Add> {};
template <> struct parse_tok_t<tuple<char_t<'-'>>> : returns<Sub> {};
template <> struct parse_tok_t<tuple<char_t<'*'>>> : returns<Mul> {};
template <> struct parse_tok_t<tuple<char_t<'/'>>> : returns<Div> {};
template <> struct parse_tok_t<tuple<char_t<'^'>>> : returns<Exp> {};
template <> struct parse_tok_t<tuple<char_t<'='>>> : returns<Assign> {};

template <> struct parse_tok_t<tuple<char_t<'('>>> : returns<OpenParen> {};
template <> struct parse_tok_t<tuple<char_t<')'>>> : returns<ClosedParen> {};

template <typename> struct is_closed_t;
template <> struct is_closed_t<ClosedParen> : true_t {};
template <typename> struct is_closed_t : false_t {};

template <char... cs>
  requires(... && (!isNumeric(char_t<cs>{}) && !isParen(char_t<cs>{}) &&
                   !isWhitespace(char_t<cs>{}) && !isOp(char_t<cs>{})))
struct parse_tok_t<tstr<cs...>> : returns<Varname<tstr<cs...>>> {};

template <typename in> struct parse_t;
template <typename in> using parse = parse_t<in>::type;

template <char... cs>
struct parse_t<tstr<cs...>>
    : returns<apply<parse_tok_t, tokenize<filter<is_space, tstr<cs...>>>>> {};

template <typename parsed> struct parenthize_t;
template <typename parsed> using parenthize = parenthize_t<parsed>::type;

template <typename e>
  requires(!std::is_same_v<e, OpenParen> && !std::is_same_v<e, ClosedParen>)
struct parenthize_t<tuple<e>> : returns<tuple<e>> {};

template <typename e, typename... es>
  requires(!std::is_same_v<e, OpenParen> && !std::is_same_v<e, ClosedParen>)
struct parenthize_t<tuple<e, es...>>
    : returns<push<e, parenthize<tuple<es...>>>> {};

template <typename e, typename... es>
  requires(std::is_same_v<e, OpenParen>)
struct parenthize_t<tuple<e, es...>>
    : returns<push<typename delimitWhen<is_closed_t, tuple<es...>>::A,
                   typename delimitWhen<is_closed_t, tuple<es...>>::B>> {};

using n = parse_tok<to_tstr<"/">>;

using number = parse_tok<to_tstr<"12">>;

using var = parse_tok<to_tstr<"a">>;

using l = tokenize<filter<is_space, to_tstr<"12 - 3">>>;
using expression = parse<to_tstr<"v =(12 - 3) ^ 4">>;

using par = parenthize<expression>;

static_assert(number::value == 12, "number parsing has failed");

int main() {}

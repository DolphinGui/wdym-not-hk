#include "core.hpp"
#include "string.hpp"

#include <fmt/format.h>
#include <type_traits>

template <char... cs> using tstring = tuple<char_t<cs>...>;

template <char c> void serialize_(std::string &str, char_t<c>) {
  str.push_back(c);
}

template <template <typename... Ts> typename tuple, typename t>
void serialize_(std::string &str, tuple<t>) {
  serialize_(str, t{});
}

template <template <typename... Ts> typename tuple, typename t, typename... ts>
void serialize_(std::string &str, tuple<t, ts...>) {
  serialize_(str, t{});
  serialize_(str, tuple<ts...>{});
}

template <template <typename... Ts> typename tuple, typename... ts>
std::string format_as(tuple<ts...> tup) {
  std::string result;
  result.reserve(sizeof...(ts));
  serialize_(result, tup);
  return result;
}

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
using delimitWhen = delimitWhen_<pred, in, tstring<>>;

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

template <typename str> struct tokenize_;
template <typename str> using tokenize = tokenize_<str>::type;

template <> struct tokenize_<tstring<>> : returns<tuple<tstring<>>> {};

template <typename str>
struct tokenize_
    : returns<cat<tuple<typename splitWhen<split_types, str>::A>,
                  tokenize<typename splitWhen<split_types, str>::B>>> {};

template <template <typename> typename pred, typename in, typename out>
struct filter__;
template <template <typename> typename pred, typename in, typename out>
using filter_ = filter__<pred, in, out>::type;

template <template <typename> typename pred, typename out>
struct filter__<pred, tstring<>, out> : returns<out> {};
template <template <typename> typename pred, typename in, typename out>
struct filter__
    : returns<if_else<
          pred<typename in::head>::value, filter_<pred, typename in::tail, out>,
          filter_<pred, typename in::tail, cat<typename in::head, out>>>> {};

template <template <typename> typename pred, typename in>
using filter = filter_<pred, in, tstring<>>;

template <template <typename> typename pred,
          template <bool, bool> typename comb, typename str>
struct multi_;
template <template <typename> typename pred,
          template <bool, bool> typename comb, typename str>
constexpr static bool multi = multi_<pred, comb, str>::value;

template <template <typename> typename pred,
          template <bool, bool> typename comb, typename a, typename b>
struct multi_<pred, comb, tuple<a, b>> {
  constexpr static bool value = comb<pred<a>::value, pred<b>::value>::value;
};

template <template <typename> typename pred,
          template <bool, bool> typename comb, typename c, typename... cs>
struct multi_<pred, comb, tuple<c, cs...>> {
  constexpr static bool value =
      comb<pred<c>::value, multi<pred, comb, tuple<cs...>>>::value;
};
template <template <typename> typename pred, typename str>
constexpr static bool any = multi<pred, or_t, str>;
template <template <typename> typename pred, typename str>
constexpr static bool all = multi<pred, and_t, str>;

using hello_world = to_tstr<"Hello world!">;
namespace scope {
using split_result = delimitWhen<is_space, hello_world>;
using hello = split_result::A;
static_assert(any<is_space, hello_world>, "any or is_space does not work");
}; // namespace scope

using m = splitWhen<split_types, hello_world>;

using expr = tstring<'h', 'e', 'l', '1', 'o', 'l', 'd'>;
static_assert(!any<is_space, expr>, "any or is_space does not work");
using g = tokenize<expr>;

int main() {
  auto tstr = hello_world{};

  fmt::print("{}\n", tstr);
}
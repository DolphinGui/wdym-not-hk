#pragma once

#include "core.hpp"
#include "fmt/format.h"
#include <fmt/base.h>
#include <type_traits>

template <typename T, T v> inline auto format_as(value_t<T, v>) { return v; }

template <typename T, typename... Ts> struct fmt::formatter<tuple<T, Ts...>> {
  constexpr inline auto
  parse(format_parse_context &ctx) -> format_parse_context::iterator {
    return ctx.end();
  }

  inline auto format(tuple<T, Ts...> t,
                     format_context &ctx) const -> format_context::iterator {
    auto out = ctx.out();
    *out++ = '[';
    if constexpr (sizeof...(Ts) > 0)
      out = impl(out, drop_last<tuple<T, Ts...>>{});
    *out++ = fmt::format_to(out, "{}]", last<tuple<T, Ts...>>{});
    return out;
  }

private:
  template <typename Type, Type val>
  inline constexpr auto format_impl(auto &&out,
                                    value_t<Type, val>) const noexcept {
    if constexpr (std::is_same_v<Type, char>)
      return fmt::format_to(out, "{}", val);
    return fmt::format_to(out, "{} ", val);
  }

  template <typename A, typename... As>
  inline constexpr auto impl(auto &&out, tuple<A, As...>) const noexcept {
    return impl(format_impl(out, A{}), tuple<As...>{});
  }

  template <typename A>
  inline constexpr auto impl(auto &&out, tuple<A> tup) const noexcept {
    return format_impl(out, A{});
  }
};

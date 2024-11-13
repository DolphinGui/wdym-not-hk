#pragma once

#include "core.hpp"
#include "string.hpp"
#include "tokenize.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

// I could implement a hash in
template <size_t l> constexpr size_t str_hash(const char (&str)[l]) {
  size_t hash = 0x811c9dc5;
  for (int i = 0; i != l; ++i) {
    hash ^= (unsigned char)str[i];
    hash *= 0x01000193;
  }
  return hash;
}

template <typename str> struct hash_t;
template <char... c>
struct hash_t<tstr<c...>> : returns<value_t<size_t, str_hash({c...})>> {};
template <typename str> using hash = hash_t<str>::type;

template <typename key, typename map> struct access_t;
template <typename str, typename map>
using access = typename access_t<str, map>::type;

template <size_t key, size_t h, size_t v>
requires(key == h)
struct access_t<index_t<key>, tuple<pair<index_t<h>, int_t<v>>>>
    : returns<int_t<v>> {};

template <size_t key, size_t hash, size_t value, typename... others>
  requires(sizeof...(others) > 0)
struct access_t<value_t<size_t, key>,
                tuple<pair<index_t<hash>, int_t<value>>, others...>>
    : returns<if_else<key == hash, int_t<value>,
                      access<index_t<key>, tuple<others...>>>> {};

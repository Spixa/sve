#pragma once

#include <functional>
#include <string_view>
 
namespace sve {
  // from: https://stackoverflow.com/a/57595105
  template <typename T, typename... Rest>
  void hashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
    seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    (hashCombine(seed, rest), ...);
  }

  static bool endsWith(std::string_view str, std::string_view suffix)
  {
    return str.size() >= suffix.size() && 0 == str.compare(str.size()-suffix.size(), suffix.size(), suffix);
  }
 
}  // namespace sve
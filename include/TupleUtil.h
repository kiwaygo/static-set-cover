#pragma once

#include <cstddef>
#include <tuple>
#include <utility>

template <typename tTgtTuple, typename tSrcTuple, std::size_t... tIs,
          std::size_t... tJs>
constexpr void replace(tTgtTuple &aTgt, const tSrcTuple &aSrc,
                       std::index_sequence<tIs...>,
                       std::index_sequence<tJs...>) {
  ((std::get<tJs>(aTgt) = std::get<tIs>(aSrc)), ...);
}

template <typename tTuple, std::size_t... tIs>
constexpr auto reorder(const tTuple &aTuple, std::index_sequence<tIs...>) {
  return std::make_tuple(std::get<tIs>(aTuple)...);
}

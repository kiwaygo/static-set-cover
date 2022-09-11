#pragma once

#include <cstddef>
#include <utility>

template <std::size_t tIndex, std::size_t... tIndices>
auto prepend(std::index_sequence<tIndices...>) -> std::index_sequence<tIndex, tIndices...>;

template <std::size_t tValue, std::size_t... tIndices>
auto add(std::index_sequence<tIndices...>) -> std::index_sequence<(tIndices + tValue)...>;

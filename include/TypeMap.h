#pragma once

#include <type_traits>
#include <utility>

#include "BoolPack.h"

namespace set_cover {

struct MapItemTag {};

template <typename tKey, typename tValue> struct MapItem : public MapItemTag {
  using Key = tKey;
  using Value = tValue;

protected:
  static auto find(Key) -> Value;
};

template <typename... tMapItems> struct Map : public tMapItems... {
  static_assert(decltype(allOf(
      BoolPack<std::is_base_of_v<MapItemTag, tMapItems>...>{})){});

protected:
  using tMapItems::find...;

public:
  template <typename tKey> using Find = decltype(find(tKey{}));
};

}
#pragma once
#include "types.hpp"

#include <nlohmann/json.hpp>

namespace nlohmann {
template <>
struct adl_serializer<ColorRGB> {
    static void
    to_json(json &j, const ColorRGB &value)
    {
        j["r"] = value.r;
        j["g"] = value.g;
        j["b"] = value.b;
    }

    static void
    from_json(const json &j, ColorRGB &value)
    {
        value.r = j["r"];
        value.g = j["g"];
        value.b = j["b"];
    }
};

template <>
struct adl_serializer<Location> {
    static void
    to_json(json &j, const Location &value)
    {
        j["x"] = value.x;
        j["y"] = value.y;
    }

    static void
    from_json(const json &j, Location &value)
    {
        value.x = j["x"];
        value.y = j["y"];
    }
};

template <>
struct adl_serializer<IntVal> {
    static void
    to_json(json &j, const IntVal &value)
    {
        j = value.val;
    }

    static void
    from_json(const json &j, IntVal &value)
    {
        value.val = j;
    }
};
}  // namespace nlohmann
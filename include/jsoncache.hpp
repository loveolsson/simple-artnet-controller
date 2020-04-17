#pragma once
#include "helpers.hpp"

#include <nlohmann/json_fwd.hpp>

#include <optional>
#include <shared_mutex>
#include <utility>

class JSONCache
{
    nlohmann::json data;
    TimePoint timePoint;
    mutable std::shared_mutex mutex;

public:
    void SetData(const nlohmann::json &&);
    std::pair<std::optional<nlohmann::json>, TimePoint> GetData(TimePoint) const;
};

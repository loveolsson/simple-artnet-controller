#pragma once
#include "helpers.hpp"

#include <nlohmann/json_fwd.hpp>

#include <deque>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <utility>

class JSONCache
{
    struct Waiter {
        std::function<void(std::pair<std::optional<nlohmann::json>, TimePoint>)> fn;
        TimePoint waitStart;
        TimePoint since;
    };

    mutable std::mutex waiterMutex;
    mutable std::deque<Waiter> waiters;

    nlohmann::json data;
    TimePoint timePoint;
    mutable std::shared_mutex dataMutex;

    const TimePoint::duration maxWait;

public:
    JSONCache(const TimePoint::duration maxWait);

    void Poll();
    void SetData(const nlohmann::json &&);
    std::pair<std::optional<nlohmann::json>, TimePoint> GetData(TimePoint) const;
    void GetDataAsync(
        TimePoint t,
        std::function<void(std::pair<std::optional<nlohmann::json>, TimePoint>)>) const;
};

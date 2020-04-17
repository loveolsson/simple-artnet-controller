#include "jsoncache.hpp"

#include <nlohmann/json.hpp>

JSONCache::JSONCache(const TimePoint::duration maxWait)
    : maxWait(maxWait)
{
}

void
JSONCache::Poll()
{
    std::lock_guard lock(this->waiterMutex);
    if (this->waiters.empty()) {
        return;
    }

    auto cutoff = NowAsTimePoint() - this->maxWait;
    while (!this->waiters.empty()) {
        auto &front = this->waiters.front();
        if (front.waitStart < cutoff) {
            front.fn(this->GetData(front.since));
            this->waiters.pop_front();
        } else {
            break;
        }
    }
}

void
JSONCache::SetData(const nlohmann::json &&j)
{
    std::unique_lock lock(this->dataMutex);
    this->timePoint = NowAsTimePoint();
    this->data      = std::move(j);
}

std::pair<std::optional<nlohmann::json>, TimePoint>
JSONCache::GetData(TimePoint t) const
{
    std::shared_lock lock(this->dataMutex);
    if (t < this->timePoint) {
        return {this->data, this->timePoint};
    } else {
        return {std::nullopt, this->timePoint};
    }
}

void
JSONCache::GetDataAsync(
    TimePoint t, std::function<void(std::pair<std::optional<nlohmann::json>, TimePoint>)> fn) const
{
    std::shared_lock lock(this->dataMutex);

    // If the data has been updated since t, call the callback immediately,
    if (t < this->timePoint) {
        fn({this->data, this->timePoint});
        return;
    }

    // ...otherwise push the callback on the waiter queue
    if (this->maxWait > TimePoint::duration::zero()) {
        std::lock_guard lock(this->waiterMutex);
        this->waiters.push_back({fn, NowAsTimePoint(), t});
    } else {
        fn({std::nullopt, this->timePoint});
    }
}
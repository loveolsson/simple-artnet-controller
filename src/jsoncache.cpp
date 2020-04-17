#include "jsoncache.hpp"

#include <nlohmann/json.hpp>

void
JSONCache::SetData(const nlohmann::json &&j)
{
    std::unique_lock lock(this->mutex);
    this->timePoint = NowAsTimePoint();
    this->data      = std::move(j);
}

std::pair<std::optional<nlohmann::json>, TimePoint>
JSONCache::GetData(TimePoint t) const
{
    std::shared_lock lock(this->mutex);
    if (t < this->timePoint) {
        return {this->data, this->timePoint};
    } else {
        return {std::nullopt, this->timePoint};
    }
}
